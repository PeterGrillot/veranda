#include <SFML/Graphics.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <chrono>
#include <thread>

#define TEXT_LEFT 1200
#define TEXT_TOP 400
#define BOX_LEFT 320
#define FONT_FILENAME "RobotoCondensed-Regular.ttf"
#define NO_VALUE "NO_VALUE"

using namespace std;
using namespace chrono;

int main()
{
  // Window
  sf::RenderWindow window(sf::VideoMode(), "Arcade Runner", sf::Style::Fullscreen);
  // DEV
  // sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
  const float screenWidth = 2560;
  const float screenHeight = 1440;
  const int frameRate = 60;
  window.setFramerateLimit(frameRate);
  window.setKeyRepeatEnabled(false);

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
    None
  };
  int animationDirection = None;

  // Color
  sf::Color darkGreyColor = sf::Color(23, 23, 23, 255);

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

  // Var Vectors
  vector<sf::RectangleShape> fadeRectangle(jsonObjSize);

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
    // 1280w x 720h
    string bgPath = "assets/" + obj[index]["bg"].asString();
    if (!backgroundTexture[i].loadFromFile(bgPath, sf::IntRect(0, 0, screenWidth, screenHeight)))
      return -1;
    backgroundSprite[i].scale(sf::Vector2f(2.1f, 2.1f)); //
    backgroundTexture[i].setSmooth(true);
    backgroundSprite[i].setTexture(backgroundTexture[i]);
    backgroundSprite[i].move(sf::Vector2f(screenWidthOffset, 0));

    // BG Effect
    fadeRectangle[i].setSize(sf::Vector2f(screenWidth, screenHeight));
    fadeRectangle[i].setFillColor(darkGreyColor);
    fadeRectangle[i].move(sf::Vector2f(screenWidthOffset, 0));

    // Box Art
    // 850w x 1109h
    string barArtPath = "assets/" + obj[index]["box"].asString();
    if (!boxTexture[i].loadFromFile(barArtPath, sf::IntRect(0, 0, 1534, 2100)))
      return -1;
    boxTexture[i].setSmooth(true);
    boxSprite[i].rotate(-5.f);
    boxSprite[i].scale(sf::Vector2f(0.8f, 0.8f));
    boxSprite[i].move(sf::Vector2f(BOX_LEFT + screenWidthOffset, 300));
    boxSprite[i].setTexture(boxTexture[i]);

    // Margin
    int marginLeft = screenWidthOffset + TEXT_LEFT;

    // Logo
    // 800w x ANYh
    // If no Logo, use Text
    if (!obj[index]["logo"].empty())
    {
      string logoPath = "assets/" + obj[index]["logo"].asString();
      if (!logoTexture[i].loadFromFile(logoPath))
        return -1;
      titleText[i].setFont(font);
      titleText[i].setString(NO_VALUE);
      logoTexture[i].setSmooth(true);
      logoSprite[i].setTexture(logoTexture[i]);
      logoSprite[i].move(sf::Vector2f(880 + screenWidthOffset + (logoTexture[i].getSize().x / 2), (TEXT_TOP + 90) - (logoTexture[i].getSize().y / 2)));
    }
    else
    {
      titleText[i].setFont(font);
      titleText[i].setString(obj[index]["title"].asString());
      titleText[i].setCharacterSize(180);
      titleText[i].setFillColor(sf::Color::White);
      titleText[i].move(sf::Vector2f(marginLeft, TEXT_TOP + 40));
    }

    // DescriptionText
    string longDesc = obj[index]["description"].asString();
    size_t pos;

    for (size_t i = 0; i < longDesc.size(); i += 65)
    {
      int n = longDesc.rfind(' ', i);
      if (n != std::string::npos)
      {
        longDesc.at(n) = '\n';
      }
    }

    descriptionText[i].setFont(font);
    descriptionText[i].setString(longDesc);
    descriptionText[i].setCharacterSize(40);
    descriptionText[i].setLineSpacing(1.2);
    descriptionText[i].setFillColor(sf::Color::White);
    descriptionText[i].move(sf::Vector2f(marginLeft, TEXT_TOP + 420));
    index++;
  }

  // Camera
  sf::View view;
  view.setCenter(sf::Vector2f(screenWidth / 2, screenHeight / 2));
  view.setSize(sf::Vector2f(screenWidth, screenHeight));

  // Running
  while (window.isOpen())
  {
    sf::Event event;
    float alpha = 4.2 * frame;

    // Animate Direction
    fadeRectangle[pageNumber].setFillColor(sf::Color(23, 23, 23, -1 * alpha));
    if (animationDirection == Right && frame < frameRate)
    {
      view.move(screenWidth / frameRate, 0);
      fadeRectangle[pageNumber - 1].setFillColor(sf::Color(23, 23, 23, alpha));
      frame++;
    }
    else if (animationDirection == Left && frame < frameRate)
    {
      view.move(-screenWidth / frameRate, 0);
      fadeRectangle[pageNumber + 1].setFillColor(sf::Color(23, 23, 23, alpha));
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
          fadeRectangle[i].setFillColor(darkGreyColor);
        }
      }
    }

    while (window.pollEvent(event))
    {
      // switch to uh..switch
      // Close: q
      if (event.type == sf::Event::Closed)
        window.close();
      // Next: Right
      if (event.joystickMove.axis == sf::Joystick::X)
      {
          if (
          	event.joystickMove.position == 100 &&
          	animationDirection == None &&
          	pageNumber < pageSize)
      	{
        	animationDirection = Right;
        	pageNumber++;
      	}
      	// Prev: Left
      	if (
          	event.joystickMove.position == -100 &&
          	animationDirection == None &&
          	pageNumber > 0)
      	{
        	animationDirection = Left;
        	pageNumber--;
      	}
      	}
    }

    // Set up and draw
    window.setView(view);
    window.draw(blackBackgroundRectangle);

    for (size_t i = 0; i < obj.size(); i++)
    {
      window.draw(backgroundSprite[i]);
      window.draw(boxSprite[i]);
      window.draw(descriptionText[i]);
      if (titleText[i].getString() == NO_VALUE)
      {
        window.draw(logoSprite[i]);
      }
      else
      {
        window.draw(titleText[i]);
      }
      window.draw(fadeRectangle[i]);
    }
    window.display();

    // Run roms
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
    {
      string currentRom = "mame " + obj[pageNumber]["rom"].asString();
      const char *command = currentRom.c_str();
      system(command);
    }

    // Close
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
      window.close();
    }
  }

  return 0;
}
