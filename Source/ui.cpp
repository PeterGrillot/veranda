#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <chrono>
#include <thread>
#include <locale>
#include <codecvt>
#include <string>
#include <sstream>
#include <libgen.h>
#include <unistd.h>
#include <linux/limits.h>

#include "../Headers/initializeText.h"
#include "../Headers/ui.h"
#include "../Headers/readJson.h"

using namespace std;
using namespace chrono;

// Enums
enum Service {
  Mame,
  Console
};

enum Direction
{
  Up,
  Down,
  FullUp,
  FullDown,
  None
};

// Declare First System
Service service = Mame;
string serviceType = "";
string serviceName = "";

// Window
sf::RenderWindow window(sf::VideoMode(), "Veranda", sf::Style::Fullscreen);
//sf::RenderWindow window(sf::VideoMode(600, 400), "Veranda"); // Testing

void logError(string errorString)
{
  cerr << "Error: " << errorString << endl;
}

int buildUI()
{
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  string path;
  if (count != -1)
  {
    path = dirname(result);
  }
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

  // Layout
  const int artLeft = 400;
  const int textLeft = 820;
  const int categoryIconLeft = 2300;
  const int paginationLeft = categoryIconLeft + 24;
  const int wordWrap = 110;
  const int buttonVectorSize = 6;
  const int backgroundScaleFactor = 2.1;
  const float screenWidth = 2560;
  const float screenHeight = 1440;
  const int boxPerPage = 4;
  const float boxHeight = screenHeight / boxPerPage;
  const int frameRate = 3;

  // Window Settings
  window.setMouseCursorVisible(false);
  window.setKeyRepeatEnabled(false);
  window.setFramerateLimit(frameRate * 5); // Speed

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
  int animationDirection = None;
  bool isModalOpen = false;
  bool isWindowActive = window.hasFocus();

  // Colors
  const sf::Color darkGreyColor = sf::Color(0, 0, 0, 190);
  const sf::Color darkPurpleColor = sf::Color(30, 8, 30, 240);
  const sf::Color lazerBlueColor = sf::Color(80, 255, 200);
  const sf::Color whiteColor = sf::Color::White;

  //Declare a Font object
  sf::Font font;
  if (!font.loadFromFile(fontFilename))
  {
    logError("Cannot load font");
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
  highlightRect.setOutlineColor(lazerBlueColor);
  highlightRect.setOutlineThickness(borderSize);
  highlightRect.setPosition(sf::Vector2f(borderSize, borderSize));

  // Game Type Box
  sf::ConvexShape gameTypeBox;
  int gameTypeBoxWidth = 80;
  int gameTypeBoxHeight = 600;
  int gameTypeBoxAngleWidth = 20;

  // resize it to 5 points
  gameTypeBox.setPointCount(4);

  // Draw trapazoid for Service Type label
  gameTypeBox.setPoint(0, sf::Vector2f(0, 0));
  gameTypeBox.setPoint(1, sf::Vector2f(gameTypeBoxWidth, gameTypeBoxWidth));
  gameTypeBox.setPoint(2, sf::Vector2f(gameTypeBoxWidth, gameTypeBoxHeight - gameTypeBoxWidth));
  gameTypeBox.setPoint(3, sf::Vector2f(0, gameTypeBoxHeight));
  gameTypeBox.setPosition(sf::Vector2f(0, (screenHeight / 2 - (gameTypeBoxHeight / 2))));
  gameTypeBox.setFillColor(darkPurpleColor);

  // Game Type Label
  sf::Text gameTypeLabel = initializeText(font, serviceName, lazerBlueColor, FontSizeTitle);
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

  // Build Each Game Slot
  for (size_t i = 0; i < jsonObjSize; i++)
  {
    // Background Image: 1280w x 240
    ostringstream bgPath;
    bgPath << path << "/assets/" << serviceType << "/" << library[index]["bg"].asString();
    if (!backgroundTexture[i].loadFromFile(bgPath.str(), sf::IntRect(0, 0, screenWidth, boxHeight)))
    {
      logError(bgPath.str());
      backgroundSprite[i].setColor(sf::Color::Black);
    }
    else
    {
      backgroundSprite[i].scale(sf::Vector2f(backgroundScaleFactor, backgroundScaleFactor));
      backgroundTexture[i].setSmooth(true);
      backgroundSprite[i].setTexture(backgroundTexture[i]);
      backgroundSprite[i].move(sf::Vector2f(0, boxHeight * i));
      backgroundSprite[i].setTextureRect(sf::IntRect(0, 0, screenWidth, boxHeight / backgroundScaleFactor));
    }

    // Box or Logo Art: 560 x 340
    ostringstream artPath;
    artPath << path << "/assets/" << serviceType << "/" << library[index]["art"].asString();
    if (!artTexture[i].loadFromFile(artPath.str()))
    {
      logError(bgPath.str());
      artSprite[i].setColor(sf::Color::Black);
    }
    else
    {
      artTexture[i].setSmooth(true);
      artSprite[i].setTexture(artTexture[i]);

      int calcHeight = i * boxHeight + (boxHeight / 2) - artTexture[i].getSize().y / 2;
      int calcWidth = artTexture[i].getSize().x / 2;
      artSprite[i].rotate(-5);
      artSprite[i].move(sf::Vector2f(artLeft - calcWidth, calcHeight + 12));
    }

    // Category
    ostringstream categoryPath;
    categoryPath << path << "/assets/category-" << library[index]["category"].asString() << ".png";
    if (!categoryTexture[i].loadFromFile(categoryPath.str()))
    {
      logError(categoryPath.str());
      categorySprite[i].setColor(sf::Color::Black);
    }
    else
    {
      categoryTexture[i].setSmooth(true);
      categorySprite[i].setTexture(categoryTexture[i]);
      categorySprite[i].setPosition(sf::Vector2f(categoryIconLeft + 20, i * boxHeight + ((boxHeight - 100) / 2)));
    }

    // Title Text
    titleText[i] = initializeText(font, library[index]["title"].asString(), whiteColor, FontSizeTitle);
    titleText[i].scale(1, 1);
    titleText[i].move(sf::Vector2f(textLeft, (i * boxHeight) + 42));

    // Description Text
    string longDesc = library[index]["description"].asString();
    size_t pos;
    for (size_t i = 0; i < longDesc.size(); i += wordWrap)
    {
      int n = longDesc.rfind(' ', i);
      if (n != string::npos)
      {
        longDesc.at(n) = '\n';
      }
    }
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    wstring wideLongDesc = converter.from_bytes(longDesc);
    descriptionText[i] = initializeText(font, wideLongDesc, whiteColor, FontSizeParagraph);
    descriptionText[i].setLineSpacing(1.2);
    descriptionText[i].move(sf::Vector2f(textLeft, (i * boxHeight) + 138));
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
  sf::Text pagination = initializeText(font, "", whiteColor, FontSizeSubtitle);
  pagination.move(sf::Vector2f(paginationLeft, screenHeight - 90));

  // Modal
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
  sf::Text controlTitleText = initializeText(font, "", whiteColor, FontSizeHeading);
  controlTitleText.setPosition(sf::Vector2f(200, 100));

  // Select Instruction
  string mainControlInstructionsString = "Either player may press their Start button to begin game.";
  if (service == Console)
  {
    mainControlInstructionsString.append(" To exit game, hold down Exit for 2 seconds. Left and Right shoulder buttons are Player and Coin buttons, respectively.");
  }
  if (service == Mame)
  {
    mainControlInstructionsString.append(" To exit game, press Exit.");
  }
  sf::Text mainControlInstructions = initializeText(font, mainControlInstructionsString, whiteColor, FontSizeSubtitle);
  mainControlInstructions.setPosition(sf::Vector2f(200, 210));

  // Instruction
  sf::Text startLabel = initializeText(font, "Start", whiteColor, FontSizeControlLabel);
  startLabel.setPosition(sf::Vector2f(990, 370));
  string selectInstructionsString = "Exit";
  if (service == Console)
  {
    selectInstructionsString = "Exit (hold for 2 sec.)";
  }
  sf::Text selectLabel = initializeText(font, selectInstructionsString, whiteColor, FontSizeControlLabel);
  selectLabel.setPosition(sf::Vector2f(1270, 370));

  for (size_t i = 0; i < buttonVectorSize; i++)
  {
    controlLabel[i] = initializeText(font, "", whiteColor, FontSizeControlLabel);
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
    logError(p1_bgPath);
  controlTexture.setSmooth(true);
  controlSprite.move(sf::Vector2f(700, 580));
  controlSprite.setTexture(controlTexture);
  controlSprite.scale(sf::Vector2f(buttonScale, buttonScale));

  if (!selectTexture.loadFromFile(select_bgPath, sf::IntRect(0, 0, 900, 509)))
    logError(select_bgPath);
  selectTexture.setSmooth(true);
  selectSprite.move(sf::Vector2f(900, 420));
  selectSprite.setTexture(selectTexture);
  selectSprite.scale(sf::Vector2f(buttonScale, buttonScale));

  // Window Running
  // Animate Highlight Box
  float delta = 120;
  bool isDeltaInc = true;
  while (window.isOpen())
  {
    sf::Event event;
    // Set Pagination
    string paginationText = to_string(pageNumber + 1) + " of " + to_string(pageSize + 1);
    pagination.setString(paginationText);

    // Animate Direction: Down
    if (animationDirection == Down && frame < frameRate)
    {
      highlightRect.move(0, boxHeight / frameRate);
      if (pageNumber != 1 && pageNumber != pageSize && pageNumber != (pageSize - 1))
      {
        mainView.move(0, boxHeight / frameRate);
      }
      frame++;
    }
    // Animate Direction: Up
    else if (animationDirection == Up && frame < frameRate)
    {
      highlightRect.move(0, -1 * (boxHeight / frameRate));
      if (pageNumber != 0 && pageNumber != (pageSize - 1) && pageNumber != (pageSize - 2))
      {
        mainView.move(0, -1 * boxHeight / frameRate);
      }
      frame++;
    }
    // Animate Direction: Full Down
    else if (animationDirection == FullDown && frame < frameRate)
    {
      highlightRect.move(0, (boxHeight * pageSize) / frameRate);
      mainView.move(0, (boxHeight * (pageSize - 3)) / frameRate);
      frame++;
    }
    // Animate Direction: Full Up
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
          controlTitleText.setString(library[pageNumber]["title"].asString() + " Controls");
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

      // Change Service
      if (joystickLeft || joystickRight)
      {
        sound.play();
        if (joystickLeft)
        {
          service = Mame;
        }
        if (joystickRight)
        {
          service = Console;
        }
        servicePath.str("");
        servicePath.clear();
        servicePath << path << "/" << serviceType << ".json";

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
      window.draw(controlTitleText);
      window.draw(mainControlInstructions);
      window.draw(startLabel);
      window.draw(selectLabel);
    }
    window.draw(pagination);
    window.draw(gameTypeBox);
    window.draw(gameTypeLabel);
    window.display();
  }
  return 0;
}