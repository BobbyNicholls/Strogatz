#ifndef STROG_MAP_H
#define STROG_MAP_H

#include <SFML/Graphics.hpp>

#include "Graph.h"

class Map
{
private:
	sf::Texture& m_map_texture;
	const Graph& m_graph;
	sf::RenderTexture m_render_texture;
	sf::Sprite m_sprite;
	sf::RenderTexture m_blended_texture;
	sf::Sprite m_blended_sprite;

public:
	Map(sf::Texture& map_texture, const Graph& graph);
	void draw(sf::RenderWindow& window, const float x_move_distance, const float y_move_distance);
	void blend_textures(const int left_col, const int right_col, const bool blend_vertically);
	void blend_textures_diagonally(const int left_col, const int right_col, const bool downward_slope);
	void build_road(
		const float origin_x, const float origin_y, const float destination_x, const float destination_y
	);
};

#endif