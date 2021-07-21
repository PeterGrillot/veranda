#include <SFML/Graphics.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>
#include "data.h"

#define SHADER_FILENAME "blur.frag"
#define TEXT_LEFT 1200
#define BOXART_LEFT 400
#define FONT_FILENAME "RobotoCondensed-Regular.ttf"
#include <fstream>

int main()
{
  // Move to class
  sf::RenderWindow window(sf::VideoMode(), "Arcade Runner", sf::Style::Fullscreen);
  
  window.setFramerateLimit(60);
  window.setKeyRepeatEnabled(false);

  int index = 0;

  std::string rom = "goldnaxe1.zip";

  // json!
  std::ifstream ifs("data.json");
  Json::Reader reader;
  Json::Value obj;
  reader.parse(ifs, obj); // reader can also read strings
  std::cout << "first: " << obj[index]["name"].asString() << std::endl;

  // Shaders
  if (!sf::Shader::isAvailable())
  {
    std::cerr << "Shader are not available" << std::endl;
    return -1;
  }

  //Declare a Font object
  sf::Font font;

  //Load and check the availability of the font file
  if (!font.loadFromFile(FONT_FILENAME))
  {
      std::cout << "can't load font" << std::endl;
  }

  //Declare a Text object
  sf::Text text("",font);     
  text.setString(obj[index]["name"].asString());
        

  //Set character size
  text.setCharacterSize(120);

  //Set fill color
  text.setFillColor(sf::Color::White);
  text.move(sf::Vector2f(TEXT_LEFT, 440));//Declare a Font object
  sf::Text description("", font);
  description.setString(obj[index]["description"].asString());

  //Set character size
  description.setCharacterSize(40);

  //Set fill color
  description.setFillColor(sf::Color::White);
  description.move(sf::Vector2f(TEXT_LEFT, 640));

  // Load shaders
  sf::Shader shader;
  if (!shader.loadFromFile(SHADER_FILENAME, sf::Shader::Fragment))
  {
    std::cerr << "Error while shaders" << std::endl;
    return -1;
  }
  
  // Load a texture from a file
  sf::Texture boxArt;
  if (!boxArt.loadFromFile("assets/goldenaxe-box.jpg", sf::IntRect(0, 0, 1534, 2100)))
    return -1;

  boxArt.setSmooth(true);
  // Assign it to a sprite
  sf::Sprite box;
  // rotation
  box.rotate(-5.f); // offset relative to the current angle

  // scale
  box.scale(sf::Vector2f(0.7f, 0.7f));
  box.move(sf::Vector2f(BOXART_LEFT, 400));
  box.setTexture(boxArt);

  // Background
  sf::Texture bgImage;
  std::string bgPath = "assets/" + obj[index]["bg"].asString();
  if (!bgImage.loadFromFile(bgPath, sf::IntRect(0, 0, 1280, 900)))
    return -1;
  sf::Sprite background;
  background.scale(sf::Vector2f(2.1f, 2.1f)); // 
  bgImage.setSmooth(true);
  background.setTexture(bgImage);

  sf::View view;
  view.reset(sf::FloatRect(200.f, 200.f, 300.f, 200.f));

  sf::View view2;
  view.setCenter(sf::Vector2f(350.f, 300.f));
  view.setSize(sf::Vector2f(1200, 960));
  // view.setViewport(sf::FloatRect(0.25f, 0.25, 0.5f, 0.5f));
  // move the view at point (200, 200)
  view.setCenter(200.f, 200.f);



  shader.setUniform("source", bgImage);
  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      // use Switch
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Right)
      {
        // move the view by an offset of (100, 100) (so its final position is (300, 300))
        view.move(100.f, 100.f);
        // Move to load function
        // index = index + 1;
        // std::cout << index << std::endl;

        // if (!boxArt.loadFromFile("assets/mk-box.jpg", sf::IntRect(0, 0, 1280, 900)))
        //   return -1;
        // box.setTexture(boxArt);
        // if (!bgImage.loadFromFile("assets/mk-bg.jpg", sf::IntRect(0, 0, 1280, 900)))
        //   return -1;
        // background.setTexture(bgImage);
        // text.setString(obj[index]["name"].asString());
        // description.setString(obj[index]["description"].asString());
        // rom = "mk.zip";
      }

    }
    window.setView(view);
    window.draw(background, &shader);
    window.draw(box);
    
    window.draw(text);
    window.draw(description);
    window.display();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
    {
      std::string currentRom = "mame ";
      currentRom = currentRom + rom;
      const char *command = currentRom.c_str();
      system(command);
    }

    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
      window.close();
    }
  }

  return 0;
}