#ifndef INITIALIZE_TEXT_H
#define INITIALIZE_TEXT_H

#include <SFML/Graphics.hpp>
#include "../Headers/ui.h"

enum FontSize {
  FontSizeHeading = 72,
  FontSizeTitle = 54,
  FontSizeControlLabel = 42,
  FontSizeSubtitle = 32,
  FontSizeParagraph = 28,
};

sf::Text initializeText(sf::Font& font, sf::String text, const sf::Color& color, FontSize fontSize);

#endif
