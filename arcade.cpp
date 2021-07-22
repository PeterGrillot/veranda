#include <SFML/Graphics.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>
#include "data.h"
#include <fstream>
#include <chrono>
#include <thread>

#define SHADER_FILENAME "blur.frag"
#define TEXT_LEFT 1200
#define TEXT_TOP 340
#define BOXART_LEFT 400
#define FONT_FILENAME "RobotoCondensed-Regular.ttf"

using namespace std;
using namespace chrono;
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
  enum Direction {
    Left,
    Right,
    None
  };
  int animationDirection = None;

  // Shaders (unused for now)
  if (!sf::Shader::isAvailable())
  {
    cerr << "Shader are not available" << endl;
    return -1;
  }

  //Declare a Font object
  sf::Font font;
  if (!font.loadFromFile(FONT_FILENAME))
  {
    cerr << "Error: Cannot load font" << endl;
  }

  // Load shaders
  sf::Shader shader;
  if (!shader.loadFromFile(SHADER_FILENAME, sf::Shader::Fragment))
  {
    cerr << "Error: Cannot load Shaders" << endl;
    return -1;
  }

  // Var Vectors
  vector<sf::Texture> bgImage(jsonObjSize);
  vector<sf::RectangleShape> rect( jsonObjSize);
  vector<sf::Sprite> bgSprite(jsonObjSize);
  vector<sf::Sprite> box(jsonObjSize);
  vector<sf::Texture> boxArt(jsonObjSize);
  vector<sf::Text> title(jsonObjSize);
  vector<sf::Text> description(jsonObjSize);
 
  // Build Each Game Page
  for (size_t i = 0; i < obj.size(); i++)
  {
    int screenWidthOffset = screenWidth * i;
    // Background
    string bgPath = "assets/" + obj[index]["bg"].asString();
    if (!bgImage[i].loadFromFile(bgPath, sf::IntRect(0, 0, screenWidth, screenHeight)))
      return -1;
    bgSprite[i].scale(sf::Vector2f(2.1f, 2.1f)); // 
    bgImage[i].setSmooth(true);
    bgSprite[i].setTexture(bgImage[i]);
    bgSprite[i].move(sf::Vector2f(screenWidthOffset, 0));
    
    // BG Effect 
    rect[i].setSize(sf::Vector2f(screenWidth, screenHeight));
    rect[i].setFillColor(sf::Color(23,23,23,255));
    rect[i].move(sf::Vector2f(screenWidthOffset, 0));

    // Box Art
    string barArtPath = "assets/" + obj[index]["box"].asString();
    if (!boxArt[i].loadFromFile(barArtPath, sf::IntRect(0, 0, 1534, 2100)))
      return -1;
    boxArt[i].setSmooth(true);
    box[i].rotate(-5.f); // offset relative to the current angle
    box[i].scale(sf::Vector2f(0.8f, 0.8f));
    box[i].move(sf::Vector2f(BOXART_LEFT + (screenWidthOffset), TEXT_TOP));
    box[i].setTexture(boxArt[i]);

    // Margin
    int marginLeft = screenWidthOffset + TEXT_LEFT;
    // Title
    title[i].setFont(font);
    title[i].setString(obj[index]["title"].asString());
    title[i].setCharacterSize(120);
    title[i].setFillColor(sf::Color::White);
    title[i].move(sf::Vector2f(marginLeft, TEXT_TOP + 40));//Declare a Font object

    // Description
    description[i].setFont(font);
    description[i].setString(obj[index]["description"].asString());
    description[i].setCharacterSize(40);
    description[i].setFillColor(sf::Color::White);
    description[i].move(sf::Vector2f(marginLeft, 640));
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
    
    // Animate Direction
    cout << frame << endl;
    rect[pageNumber].setFillColor(sf::Color(23,23,23, -1 * (4*frame)));
    if (animationDirection == Right && frame < frameRate)
    {
      view.move(screenWidth / frameRate, 0);
      rect[pageNumber-1].setFillColor(sf::Color(23,23,23, 4*frame));
      frame++;
    } else if (animationDirection == Left && frame < frameRate)
    {
      view.move(-screenWidth / frameRate, 0);
      rect[pageNumber+1].setFillColor(sf::Color(23,23,23, 4*frame));
      frame++;
    } else {
      frame = 0;
      animationDirection = None;
      for (size_t i = 0; i < jsonObjSize; i++)
      {
        if(pageNumber != i)
        {
           rect[i].setFillColor(sf::Color(23,23,23, 255));
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
      if (
        event.type == sf::Event::KeyReleased &&
        event.key.code == sf::Keyboard::Right && 
        pageNumber < pageSize
      ) {
        animationDirection = Right;
        pageNumber++;
      }
      // Prev: Left
      if (
        event.type == sf::Event::KeyReleased &&
        event.key.code == sf::Keyboard::Left && 
        pageNumber > 0
      ) {
        animationDirection = Left;
        pageNumber--;
      }
    }

    // Set up and draw
    window.setView(view);
    
    for (size_t i = 0; i < obj.size(); i++)
    {
      window.draw(bgSprite[i]);
      window.draw(box[i]);
      window.draw(description[i]);
      window.draw(title[i]);
      window.draw(rect[i]);
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