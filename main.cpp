#include <SFML/Graphics.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <chrono>
#include <thread>

#include "readJson.h"

#define FONT_FILENAME "RobotoCondensed-Regular.ttf"

using namespace std;
using namespace chrono;

int main()
{
  // Window
  sf::RenderWindow window(sf::VideoMode(), "Arcade Runner", sf::Style::Fullscreen);
  // DEV
  // sf::RenderWindow window(sf::VideoMode(2560, 1440), "Veranda");
  const float screenWidth = 2560;
  const float screenHeight = 1440;
  const int boxPerPage = 4;
  const float boxHeight = screenHeight / boxPerPage;
  const int frameRate = 60;

  // Window Settings
  window.setMouseCursorVisible(false);
  window.setFramerateLimit(frameRate * 3);
  window.setKeyRepeatEnabled(false);

  // Get JSON
  Json::Value obj = readJson();

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
  int logoLeft = 280;
  int textLeft = 980;
  int wordWrap = 110;
  int buttonVectorSize = 6;

  // Color
  sf::Color darkGreyColor = sf::Color(0, 0, 0, 255);

  //Declare a Font object
  sf::Font font;
  if (!font.loadFromFile(FONT_FILENAME))
  {
    cerr << "Error: Cannot load font" << endl;
  }

  // Black Background
  sf::RectangleShape blackBackgroundRectangle;
  blackBackgroundRectangle.setSize(sf::Vector2f(screenWidth * (pageSize + 1), screenHeight));
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

  // Build Each Game Page
  for (size_t i = 0; i < obj.size(); i++)
  {
    int screenWidthOffset = screenWidth * i;
    // Background
    // 1280w x 240
    string bgPath = "assets/" + obj[index]["bg"].asString();
    if (!backgroundTexture[i].loadFromFile(bgPath, sf::IntRect(0, 0, screenWidth, boxHeight)))
      return -1;
    backgroundSprite[i].scale(sf::Vector2f(2.1f, 2.1f)); //
    backgroundTexture[i].setSmooth(true);
    backgroundSprite[i].setTexture(backgroundTexture[i]);
    backgroundSprite[i].move(sf::Vector2f(0, boxHeight * i));

    // Logo
    // 560 x ANYh
    string logoPath = "assets/" + obj[index]["logo"].asString();
    if (!logoTexture[i].loadFromFile(logoPath))
      return -1;
    logoTexture[i].setSmooth(true);
    logoSprite[i].setTexture(logoTexture[i]);

    int calcHeight = i * boxHeight + (boxHeight / 2) - logoTexture[i].getSize().y / 2;
    logoSprite[i].move(sf::Vector2f(logoLeft, calcHeight));

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

  // Build GUI
  sf::Text instructions;
  instructions.setFont(font);
  instructions.setCharacterSize(32);
  instructions.setFillColor(sf::Color::White);
  instructions.move(sf::Vector2f(screenWidth - 120, screenHeight - 90));

  // Modal
  sf::RectangleShape modalBackground;
  int modalMargin = 120;
  modalBackground.setFillColor(sf::Color(20, 0, 20, 200));
  modalBackground.setSize(sf::Vector2f(screenWidth - (modalMargin * 2), screenHeight - (modalMargin * 2)));
  modalBackground.move(sf::Vector2f(modalMargin, modalMargin));

  // Player Buttons
  sf::Texture p1_ButtonTexture;
  sf::Sprite p1_ButtonSprite;
  vector<sf::Text> p1_Label(buttonVectorSize);

  string p1_bgPath = "assets/controller.png";
  int p1_y1 = 1;
  int p1_y2 = 1;
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
    p1_Label[i].setFont(font);
    p1_Label[i].setString("");
    p1_Label[i].setCharacterSize(42);
    p1_Label[i].setFillColor(sf::Color::White);

    if (i < 3) {
      if (i == 1) {
        p1_Label[i].setPosition(sf::Vector2f(textMarginLeft + (buttonLeftMultiplier * p1_y1), 240 + textMarginTop));
      }
      else {
        p1_Label[i].setPosition(sf::Vector2f(textMarginLeft + (buttonLeftMultiplier * p1_y1), 360 + textMarginTop));
      }
      p1_y1++;
    }
    else {
      if (i == 4) {
        p1_Label[i].setPosition(sf::Vector2f(textMarginLeft + (buttonLeftMultiplier * p1_y2), 530 + textMarginTop));
      }
      else {
        p1_Label[i].setPosition(sf::Vector2f(textMarginLeft + (buttonLeftMultiplier * p1_y2), 650 + textMarginTop));
      }
      p1_y2++;
    }
  }

  if (!p1_ButtonTexture.loadFromFile(p1_bgPath, sf::IntRect(0, 0, 900, 509)))
    return -1;
  p1_ButtonTexture.setSmooth(true);
  p1_ButtonSprite.move(sf::Vector2f(400, 280));
  p1_ButtonSprite.setTexture(p1_ButtonTexture);
  p1_ButtonSprite.scale(sf::Vector2f(2, 2));

  /*
  Window Running
  */
  while (window.isOpen())
  {
    sf::Event event;

    string instructionsText = to_string(pageNumber + 1) + " of " + to_string(pageSize + 1);
    instructions.setString(instructionsText);

    // Animate Direction
    // Up
    if (animationDirection == Up && frame < frameRate)
    {
      highlightRect.move(0, -1 * (boxHeight / frameRate));
      if (pageNumber != 0 && pageNumber != (pageSize - 1) && pageNumber != (pageSize - 2))
      {
        mainView.move(0, -1 * boxHeight / frameRate);
      }
      frame++;
    }
    // Full Up
    else if (animationDirection == FullUp && frame < frameRate)
    {
      highlightRect.move(0, -1 * (boxHeight * pageSize) / frameRate);
      mainView.move(0, -1 * (boxHeight * (pageSize - 3)) / frameRate);
      frame++;
    }
    // Down
    else if (animationDirection == Down && frame < frameRate)
    {
      highlightRect.move(0, boxHeight / frameRate);
      if (pageNumber != 1 && pageNumber != pageSize && pageNumber != (pageSize - 1))
      {
        mainView.move(0, boxHeight / frameRate);
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
      bool selectButtonPressed = sf::Joystick::isButtonPressed(0, 1) || sf::Joystick::isButtonPressed(1, 1);
      // Next: Down
      if (
        joystickDown &&
        animationDirection == None)
      {
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
      if (sf::Joystick::isButtonPressed(0, 6) ||
        sf::Joystick::isButtonPressed(1, 6))
      {
        // Set Title
        controlTitle.setString(obj[pageNumber]["title"].asString() + " Controls");
        // Clear out old labels
        for (int i = 0; i < 6; i++)
        {
          p1_Label[i].setString("");
          // p1_ButtonSprite[i].setColor(sf::Color(200, 200, 255, 150));
        }
        // Add new labels
        for (int i = 0; i < obj[pageNumber]["controls"].size(); i++)
        {
          p1_Label[i].setString(obj[pageNumber]["controls"][i].asString());
          // p1_ButtonSprite[i].setColor(sf::Color(255, 255, 255));
        }
        isModalOpen = true;
      }

      // Open Modal
      if (sf::Joystick::isButtonPressed(0, 7) ||
        sf::Joystick::isButtonPressed(1, 7))
      {
        isModalOpen = false;
      }

      // Open Game
      if (sf::Joystick::isButtonPressed(0, 8) ||
        sf::Joystick::isButtonPressed(1, 8)
        )
      {
        isModalOpen = false;
        string currentRom = "mame " + obj[pageNumber]["rom"].asString();
        const char* command = currentRom.c_str();
        system(command);
      }

      // Close
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) ||
        sf::Joystick::isButtonPressed(0, 7) &&
        sf::Joystick::isButtonPressed(1, 8))
        window.close();
    }
    // Set up and draw
    window.clear();
    window.setView(mainView);
    window.draw(blackBackgroundRectangle);
    for (size_t i = 0; i < obj.size(); i++)
    {
      window.draw(backgroundSprite[i]);
      window.draw(descriptionText[i]);
      window.draw(logoSprite[i]);
      window.draw(titleText[i]);
    }
    window.draw(highlightRect);
    window.setView(guiView);
    if (isModalOpen)
    {
      window.draw(modalBackground);
      window.draw(p1_ButtonSprite);
      for (size_t i = 0; i < buttonVectorSize; i++)
      {
        window.draw(p1_Label[i]);
      }
      window.draw(controlTitle);
    }
    window.draw(instructions);
    window.display();
  }
  return 0;
}