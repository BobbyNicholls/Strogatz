#include "Distributions.h"
#include "Map.h"


enum Texture
{
	grass,
	sand,
	stone,
	mud,
};


Map::Map(sf::Texture& map_texture, const Graph& graph)
	: m_map_texture{ map_texture }, m_graph{ graph }
{
	int row{ 0 };
	int col{ 0 };
	m_render_texture.create(MAP_GRID_WIDTH * TEXTURE_WIDTH, MAP_GRID_HEIGHT * TEXTURE_WIDTH);
	m_render_texture.clear(sf::Color::Green);
	build_road_grid();
	map_textures_to_road_grid();

	for (int i{ 0 }; i < MAP_GRID_WIDTH * MAP_GRID_HEIGHT; ++i)
	{
		if (col == MAP_GRID_WIDTH)
		{
			col = 0;
			++row;
		}
		sf::Sprite sprite;
		sprite.setTexture(m_map_texture);
		sprite.setTextureRect(sf::IntRect(
			TEXTURE_WIDTH * Texture::grass,
			TEXTURE_WIDTH * uniform_distribution_int(0, 3),
			TEXTURE_WIDTH,
			TEXTURE_WIDTH
		));
		sprite.setPosition(col * TEXTURE_WIDTH_f, row * TEXTURE_WIDTH_f);
		m_render_texture.draw(sprite);
		++col;
	}
	//build_road(
	//	floor((m_graph.get_min_entity_x_pos() + m_location_offset_x) / TEXTURE_WIDTH_f),
	//	floor((avg_y_pos + m_location_offset_y) / TEXTURE_WIDTH_f),
	//	floor((m_graph.get_max_entity_x_pos() + m_location_offset_x) / TEXTURE_WIDTH_f),
	//	floor((avg_y_pos + m_location_offset_y) / TEXTURE_WIDTH_f)
	//);
	m_render_texture.display();
	m_sprite.setTexture(m_render_texture.getTexture());
	m_sprite.setPosition(-m_location_offset_x, -m_location_offset_y);
}


void Map::build_road_grid()
{
	/*
	* This function should build a grid of small integers, 8 bit, which will delineate the road grid.
	* Each anchor point is treated as some kind of building, the goal is to build a road from each 
	* anchor point's (x,y) location all the way to the y_centre. Then one big left to right road from
	* the min x point to the max x point at the height of y_centre.
	*/

	const int x_min{ //this point is now 0 on the x-axis for the grid
		static_cast<int>((m_graph.get_min_entity_x_pos() + m_location_offset_x) / TEXTURE_WIDTH_f) 
	};
	const int x_max{ 
		static_cast<int>((m_graph.get_max_entity_x_pos() + m_location_offset_x) / TEXTURE_WIDTH_f) 
	};
	const int y_min{ 
		static_cast<int>((m_graph.get_min_entity_y_pos() + m_location_offset_y) / TEXTURE_WIDTH_f) 
	};
	const int y_max{ 
		static_cast<int>((m_graph.get_max_entity_y_pos() + m_location_offset_y) / TEXTURE_WIDTH_f) 
	};
	m_road_grid.width = x_max - x_min + 1;
	m_road_grid.height = y_max - y_min + 1;
	m_road_grid.mid_y_coord = static_cast<int>(m_road_grid.height * 0.5);
	m_road_grid.grid.resize(m_road_grid.width * m_road_grid.height);

	int y_diff;
	int increment;
	int anchor_x;
	int anchor_y;
	for (sf::Vector2f anchor_pt : m_graph.m_anchor_points)
	{
		anchor_x = static_cast<int>((anchor_pt.x + m_location_offset_x) / TEXTURE_WIDTH_f) - x_min;
		anchor_y = static_cast<int>((anchor_pt.y + m_location_offset_y) / TEXTURE_WIDTH_f) - y_min;
		y_diff = m_road_grid.mid_y_coord - anchor_y;
		increment = (y_diff > 0) ? -1 : 1;

		while (y_diff != 0)
		{
			y_diff += increment;
			m_road_grid.grid[(anchor_y + y_diff) * m_road_grid.width + anchor_x] = 1;
		}
	}
	for (int i{ 0 }; i < m_road_grid.width; ++i)
	{
		m_road_grid.grid[(m_road_grid.mid_y_coord * m_road_grid.width) + i] = 1;
	}
	print_road_grid();
}


void Map::print_road_grid()
{
	for (int i{ 0 }; i < m_road_grid.height; ++i)
	{
		for (int j{ 0 }; j < m_road_grid.width; ++j)
		{
			if (static_cast<int>(m_road_grid.grid[(i * m_road_grid.width) + j])) std::cout << "|:|";
			else std::cout << "   ";
		}
		std::cout << '\n';
	}
}


void Map::map_textures_to_road_grid()
{
	// the matrix might need padding??
	int top_left{};
	int top_right{};
	int bot_left{};
	int bot_right{};
	std::vector<int> convolution_window(4);
	for (int row{ 0 }; row < (m_road_grid.height - 1); ++row)
	{
		for (int col{ 0 }; col < (m_road_grid.width - 1); ++col)
		{
			top_left = convolution_window[row * m_road_grid.width + col];
			top_right = convolution_window[row * m_road_grid.width + col + 1];
			bot_left = convolution_window[(row + 1) * m_road_grid.width + col];
			bot_left = convolution_window[(row + 1) * m_road_grid.width + col + 1];
			// case switch??
			if (top_left && !top_right && bot_left && !bot_right) blend_ltr();
			else if (!top_left && top_right && !bot_left && bot_right) blend_rtl();
			else if (top_left && top_right && !bot_left && !bot_right) blend_utd();
			else if (!top_left && !top_right && bot_left && bot_right) blend_dtu();
			else if (top_left && top_right && bot_left && !bot_right) blend_uli();
			else if (top_left && !top_right && bot_left && bot_right) blend_dli();
			else if (top_left && top_right && !bot_left && bot_right) blend_uri();
			else if (!top_left && top_right && bot_left && bot_right) blend_dri();
			else if (top_left && !top_right && !bot_left && !bot_right) blend_ulo();
			else if (!top_left && !top_right && bot_left && !bot_right) blend_dlo();
			else if (!top_left && top_right && !bot_left && !bot_right) blend_uro();
			else if (!top_left && !top_right && !bot_left && bot_right) blend_dro();
		}
	}
}


void Map::blend_ltr() // this func and the next should obviously be merged
{

}


void Map::blend_rtl()
{

}


void Map::blend_utd() // same with this and the next
{

}


void Map::blend_dtu()
{

}


void Map::blend_uli()
{

}


void Map::blend_dli()
{

}


void Map::blend_uri()
{

}


void Map::blend_dri()
{

}


void Map::blend_ulo()
{

}


void Map::blend_dlo()
{

}


void Map::blend_uro()
{

}


void Map::blend_dro()
{

}


void Map::draw(sf::RenderWindow& window, const float x_move_distance, const float y_move_distance)
{
	m_sprite.move(x_move_distance, y_move_distance);
	window.draw(m_sprite);
}


void Map::blend_textures(
	const int left_texture_col, const int right_texture_col, const bool blend_vertically
)
{
	const int panel_size{ 4 };
	const int panels_per_row{ (TEXTURE_WIDTH / panel_size) };
	const float neatness_scaler{ 0.3f };
	int row_ref{ uniform_distribution_int(0, 3) };
	float probability_denominator{ static_cast<float>(panels_per_row) };
	int row{ 0 };
	int col{ 0 };
	const int* col_or_row_ptr{ blend_vertically ? &col : &row };
	m_blended_texture.create(TEXTURE_WIDTH, TEXTURE_WIDTH);
	m_blended_texture.clear(sf::Color::Green);
	for (; row < panels_per_row; ++row)
	{
		for (; col < panels_per_row; ++col)
		{
			sf::Sprite sprite;
			sprite.setTexture(m_map_texture);
			if (uniform_distribution_float(0 + neatness_scaler, 1 - neatness_scaler) < 
				(*col_or_row_ptr / probability_denominator))
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * right_texture_col + col * panel_size,
					TEXTURE_WIDTH * row_ref + row * panel_size,
					panel_size,
					panel_size
				));
			}
			else
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * left_texture_col + col * panel_size,
					TEXTURE_WIDTH * row_ref + row * panel_size,
					panel_size,
					panel_size
				));
			}
			sprite.setPosition(
				col * static_cast<float>(panel_size), row * static_cast<float>(panel_size)
			);
			m_blended_texture.draw(sprite);
		}
		col = 0;
	}
	m_blended_texture.display();
	m_blended_sprite.setTexture(m_blended_texture.getTexture());
}


void Map::blend_textures_diagonally(
	const int left_texture_col, const int right_texture_col, const bool downward_slope
)
{
	const int panel_size{ 4 };
	const float panels_per_row{ (TEXTURE_WIDTH / panel_size) };
	int row_ref{ uniform_distribution_int(0, 3) };
	m_blended_texture.create(TEXTURE_WIDTH, TEXTURE_WIDTH);
	m_blended_texture.clear(sf::Color::Green);
	int row_mult{ downward_slope ? -1 : 1 };
	int slider{ downward_slope ? static_cast<int>(panels_per_row) : 0 };
	for (int row{ 0 }; row < panels_per_row; ++row)
	{
		for (int col{ 0 }; col < panels_per_row; ++col)
		{
			sf::Sprite sprite;
			sprite.setTexture(m_map_texture);
			if ((col + (row * row_mult) + slider) / (2 * panels_per_row) > uniform_distribution_float(0.65f, 0.85f))
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * right_texture_col + col * panel_size,
					TEXTURE_WIDTH * row_ref + row * panel_size,
					panel_size,
					panel_size
				));
			}
			else
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * left_texture_col + col * panel_size,
					TEXTURE_WIDTH * row_ref + row * panel_size,
					panel_size,
					panel_size
				));
			}
			sprite.setPosition(
				col * static_cast<float>(panel_size), row * static_cast<float>(panel_size)
			);
			m_blended_texture.draw(sprite);
		}
	}
	m_blended_texture.display();
	m_blended_sprite.setTexture(m_blended_texture.getTexture());
}


void Map::build_road(
	const float origin_x, const float origin_y, const float destination_x, const float destination_y
)
{
	float increment;
	const float x_diff{ destination_x - origin_x };
	const float y_diff{ destination_y - origin_y };
	increment = (y_diff > 0.f) ? 1.f : -1.f;
	float i{ increment };
	// travel the y-axis first
	if (y_diff != 0)
	{
		while (i != (floor(y_diff-increment)+1)) // this could enter inescapable loop when y_diff = 1
		{
			blend_textures(Texture::grass, Texture::stone, true);
			m_blended_sprite.setPosition(
				(origin_x - 1) * TEXTURE_WIDTH, (origin_y + i) * TEXTURE_WIDTH
			);
			m_render_texture.draw(m_blended_sprite);
			blend_textures(Texture::stone, Texture::grass, true);
			m_blended_sprite.setPosition(
				origin_x * TEXTURE_WIDTH, (origin_y + i) * TEXTURE_WIDTH
			);
			m_render_texture.draw(m_blended_sprite);
			i += increment;
		}
	}
	if (x_diff != 0)
	{
		increment = (x_diff > 0) ? 1.f : -1.f;
		float j{ increment };
		while (j != floor(x_diff-increment))
		{
			blend_textures(Texture::grass, Texture::stone, false);
			m_blended_sprite.setPosition(
				(origin_x + j) * TEXTURE_WIDTH, (destination_y - 1) * TEXTURE_WIDTH
			);
			m_render_texture.draw(m_blended_sprite);
			blend_textures(Texture::stone, Texture::grass, false);
			m_blended_sprite.setPosition(
				(origin_x + j) * TEXTURE_WIDTH, destination_y * TEXTURE_WIDTH
			);
			m_render_texture.draw(m_blended_sprite);
			j += increment;
		}
	}

	if (x_diff != 0 && y_diff != 0)
	{
		if (x_diff > 0) blend_textures(Texture::grass, Texture::stone, true);
		else blend_textures_diagonally(Texture::grass, Texture::stone, false);
		m_blended_sprite.setPosition(
			(origin_x - 1) * TEXTURE_WIDTH, (origin_y + i) * TEXTURE_WIDTH
		);
		m_render_texture.draw(m_blended_sprite);
		
		if (x_diff > 0) blend_textures_diagonally(Texture::grass, Texture::stone, true);
		else blend_textures(Texture::stone, Texture::grass, false);
		m_blended_sprite.setPosition(
			(origin_x - 1) * TEXTURE_WIDTH, (origin_y + i + 1) * TEXTURE_WIDTH
		);
		m_render_texture.draw(m_blended_sprite);
		
		if (x_diff > 0) blend_textures(Texture::stone, Texture::grass, false);
		else blend_textures_diagonally(Texture::stone, Texture::grass, false);
		m_blended_sprite.setPosition(
			origin_x * TEXTURE_WIDTH, destination_y * TEXTURE_WIDTH
		);
		m_render_texture.draw(m_blended_sprite);

		if (x_diff > 0) blend_textures_diagonally(Texture::stone, Texture::grass, true);
		else blend_textures(Texture::stone, Texture::grass, true);
		m_blended_sprite.setPosition(
			origin_x * TEXTURE_WIDTH, (destination_y - 1) * TEXTURE_WIDTH
		);
		m_render_texture.draw(m_blended_sprite);
	}
}
