#ifndef STROG_TEXT_H
#define STROG_TEXT_H

#include <SFML/Graphics.hpp>

void get_title_text(
	const std::string& title_text_str,
	sf::Text& text,
	sf::Font& font,
	unsigned int const size = 50
);

#endif