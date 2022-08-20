#ifndef STROG_TEXT_H
#define STROG_TEXT_H

#include <SFML/Graphics.hpp>

void get_text(
    const std::string& title_text_str,
    sf::Text& text,
    sf::Font& font,
    unsigned int const size = 20,
    float x_pos = 10.f,
    float y_pos = 5.f
);

#endif
