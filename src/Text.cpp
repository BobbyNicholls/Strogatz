#include "Text.h"

void get_title_text(
    const std::string& title_text_str, 
    sf::Text& text,
    sf::Font& font,
    unsigned int const size
)
{
    font.loadFromFile("SitkaB.ttc");
    text.setFont(font);
    text.setString(title_text_str);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::Green);
    text.setPosition(150.f, 250.f);
}
