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


#include "readJson.h"

using namespace std;
using namespace chrono;

int main()
{
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  string path;
  if (count != -1) {
    path = dirname(result);
  }

  const string fontFilename = path + "/RobotoCondensed-Regular.ttf";
  // Window
  sf::RenderWindow window(sf::VideoMode(), "Arcade Runner", sf::Style::Fullscreen);
  // DEV
  // sf::RenderWindow window(sf::VideoMode(600, 400), "Veranda");

  const float screenWidth = 2560;
  const float screenHeight = 1440;
  const int boxPerPage = 4;
  const float boxHeight = screenHeight / boxPerPage;
  const int frameRate = 60;
  // Window Settings
  window.setMouseCursorVisible(false);
  window.setKeyRepeatEnabled(false);

  // Speen
  window.setFramerateLimit(frameRate * 4);

  // Sound
  sf::SoundBuffer buffer;
  if (!buffer.loadFromFile(path + "/assets/blunk.wav"))
    return -1;
  sf::Sound sound;
  sound.setBuffer(buffer);

  // Get JSON
  Json::Value obj = readJson(path + "/mame.json");

  // Pagination
  int index = 0;
  int pageNumber = 0;
  int jsonObjSize = obj.size();
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

  // Layout
  int logoLeft = 190;
  int textLeft = 820;
  int categoryIconLeft = 2300;
  int paginationLeft = categoryIconLeft + 24;
  int wordWrap = 110;
  int buttonVectorSize = 6;

  // Color
  sf::Color darkGreyColor = sf::Color(0, 0, 0, 190);
  sf::Color darkPurpleColor = sf::Color(30, 8, 30, 190);

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
  highlightRect.setFillColor(sf::Color(0, 0, 0, 0));
  highlightRect.setOutlineColor(sf::Color(80, 255, 200));
  highlightRect.setOutlineThickness(borderSize);
  highlightRect.setPosition(sf::Vector2f(borderSize, borderSize));

  // Var Vectors
  vector<sf::Texture> backgroundTexture(jsonObjSize);
  vector<sf::Sprite> backgroundSprite(jsonObjSize);

  vector<sf::Sprite> boxSprite(jsonObjSize);
  vector<sf::Texture> boxTexture(jsonObjSize);

  vector<sf::Sprite> logoSprite(jsonObjSize);
  vector<sf::Texture> logoTexture(jsonObjSize);

  vector<sf::Text> titleText(jsonObjSize);
  vector<sf::Text> descriptionText(jsonObjSize);

  vector<sf::Texture> categoryTexture(jsonObjSize);
  vector<sf::Sprite> categorySprite(jsonObjSize);
  vector<sf::CircleShape> categoryDisc(jsonObjSize);

  // Build Each Game Page
  for (size_t i = 0; i < obj.size(); i++)
  {
    int screenWidthOffset = screenWidth * i;
    // Background
    // 1280w x 240
    ostringstream bgPath;
    bgPath << path << "/assets/" << obj[index]["bg"].asString();
    if (!backgroundTexture[i].loadFromFile(bgPath.str(), sf::IntRect(0, 0, screenWidth, boxHeight)))
      return -1;
    backgroundSprite[i].scale(sf::Vector2f(2.1f, 2.1f)); //
    backgroundTexture[i].setSmooth(true);
    backgroundSprite[i].setTexture(backgroundTexture[i]);
    backgroundSprite[i].move(sf::Vector2f(0, boxHeight * i));

    // Logo
    // 560 x ANYh
    ostringstream logoPath;
    logoPath << path << "/assets/" << obj[index]["logo"].asString();
    if (!logoTexture[i].loadFromFile(logoPath.str()))
      return -1;
    logoTexture[i].setSmooth(true);
    logoSprite[i].setTexture(logoTexture[i]);

    int calcHeight = i * boxHeight + (boxHeight / 2) - logoTexture[i].getSize().y / 2;
    logoSprite[i].move(sf::Vector2f(logoLeft, calcHeight));

    // Category
    ostringstream categoryPath;
    categoryPath << path << "/assets/" << obj[index]["category"].asString() << ".png";
    if (!categoryTexture[i].loadFromFile(categoryPath.str()))
      return -1;
    categoryTexture[i].setSmooth(true);
    categorySprite[i].setTexture(categoryTexture[i]);

    categorySprite[i].setPosition(sf::Vector2f(categoryIconLeft + 20, i * boxHeight + ((boxHeight - 100) / 2)));
    categoryDisc[i].setRadius(70);
    categoryDisc[i].setFillColor(darkPurpleColor);
    categoryDisc[i].setPosition(sf::Vector2f(categoryIconLeft, i * boxHeight + ((boxHeight - 140) / 2)));

    // Title Text
    titleText[i].setFont(font);
    titleText[i].setString(obj[index]["title"].asString());
    titleText[i].setCharacterSize(54);
    titleText[i].setFillColor(sf::Color::White);
    titleText[i].move(sf::Vector2f(textLeft, (i * boxHeight) + 42));

    // Description Text
    string longDesc = obj[index]["description"].asString();
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

  // Modal
  sf::RectangleShape modalBackground;
  int modalMargin = 120;
  modalBackground.setFillColor(sf::Color(darkPurpleColor));
  modalBackground.setSize(sf::Vector2f(screenWidth - (modalMargin * 2), screenHeight - (modalMargin * 2)));
  modalBackground.move(sf::Vector2f(modalMargin, modalMargin));

  // Player Buttons
  sf::Texture controlTexture;
  sf::Sprite controlSprite;
  vector<sf::Text> controlLabel(buttonVectorSize);

  string p1_bgPath = path + "/assets/controller.png";
  int y1 = 1;
  int y2 = 1;
  int buttonLeftMultiplier = 420;
  int textMarginLeft = 650;
  int textMarginTop = 180;

  sf::Text controlTitle;
  controlTitle.setFont(font);
  controlTitle.setString("");
  controlTitle.setCharacterSize(62);
  controlTitle.setFillColor(sf::Color::White);
  controlTitle.setPosition(sf::Vector2f(200, 180));

  for (size_t i = 0; i < buttonVectorSize; i++)
  {
    controlLabel[i].setFont(font);
    controlLabel[i].setString("");
    controlLabel[i].setCharacterSize(42);
    controlLabel[i].setFillColor(sf::Color::White);

    if (i < 3) {
      if (i == 1) {
        controlLabel[i].setPosition(sf::Vector2f(textMarginLeft + (buttonLeftMultiplier * y1), 240 + textMarginTop));
      }
      else {
        controlLabel[i].setPosition(sf::Vector2f(textMarginLeft + (buttonLeftMultiplier * y1), 360 + textMarginTop));
      }
      y1++;
    }
    else {
      if (i == 4) {
        controlLabel[i].setPosition(sf::Vector2f(textMarginLeft + (buttonLeftMultiplier * y2), 530 + textMarginTop));
      }
      else {
        controlLabel[i].setPosition(sf::Vector2f(textMarginLeft + (buttonLeftMultiplier * y2), 650 + textMarginTop));
      }
      y2++;
    }
  }

  if (!controlTexture.loadFromFile(p1_bgPath, sf::IntRect(0, 0, 900, 509)))
    return -1;
  controlTexture.setSmooth(true);
  controlSprite.move(sf::Vector2f(400, 280));
  controlSprite.setTexture(controlTexture);
  controlSprite.scale(sf::Vector2f(2, 2));

  /*
  Window Running
  */
  // Animate Highlight Box
  float delta = 120;
  bool isDeltaInc = true;
  while (window.isOpen())
  {
    sf::Event event;

    if (isDeltaInc) {
      delta++;
      if (delta == 255) {
        isDeltaInc = false;
      }
    }
    else
    {
      delta--;
      if (delta == 120) {
        isDeltaInc = true;
      }
    }
    highlightRect.setOutlineColor(sf::Color(240, 240, 120, delta));

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
      for (size_t i = 0; i < jsonObjSize; i++)
      {
        if (pageNumber != i)
        {
        }
      }
    }

    while (window.pollEvent(event))
    {
      // Events
      bool joystickUp = (event.joystickMove.axis == sf::Joystick::Y && event.joystickMove.position == -100) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
      bool joystickDown = (event.joystickMove.axis == sf::Joystick::Y && event.joystickMove.position == 100) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
      bool selectButtonPressed = sf::Joystick::isButtonPressed(0, 1) || sf::Joystick::isButtonPressed(1, 1);

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
        if (event.key.code == 6 || event.key.code == 'o')
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

          controlTitle.setString(obj[pageNumber]["title"].asString() + " Controls");
          // Clear out old labels
          for (int i = 0; i < 6; i++)
          {
            controlLabel[i].setString("");
          }
          // Add new labels
          for (int i = 0; i < obj[pageNumber]["controls"].size(); i++)
          {
            controlLabel[i].setString(obj[pageNumber]["controls"][i].asString());
          }
        }
      }

      // Open Game
      if (sf::Joystick::isButtonPressed(0, 8) ||
        sf::Joystick::isButtonPressed(1, 8))
      {
        isModalOpen = false;
        string currentRom = "mame " + obj[pageNumber]["rom"].asString();
        const char* command = currentRom.c_str();
        system(command);
      }

      // Close
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) ||
        sf::Joystick::isButtonPressed(0, 4) &&
        sf::Joystick::isButtonPressed(1, 4))
        window.close();
    }

    // Set up and draw
    window.clear();
    window.setView(mainView);
    window.draw(blackBackgroundRectangle);
    // Game Box
    for (size_t i = 0; i < obj.size(); i++)
    {
      window.draw(backgroundSprite[i]);
      window.draw(titleText[i]);
      window.draw(descriptionText[i]);
      window.draw(logoSprite[i]);
      window.draw(categoryDisc[i]);
      window.draw(categorySprite[i]);
    }
    window.draw(highlightRect);
    window.setView(guiView);
    if (isModalOpen)
    {
      window.draw(modalBackground);
      window.draw(controlSprite);
      for (size_t i = 0; i < buttonVectorSize; i++)
      {
        window.draw(controlLabel[i]);
      }
      window.draw(controlTitle);
    }
    window.draw(pagination);
    window.display();
  }
  return 0;
}