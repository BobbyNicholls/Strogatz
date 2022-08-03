#include "Text.h"

void get_title_text(
    const std::string& title_text_str, 
    sf::Text& text,
    sf::Font& font,
    unsigned int const size
)
{
	font.loadFromFile("GLSNECB.TTF");
    text.setFont(font);
    text.setString(title_text_str);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::Green);
    text.setPosition(300.f, 250.f);
}
