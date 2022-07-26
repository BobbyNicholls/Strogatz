#include "Text.h"

void get_text(
    const std::string& text_str, 
    sf::Text& text,
    sf::Font& font,
    unsigned int const size,
    float x_pos,
    float y_pos
)
{
    font.loadFromFile("SitkaB.ttc");
    text.setFont(font);
    text.setString(text_str);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::Green);
    text.setPosition(x_pos, y_pos);
}
