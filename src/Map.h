#ifndef STROG_MAP_H
#define STROG_MAP_H

#include <SFML/Graphics.hpp>

#include "Graph.h"

constexpr int TEXTURE_WIDTH{ 64 };
constexpr float TEXTURE_WIDTH_f{ 64.f };


class Map
{
private:
	const float m_location_offset_x;
	const float m_location_offset_y;
	const int m_map_grid_width;
	const float m_map_pixel_width;
	const int m_map_grid_height;
	const float m_map_pixel_height;
	const int m_panel_size{ 4 };
	const int m_panels_per_row{ (TEXTURE_WIDTH / m_panel_size) };
	const float m_neatness_scaler{ 0.3f }; // TODO: rename `roughness_scaler`? more accurate
	float m_probability_denominator{ static_cast<float>(m_panels_per_row) };
	const Graph& m_graph;
	RoadGrid m_road_grid;
	const sf::Texture& m_map_texture;
	const sf::Texture& m_anchor_texture;
	sf::RenderTexture m_render_texture;
	sf::Sprite m_sprite;
	sf::RenderTexture m_blended_texture;
	sf::Sprite m_blended_sprite;

public:
	Map(const sf::Texture& map_texture, const sf::Texture& anchor_texture, const Graph& graph);
	void build_road_grid();
	void draw(sf::RenderWindow& window, const float x_move_distance, const float y_move_distance);
	void cover_map_with_texture(const int texture_columm);
	void print_road_grid();
	void map_textures_to_road_grid(const int road_texture_columm, const int ground_texture_columm);
	void map_textures_to_anchor_points();
	// texture blending functions:
	void blend_horizontally(
		const int up_texture_col, const int down_texture_col, const int pos_x, const int pos_y
	);
	void blend_vertically(
		const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y
	);
	void blend_uli(const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y);
	void blend_dli(const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y);
	void blend_uri(const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y);
	void blend_dri(const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y);
	void blend_ulo(const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y);
	void blend_dlo(const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y);
	void blend_uro(const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y);
	void blend_dro(const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y);
	void all_one_texture(const int texture_col, const int pos_x, const int pos_y);
	void snap_entities_to_grid() const;
};

#endif