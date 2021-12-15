#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
#include <libgen.h>
#include <unistd.h>
#include <linux/limits.h>

#include "ui.h"
#include "readJson.h"

using namespace std;
using namespace chrono;

enum Service {
  Mame,
  Console
};

// Declare First System
Service service = Mame;
string serviceType = "";
string serviceName = "";


// Window
sf::RenderWindow window(sf::VideoMode(), "Veranda", sf::Style::Fullscreen);

// DEV
//sf::RenderWindow window(sf::VideoMode(600, 400), "Veranda");

int buildUI() {

  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  string path;
  if (count != -1)
    path = dirname(result);

  switch (service)
  {
  case Console:
    serviceType = "console";
    serviceName = "Console Games";
    break;

  case Mame:
    serviceType = "mame";
    serviceName = "Arcade Games";
    break;


  default:
    break;
  }

  const string fontFilename = path + "/RobotoCondensed-Regular.ttf";
  ostringstream servicePath;
  servicePath << path << "/" << serviceType << ".json";

  const float screenWidth = 2560;
  const float screenHeight = 1440;
  const int boxPerPage = 4;
  const float boxHeight = screenHeight / boxPerPage;
  const int frameRate = 3;
  // Window Settings
  window.setMouseCursorVisible(false);
  window.setKeyRepeatEnabled(false);

  // Speed
  window.setFramerateLimit(frameRate * 5);

  // Sound
  sf::SoundBuffer buffer;
  if (!buffer.loadFromFile(path + "/assets/blunk.wav"))
    cout << "Error" << endl;
  sf::Sound sound;
  sound.setBuffer(buffer);

  // Get JSON
  Json::Value cmd = readJson(servicePath.str())["cmd"].asString();
  Json::Value library = readJson(servicePath.str())["library"];
  // Pagination
  int index = 0;
  int pageNumber = 0;
  int jsonObjSize = library.size();
  int pageSize = jsonObjSize - 1;

  // Animation
  int frame = 0;
  enum Direction
  {
    Up,
    Down,
    FullUp,
    FullDown,
    None
  };
  int animationDirection = None;

  bool isModalOpen = false;
  bool isWindowActive = window.hasFocus();

  // Layout
  int artLeft = 400;
  int textLeft = 820;
  int categoryIconLeft = 2300;
  int paginationLeft = categoryIconLeft + 24;
  int wordWrap = 110;
  int buttonVectorSize = 6;
  int backgroundScaleFactor = 2.1;

  // Color
  sf::Color darkGreyColor = sf::Color(0, 0, 0, 190);
  sf::Color darkPurpleColor = sf::Color(30, 8, 30, 240);
  sf::Color lazerBlue = sf::Color(80, 255, 200);

  //Declare a Font object
  sf::Font font;
  if (!font.loadFromFile(fontFilename))
  {
    cerr << "Error: Cannot load font" << endl;
  }

  // Black Background
  sf::RectangleShape blackBackgroundRectangle;
  blackBackgroundRectangle.setSize(sf::Vector2f(screenWidth, screenHeight * (pageSize / boxPerPage)));
  blackBackgroundRectangle.setFillColor(sf::Color::Black);
  blackBackgroundRectangle.move(sf::Vector2f(0, 0));

  // Highlight Box
  int borderSize = 12;
  sf::RectangleShape highlightRect;
  highlightRect.setSize(sf::Vector2f(screenWidth - (borderSize * 2), boxHeight - (borderSize * 2)));
  highlightRect.setFillColor(sf::Color::Transparent);
  highlightRect.setOutlineColor(lazerBlue);
  highlightRect.setOutlineThickness(borderSize);
  highlightRect.setPosition(sf::Vector2f(borderSize, borderSize));

  // Game Type Box
  sf::ConvexShape gameTypeBox;
  int gameTypeBoxWidth = 80;
  int gameTypeBoxHeight = 600;
  int gameTypeBoxAngleWidth = 20;

  // resize it to 5 points
  gameTypeBox.setPointCount(4);

  // define the points
  gameTypeBox.setPoint(0, sf::Vector2f(0, 0));
  gameTypeBox.setPoint(1, sf::Vector2f(gameTypeBoxWidth, gameTypeBoxWidth));
  gameTypeBox.setPoint(2, sf::Vector2f(gameTypeBoxWidth, gameTypeBoxHeight - gameTypeBoxWidth));
  gameTypeBox.setPoint(3, sf::Vector2f(0, gameTypeBoxHeight));
  gameTypeBox.setPosition(sf::Vector2f(0, (screenHeight / 2 - (gameTypeBoxHeight / 2))));
  gameTypeBox.setFillColor(darkPurpleColor);

  // Game Type Label
  sf::Text gameTypeLabel;
  gameTypeLabel.setFont(font);
  gameTypeLabel.setString(serviceName);
  gameTypeLabel.setCharacterSize(54);
  gameTypeLabel.setFillColor(lazerBlue);
  size_t gameLabelSize = serviceName.length() * 13;
  gameTypeLabel.move(8, (screenHeight / 2) + gameLabelSize);
  gameTypeLabel.rotate(-90);

  // Var Vectors
  vector<sf::Texture> backgroundTexture(jsonObjSize);
  vector<sf::Sprite> backgroundSprite(jsonObjSize);

  vector<sf::Sprite> boxSprite(jsonObjSize);
  vector<sf::Texture> boxTexture(jsonObjSize);

  vector<sf::Sprite> artSprite(jsonObjSize);
  vector<sf::Texture> artTexture(jsonObjSize);

  vector<sf::Text> titleText(jsonObjSize);
  vector<sf::Text> descriptionText(jsonObjSize);

  vector<sf::Texture> categoryTexture(jsonObjSize);
  vector<sf::Sprite> categorySprite(jsonObjSize);

  // Build Each Game Page
  for (size_t i = 0; i < jsonObjSize; i++)
  {
    int screenWidthOffset = screenWidth * i;
    // Background
    // 1280w x 240
    ostringstream bgPath;
    bgPath << path << "/assets/" << serviceType << "/" << library[index]["bg"].asString();
    if (!backgroundTexture[i].loadFromFile(bgPath.str(), sf::IntRect(0, 0, screenWidth, boxHeight)))
      cout << "Error" << endl;
    backgroundSprite[i].scale(sf::Vector2f(backgroundScaleFactor, backgroundScaleFactor));
    backgroundTexture[i].setSmooth(true);
    backgroundSprite[i].setTexture(backgroundTexture[i]);
    backgroundSprite[i].move(sf::Vector2f(0, boxHeight * i));
    backgroundSprite[i].setTextureRect(sf::IntRect(0, 0, screenWidth, boxHeight / backgroundScaleFactor));

    // art
    // 560 x ANYh
    ostringstream artPath;
    artPath << path << "/assets/" << serviceType << "/" << library[index]["art"].asString();
    if (!artTexture[i].loadFromFile(artPath.str()))
      cout << "Error" << endl;
    artTexture[i].setSmooth(true);
    artSprite[i].setTexture(artTexture[i]);

    int calcHeight = i * boxHeight + (boxHeight / 2) - artTexture[i].getSize().y / 2;
    int calcWidth = artTexture[i].getSize().x / 2;
    artSprite[i].rotate(-5);
    artSprite[i].move(sf::Vector2f(artLeft - calcWidth, calcHeight + 12));

    // Category
    ostringstream categoryPath;
    categoryPath << path << "/assets/category-" << library[index]["category"].asString() << ".png";
    if (!categoryTexture[i].loadFromFile(categoryPath.str()))
      cout << "Error" << endl;
    categoryTexture[i].setSmooth(true);
    categorySprite[i].setTexture(categoryTexture[i]);

    categorySprite[i].setPosition(sf::Vector2f(categoryIconLeft + 20, i * boxHeight + ((boxHeight - 100) / 2)));

    // Title Text
    titleText[i].setFont(font);
    titleText[i].setString(library[index]["title"].asString());
    titleText[i].setCharacterSize(54);
    titleText[i].setFillColor(sf::Color::White);
    titleText[i].move(sf::Vector2f(textLeft, (i * boxHeight) + 42));

    // Description Text
    string longDesc = library[index]["description"].asString();
    size_t pos;

    for (size_t i = 0; i < longDesc.size(); i += wordWrap)
    {
      int n = longDesc.rfind(' ', i);
      if (n != std::string::npos)
      {
        longDesc.at(n) = '\n';
      }
    }
    descriptionText[i].setFont(font);
    descriptionText[i].setString(longDesc);
    descriptionText[i].setCharacterSize(28);
    descriptionText[i].setLineSpacing(1.2);
    descriptionText[i].setFillColor(sf::Color::White);
    descriptionText[i].move(sf::Vector2f(textLeft, (i * boxHeight) + 138));

    // Effect
    index++;
  }

  // Camera
  sf::View mainView;
  mainView.setCenter(sf::Vector2f(screenWidth / 2, screenHeight / 2));
  mainView.setSize(sf::Vector2f(screenWidth, screenHeight));

  // GUI Camera
  sf::View guiView;
  guiView.setCenter(sf::Vector2f(screenWidth / 2, screenHeight / 2));
  guiView.setSize(sf::Vector2f(screenWidth, screenHeight));

  // Build pagination
  sf::Text pagination;
  pagination.setFont(font);
  pagination.setCharacterSize(32);
  pagination.setFillColor(sf::Color::White);
  pagination.move(sf::Vector2f(paginationLeft, screenHeight - 90));

  /*
  /*  Modal
  */
  sf::RectangleShape modalBackground;
  int modalMargin = 10;
  modalBackground.setFillColor(sf::Color(darkPurpleColor));
  modalBackground.setSize(sf::Vector2f(screenWidth - (modalMargin * 2), screenHeight - (modalMargin * 2)));
  modalBackground.move(sf::Vector2f(modalMargin, modalMargin));

  // Player Buttons
  sf::Texture controlTexture;
  sf::Sprite controlSprite;
  vector<sf::Text> controlLabel(buttonVectorSize);
  string p1_bgPath = path + "/assets/controller.png";

  // Select Button
  sf::Texture selectTexture;
  sf::Sprite selectSprite;
  string select_bgPath = path + "/assets/select.png";

  int y1 = 1;
  int y2 = 1;
  float buttonScale = 1.3;
  int buttonLeftMultiplier = 220 * buttonScale;
  int textMarginLeft = 820;
  int textMarginTop = 370;

  // Game Title
  sf::Text controlTitle;
  controlTitle.setFont(font);
  controlTitle.setString("");
  controlTitle.setCharacterSize(72);
  controlTitle.setFillColor(sf::Color::White);
  controlTitle.setPosition(sf::Vector2f(200, 100));

  // Select Instruction
  sf::Text selectInstructions;
  selectInstructions.setFont(font);
  selectInstructions.setString("Either player may press their Start button to begin game. To exit while playing, either player can press their Exit button.");
  selectInstructions.setCharacterSize(32);
  selectInstructions.setFillColor(sf::Color::White);
  selectInstructions.setPosition(sf::Vector2f(200, 210));
  // Select Instruction
  sf::Text selectLabel;
  selectLabel.setFont(font);
  selectLabel.setString("Start                    Exit");
  selectLabel.setCharacterSize(42);
  selectLabel.setFillColor(sf::Color::White);
  selectLabel.setPosition(sf::Vector2f(990, 370));

  for (size_t i = 0; i < buttonVectorSize; i++)
  {
    controlLabel[i].setFont(font);
    controlLabel[i].setString("");
    controlLabel[i].setCharacterSize(42);
    controlLabel[i].setFillColor(sf::Color::White);

    if (i < 3) {
      if (i == 1) {
        controlLabel[i].setPosition(sf::Vector2f(textMarginLeft + (buttonLeftMultiplier * y1), 280 + textMarginTop));
      }
      else {
        controlLabel[i].setPosition(sf::Vector2f(textMarginLeft + (buttonLeftMultiplier * y1), 360 + textMarginTop));
      }
      y1++;
    }
    else {
      if (i == 4) {
        controlLabel[i].setPosition(sf::Vector2f(textMarginLeft + (buttonLeftMultiplier * y2), 470 + textMarginTop));
      }
      else {
        controlLabel[i].setPosition(sf::Vector2f(textMarginLeft + (buttonLeftMultiplier * y2), 550 + textMarginTop));
      }
      y2++;
    }
  }

  if (!controlTexture.loadFromFile(p1_bgPath, sf::IntRect(0, 0, 900, 509)))
    cout << "Error" << endl;
  controlTexture.setSmooth(true);
  controlSprite.move(sf::Vector2f(700, 580));
  controlSprite.setTexture(controlTexture);
  controlSprite.scale(sf::Vector2f(buttonScale, buttonScale));

  if (!selectTexture.loadFromFile(select_bgPath, sf::IntRect(0, 0, 900, 509)))
    cout << "Error" << endl;
  selectTexture.setSmooth(true);
  selectSprite.move(sf::Vector2f(900, 420));
  selectSprite.setTexture(selectTexture);
  selectSprite.scale(sf::Vector2f(buttonScale, buttonScale));

  /*
  Window Running
  */
  // Animate Highlight Box
  float delta = 120;
  bool isDeltaInc = true;
  while (window.isOpen())
  {
    sf::Event event;

    // Set Pagination
    string paginationText = to_string(pageNumber + 1) + " of " + to_string(pageSize + 1);
    pagination.setString(paginationText);

    // Animate Direction
    // Down
    if (animationDirection == Down && frame < frameRate)
    {
      highlightRect.move(0, boxHeight / frameRate);
      if (pageNumber != 1 && pageNumber != pageSize && pageNumber != (pageSize - 1))
      {
        mainView.move(0, boxHeight / frameRate);
      }
      frame++;
    }
    // Up
    else if (animationDirection == Up && frame < frameRate)
    {
      highlightRect.move(0, -1 * (boxHeight / frameRate));
      if (pageNumber != 0 && pageNumber != (pageSize - 1) && pageNumber != (pageSize - 2))
      {
        mainView.move(0, -1 * boxHeight / frameRate);
      }
      frame++;
    }
    // Full Down
    else if (animationDirection == FullDown && frame < frameRate)
    {
      highlightRect.move(0, (boxHeight * pageSize) / frameRate);
      mainView.move(0, (boxHeight * (pageSize - 3)) / frameRate);
      frame++;
    }
    // Full Up
    else if (animationDirection == FullUp && frame < frameRate)
    {
      highlightRect.move(0, -1 * (boxHeight * pageSize) / frameRate);
      mainView.move(0, -1 * (boxHeight * (pageSize - 3)) / frameRate);
      frame++;
    }
    else
    {

      frame = 0;
      animationDirection = None;
    }

    while (window.pollEvent(event))
    {
      // Events
      bool joystickUp = (event.joystickMove.axis == sf::Joystick::Y && event.joystickMove.position == -100) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
      bool joystickDown = (event.joystickMove.axis == sf::Joystick::Y && event.joystickMove.position == 100) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
      bool joystickLeft = (event.joystickMove.axis == sf::Joystick::X && event.joystickMove.position == -100) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
      bool joystickRight = (event.joystickMove.axis == sf::Joystick::X && event.joystickMove.position == 100) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
      bool isSelectPressed = sf::Joystick::isButtonPressed(0, 8) || sf::Joystick::isButtonPressed(1, 8) || sf::Joystick::isButtonPressed(0, 0) || sf::Joystick::isButtonPressed(1, 0) || sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
      bool isCloseButtonPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Joystick::isButtonPressed(0, 4) && sf::Joystick::isButtonPressed(1, 4);
      bool isModalOpenButtonPressed = sf::Joystick::isButtonPressed(0, 9) || sf::Joystick::isButtonPressed(1, 9);

      // Next: Down
      if (
        joystickDown &&
        animationDirection == None)
      {
        sound.play();
        isModalOpen = false;
        if (pageNumber < pageSize)
        {
          animationDirection = Down;
          pageNumber++;
        }
        else {
          animationDirection = FullUp;
          pageNumber = 0;
        }
      }
      // Prev: Up
      if (
        joystickUp &&
        animationDirection == None)
      {
        sound.play();
        isModalOpen = false;
        if (pageNumber > 0)
        {
          animationDirection = Up;
          pageNumber--;
        }
        else {
          animationDirection = FullDown;
          pageNumber = pageSize;
        }
      }

      // Open Modal
      if (event.JoystickButtonReleased || event.KeyReleased) {
        if (isModalOpenButtonPressed || event.key.code == 'o')
        {
          // Open Modal if close, else open
          if (isModalOpen == true)
          {
            isModalOpen = false;
          }
          else
          {
            isModalOpen = true;
          }

          controlTitle.setString(library[pageNumber]["title"].asString() + " Controls");
          // Clear out old labels
          for (int i = 0; i < 6; i++)
          {
            controlLabel[i].setString("");
          }
          // Add new labels
          for (int i = 0; i < library[pageNumber]["controls"].size(); i++)
          {
            controlLabel[i].setString(library[pageNumber]["controls"][i].asString());
          }
        }
      }

      // Open Game
      if (isSelectPressed)
      {
        isModalOpen = false;
        string romPath;
        romPath.append(cmd.asString());
        romPath.append(" ");
        romPath.append(library[pageNumber]["emulator"].asString());
        romPath.append(" ");
        romPath.append(library[pageNumber]["rom"].asString());
        const char* command = romPath.c_str();
        system(command);
      }

      // Close
      if (isCloseButtonPressed)
      {
        window.close();
      }

      if (joystickRight)
      {
        service = Console;
        servicePath.str("");
        servicePath.clear();
        servicePath << path << "/console.json";

        // Get JSON
        cmd = readJson(servicePath.str())["cmd"].asString();
        library = readJson(servicePath.str())["library"];
        buildUI();
      }
      if (joystickLeft)
      {
        service = Mame;
        servicePath.str("");
        servicePath.clear();
        servicePath << path << "/mame.json";

        // Get JSON
        cmd = readJson(servicePath.str())["cmd"].asString();
        library = readJson(servicePath.str())["library"];
        buildUI();
      }
    }
    // Draw
    window.clear();
    window.setView(mainView);
    window.draw(blackBackgroundRectangle);
    // Game Box
    for (size_t i = 0; i < jsonObjSize; i++)
    {
      window.draw(backgroundSprite[i]);
      window.draw(titleText[i]);
      window.draw(descriptionText[i]);
      window.draw(artSprite[i]);
      window.draw(categorySprite[i]);
    }
    window.draw(highlightRect);
    window.setView(guiView);
    if (isModalOpen)
    {
      window.draw(modalBackground);
      window.draw(controlSprite);
      window.draw(selectSprite);
      for (size_t i = 0; i < buttonVectorSize; i++)
      {
        window.draw(controlLabel[i]);
      }
      window.draw(controlTitle);
      window.draw(selectInstructions);
      window.draw(selectLabel);
    }
    window.draw(pagination);
    window.draw(gameTypeBox);
    window.draw(gameTypeLabel);
    window.display();
  }
  return 0;
}
