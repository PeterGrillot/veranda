#include <SFML/Graphics.hpp>
#include "../Headers/ui.h"
#include "../Headers/initializeText.h"

sf::Text initializeText(sf::Font& font, sf::String text, const sf::Color& color, FontSize fontSize)
{
  sf::Text label;
  label.setFont(font);
  label.setString(text);
  label.setCharacterSize(fontSize);
  label.setFillColor(color);
  return label;
}