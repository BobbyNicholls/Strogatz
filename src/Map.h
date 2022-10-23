#ifndef STROG_MAP_H
#define STROG_MAP_H

#include <SFML/Graphics.hpp>

#include "Graph.h"

constexpr int MAP_GRID_WIDTH{ 200 };
constexpr int MAP_GRID_HEIGHT{ 140 };
constexpr int TEXTURE_WIDTH{ 64 };
constexpr float TEXTURE_WIDTH_f{ 64.f };


struct RoadGrid
{
	std::vector<uint8_t> grid;
	int width;
	int height;
	int mid_y_coord; // for the horizonal road's y coordinate
};


class Map
{
private:
	const float m_location_offset_x{ MAP_GRID_WIDTH * TEXTURE_WIDTH_f * 0.5f };
	const float m_location_offset_y{ MAP_GRID_HEIGHT * TEXTURE_WIDTH_f * 0.5f };
	const Graph& m_graph;
	RoadGrid m_road_grid;
	sf::Texture& m_map_texture;
	sf::RenderTexture m_render_texture;
	sf::Sprite m_sprite;
	sf::RenderTexture m_blended_texture;
	sf::Sprite m_blended_sprite;

public:
	Map(sf::Texture& map_texture, const Graph& graph);
	void build_road_grid();
	void draw(sf::RenderWindow& window, const float x_move_distance, const float y_move_distance);
	void blend_textures(const int left_col, const int right_col, const bool blend_vertically);
	void blend_textures_diagonally(const int left_col, const int right_col, const bool downward_slope);
	void build_road(
		const float origin_x, const float origin_y, const float destination_x, const float destination_y
	);
	void print_road_grid();
};

#endif