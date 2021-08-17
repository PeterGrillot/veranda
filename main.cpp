#include <SFML/Graphics.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <chrono>
#include <thread>

#define FONT_FILENAME "RobotoCondensed-Regular.ttf"
#define NO_VALUE "NO_VALUE"

using namespace std;
using namespace chrono;

int main()
{
  // Window
  // sf::RenderWindow window(sf::VideoMode(), "Arcade Runner", sf::Style::Fullscreen);
  // DEV
  sf::RenderWindow window(sf::VideoMode(2560, 1440), "Veranda");
  const float screenWidth = 2560;
  const float screenHeight = 1440;
  const float boxHeight = screenHeight / 4;
  const int frameRate = 60;
  window.setFramerateLimit(frameRate * 3);
  window.setKeyRepeatEnabled(false);

  // Timer
  int timer = 0;
  int timeout = 1200; // 20 seconds

  // JSON
  ifstream ifs("data.json");
  Json::Reader reader;
  Json::Value obj;
  reader.parse(ifs, obj);

  // Pagination
  int index = 0;
  int pageNumber = 0;
  int jsonObjSize = obj.size();
  int pageSize = jsonObjSize - 1;

  // Animation
  int frame = 0;
  enum Direction
  {
    Left,
    Right,
    Up,
    Down,
    Full,
    None
  };
  int animationDirection = None;

  // Layout
  int logoLeft = 280;
  int textLeft = 980;
  int wordWrap = 110;

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
  blackBackgroundRectangle.setFillColor(sf::Color(0, 0, 0));
  blackBackgroundRectangle.move(sf::Vector2f(0, 0));

  // GUI
  sf::Text instructions;
  instructions.setFont(font);
  instructions.setCharacterSize(32);
  instructions.setFillColor(sf::Color::White);
  instructions.move(10, 22);

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
    titleText[i].setFont(font);
    titleText[i].setString(NO_VALUE);
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
  guiView.setViewport(sf::FloatRect(0.8f, 0.95f, 0.25f, 0.5f));

  // Running
  while (window.isOpen())
  {
    sf::Event event;
    float alpha = 4.2 * frame;

    // Build GUI
    string instructionsText = "Up/Down to Cycle >> Press any buttons to select >> " + to_string(pageNumber + 1) + " of " + to_string(pageSize + 1);
    instructions.setString(instructionsText);

    // Animate Direction
    if (animationDirection == Up && frame < frameRate)
    {
      highlightRect.move(0, -1 * (boxHeight / frameRate));

      if (pageNumber != pageSize)
      {
        cout << "mv" << endl;
      }

      if (pageNumber != 0 && pageNumber != (pageSize - 1) && pageNumber != (pageSize - 2))
      {
        mainView.move(0, -1 * boxHeight / frameRate);
      }
      frame++;
    }
    else if (animationDirection == Down && frame < frameRate)
    {
      highlightRect.move(0, boxHeight / frameRate);
      if (pageNumber != 1 && pageNumber != pageSize && pageNumber != (pageSize - 1))
      {
        mainView.move(0, boxHeight / frameRate);
      }
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
      bool joystickRight = (event.joystickMove.axis == sf::Joystick::X && event.joystickMove.position == 100) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
      bool joystickLeft = (event.joystickMove.axis == sf::Joystick::X && event.joystickMove.position == -100) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
      bool joystickUp = (event.joystickMove.axis == sf::Joystick::Y && event.joystickMove.position == -100) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
      bool joystickDown = (event.joystickMove.axis == sf::Joystick::Y && event.joystickMove.position == -100) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
      bool selectButtonPressed = sf::Joystick::isButtonPressed(0, 1) || sf::Joystick::isButtonPressed(1, 1);
      // Next: Down => 100
      if (
        joystickDown &&
        animationDirection == None &&
        pageNumber < pageSize)
      {
        animationDirection = Down;
        pageNumber++;
        timeout = 2400;
      }
      // Prev: Up
      if (
        joystickUp &&
        animationDirection == None &&
        pageNumber > 0)
      {
        animationDirection = Up;
        pageNumber--;
        timeout = 2400;
      }

      // Open
      if (event.joystickButton.button == 0)
      {
      }
      // Run roms
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) || selectButtonPressed)
      {
        string currentRom = "mame " + obj[pageNumber]["rom"].asString();
        const char* command = currentRom.c_str();
        system(command);
      }

      // Close
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || event.type == sf::Event::Closed)
        window.close();
    }

    // Set up and draw
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
    window.draw(instructions);
    window.display();

    // DEV
    if (sf::Joystick::isButtonPressed(0, 1))
    {
      cout << "button 1 pressed" << endl;
    }
    if (sf::Joystick::isButtonPressed(0, 1))
    {
      cout << "button 1 pressed" << endl;
    }
    if (sf::Joystick::isButtonPressed(0, 2))
    {
      cout << "button 2 pressed" << endl;
    }
    if (sf::Joystick::isButtonPressed(0, 3))
    {
      cout << "button 3 pressed" << endl;
    }
    if (sf::Joystick::isButtonPressed(0, 4))
    {
      cout << "button 4 pressed" << endl;
    }
    if (sf::Joystick::isButtonPressed(0, 5))
    {
      cout << "button 5 pressed" << endl;
    }
    if (sf::Joystick::isButtonPressed(0, 6))
    {
      cout << "button 6 pressed" << endl;
    }
    if (sf::Joystick::isButtonPressed(0, 7))
    {
      cout << "button 6 pressed" << endl;
    }
    if (sf::Joystick::isButtonPressed(0, 8))
    {
      cout << "button 6 pressed" << endl;
    }
  }

  return 0;
}
