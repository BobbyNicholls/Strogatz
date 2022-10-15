#ifndef STROG_MAP_H
#define STROG_MAP_H

#include <SFML/Graphics.hpp>

class Map
{
private:
	sf::Texture& m_map_texture;
	sf::RenderTexture m_render_texture;
	sf::Sprite m_sprite;
	sf::RenderTexture m_blended_texture;
	sf::Sprite m_blended_sprite;

public:
	Map(sf::Texture& map_texture);
	void draw(sf::RenderWindow& window, const float x_move_distance, const float y_move_distance);
	void blend_textures(const int left_col, const int right_col, const bool blend_vertically);
	void blend_textures_diagonally(const int left_col, const int right_col, const bool downward_slope);
	void build_road(
		const int origin_x, const int origin_y, const int destination_x, const int destination_y
	);
};

#endif