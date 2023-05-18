#include "Distributions.h"
#include "Map.h"


enum Texture
{
	grass,
	sand,
	stone,
	mud,
};


Map::Map(sf::Texture& map_texture, sf::Texture& anchor_texture, const Graph& graph)
	: m_location_offset_x { graph.get_min_x() - (2 * TEXTURE_WIDTH_f) },
	m_location_offset_y{ graph.get_min_y() - (2 * TEXTURE_WIDTH_f) },
	m_map_grid_width{ static_cast<int>((graph.get_width() + (4 * TEXTURE_WIDTH)) / TEXTURE_WIDTH) + 1 },
	m_map_pixel_width { graph.get_width() + (4 * TEXTURE_WIDTH) },
	m_map_grid_height{ static_cast<int>((graph.get_height() + (4 * TEXTURE_WIDTH)) / TEXTURE_WIDTH) + 1 },
	m_map_pixel_height{ graph.get_height() + (4 * TEXTURE_WIDTH) },
	m_map_texture{ map_texture }, 
	m_anchor_texture{ anchor_texture }, 
	m_graph{ graph }
{
	m_render_texture.create(m_map_pixel_width, m_map_pixel_height);
	m_render_texture.clear(sf::Color::Green);
	cover_map_with_texture(Texture::grass);
	build_road_grid();
	map_textures_to_road_grid(Texture::stone, Texture::grass);
	map_textures_to_anchor_points();
	m_render_texture.display();
	m_sprite.setTexture(m_render_texture.getTexture());
	m_sprite.setPosition(m_location_offset_x, m_location_offset_y);
}


void Map::cover_map_with_texture(const int texture_columm)
{
	int row{ 0 };
	int col{ 0 };
	sf::Sprite sprite;
	sprite.setTexture(m_map_texture);
	for (int i{ 0 }; i < m_map_grid_width * m_map_grid_height; ++i)
	{
		if (col == m_map_grid_width)
		{
			col = 0;
			++row;
		}
		sprite.setTextureRect(sf::IntRect(
			TEXTURE_WIDTH * texture_columm,
			TEXTURE_WIDTH * uniform_distribution_int(0, 3),
			TEXTURE_WIDTH,
			TEXTURE_WIDTH
		));
		sprite.setPosition(col * TEXTURE_WIDTH_f, row * TEXTURE_WIDTH_f);
		m_render_texture.draw(sprite);
		++col;
	}
}


void Map::build_road_grid()
{
	/*
	* This function should build a grid of small integers, 8 bit, which will delineate the road grid.
	* Each anchor point is treated as some kind of building, the goal is to build a road from each 
	* anchor point's (x,y) location all the way to the y_centre. Then one big left to right road from
	* the min x point to the max x point at the height of y_centre.
	*/

	m_road_grid.min_x_coord = static_cast<int>(
		(m_graph.get_min_entity_x_pos() - m_location_offset_x) / TEXTURE_WIDTH_f
	);
	m_road_grid.max_x_coord = static_cast<int>(
		(m_graph.get_max_entity_x_pos() - m_location_offset_x) / TEXTURE_WIDTH_f
	);
	m_road_grid.min_y_coord = static_cast<int>(
		(m_graph.get_min_entity_y_pos() - m_location_offset_y) / TEXTURE_WIDTH_f
	);
	m_road_grid.max_y_coord = static_cast<int>(
		(m_graph.get_max_entity_y_pos() - m_location_offset_y) / TEXTURE_WIDTH_f
	);
	m_road_grid.width = m_road_grid.max_x_coord - m_road_grid.min_x_coord + 3;
	m_road_grid.height = m_road_grid.max_y_coord - m_road_grid.min_y_coord + 3;
	m_road_grid.mid_y_coord = static_cast<int>((m_road_grid.height * 0.5) + m_road_grid.min_y_coord);
	m_road_grid.grid.resize(m_road_grid.width * m_road_grid.height);

	int y_diff;
	int increment;
	int anchor_x;
	int anchor_y;
	for (sf::Vector2f anchor_pt : m_graph.m_anchor_points)
	{
		anchor_x = static_cast<int>((anchor_pt.x - m_location_offset_x) / TEXTURE_WIDTH_f) - m_road_grid.min_x_coord + 1;
		anchor_y = static_cast<int>((anchor_pt.y - m_location_offset_y) / TEXTURE_WIDTH_f) - m_road_grid.min_y_coord + 1;
		m_structures.push_back(Structure(
			static_cast<float>(anchor_x), static_cast<float>(anchor_y), Structure::Index::home
		));
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


void Map::map_textures_to_road_grid(const int road_texture_columm, const int ground_texture_columm)
{
	int convolution_window{};
	for (int row{ 0 }; row < (m_road_grid.height - 1); ++row)
	{
		for (int col{ 0 }; col < (m_road_grid.width - 1); ++col)
		{
			convolution_window = 0;
			convolution_window += m_road_grid.grid[row * m_road_grid.width + col] * 1000; // top left
			convolution_window += m_road_grid.grid[row * m_road_grid.width + col + 1] * 100; // top right
			convolution_window += m_road_grid.grid[(row + 1) * m_road_grid.width + col] * 10; // bot left
			convolution_window += m_road_grid.grid[(row + 1) * m_road_grid.width + col + 1]; // bot right
			switch (convolution_window)
			{
			case 1111:
				all_one_texture(road_texture_columm, col, row);
				break;

			case 1010:
				blend_vertically(road_texture_columm, ground_texture_columm, col, row);
				break;

			case 101:
				blend_vertically(ground_texture_columm, road_texture_columm, col, row);
				break;

			case 1100:
				blend_horizontally(road_texture_columm, ground_texture_columm, col, row);
				break;

			case 11:
				blend_horizontally(ground_texture_columm, road_texture_columm, col, row);
				break;

			case 1110:
				blend_uli(road_texture_columm, ground_texture_columm, col, row);
				break;

			case 1011:
				blend_dli(road_texture_columm, ground_texture_columm, col, row);
				break;

			case 1101:
				blend_uri(ground_texture_columm, road_texture_columm, col, row);
				break;

			case 111:
				blend_dri(ground_texture_columm, road_texture_columm, col, row);
				break;

			case 1000:
				blend_ulo(road_texture_columm, ground_texture_columm, col, row);
				break;

			case 10:
				blend_dlo(road_texture_columm, ground_texture_columm, col, row);
				break;

			case 100:
				blend_uro(ground_texture_columm, road_texture_columm, col, row);
				break;

			case 1:
				blend_dro(ground_texture_columm, road_texture_columm, col, row);
				break;
			}
		}
	}
}


void Map::map_textures_to_anchor_points()
{
	int anchor_x;
	int anchor_y;
	sf::Sprite sprite;
	sprite.setTexture(m_anchor_texture);

	for (sf::Vector2f anchor_pt : m_graph.m_anchor_points)
	{
		anchor_x = static_cast<int>((anchor_pt.x - m_location_offset_x) / TEXTURE_WIDTH_f);
		anchor_y = static_cast<int>((anchor_pt.y - m_location_offset_y) / TEXTURE_WIDTH_f);
		if (anchor_y > m_road_grid.mid_y_coord)
		{
			sprite.setTextureRect(sf::IntRect(
				TEXTURE_WIDTH * 0,
				TEXTURE_WIDTH * 4,
				TEXTURE_WIDTH * 4,
				TEXTURE_WIDTH * 4
			));
		}
		else
		{
			sprite.setTextureRect(sf::IntRect(
				TEXTURE_WIDTH * 0,
				TEXTURE_WIDTH * 0,
				TEXTURE_WIDTH * 4,
				TEXTURE_WIDTH * 4
			));
		}
		sprite.setPosition(
			(anchor_x * TEXTURE_WIDTH_f) - TEXTURE_WIDTH_f, 
			(anchor_y * TEXTURE_WIDTH_f) - TEXTURE_WIDTH_f
		);
		m_render_texture.draw(sprite);
	}
}


void Map::blend_horizontally(
	const int up_texture_col, const int down_texture_col, const int pos_x, const int pos_y
)
{
	m_blended_texture.create(TEXTURE_WIDTH, TEXTURE_WIDTH);
	m_blended_texture.clear(sf::Color::Green);
	int row_ref{ uniform_distribution_int(0, 3) };

	for (int row{ 0 }; row < m_panels_per_row; ++row)
	{
		for (int col{ 0 }; col < m_panels_per_row; ++col)
		{
			sf::Sprite sprite;
			sprite.setTexture(m_map_texture);
			if (uniform_distribution_float(0 + m_neatness_scaler, 1 - m_neatness_scaler) <
				(row / m_probability_denominator)) // as row gets higher this becomes more likely
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * down_texture_col + col * m_panel_size,
					TEXTURE_WIDTH * row_ref + row * m_panel_size,
					m_panel_size,
					m_panel_size
				));
			}
			else
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * up_texture_col + col * m_panel_size,
					TEXTURE_WIDTH * row_ref + row * m_panel_size,
					m_panel_size,
					m_panel_size
				));
			}
			sprite.setPosition(
				col * static_cast<float>(m_panel_size), row * static_cast<float>(m_panel_size)
			);
			m_blended_texture.draw(sprite);
		}
	}
	m_blended_texture.display();
	m_blended_sprite.setTexture(m_blended_texture.getTexture());
	m_blended_sprite.setPosition(
		(pos_x + m_road_grid.min_x_coord) * TEXTURE_WIDTH_f,
		(pos_y + m_road_grid.min_y_coord) * TEXTURE_WIDTH_f
	);
	m_render_texture.draw(m_blended_sprite);
}


void Map::blend_vertically(
	const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y
)
{
	m_blended_texture.create(TEXTURE_WIDTH, TEXTURE_WIDTH);
	m_blended_texture.clear(sf::Color::Green);
	int row_ref{ uniform_distribution_int(0, 3) };

	for (int row{ 0 }; row < m_panels_per_row; ++row)
	{
		for (int col{ 0 }; col < m_panels_per_row; ++col)
		{
			sf::Sprite sprite;
			sprite.setTexture(m_map_texture);
			if (uniform_distribution_float(0 + m_neatness_scaler, 1 - m_neatness_scaler) <
				(col / m_probability_denominator)) // as col gets higher this becomes more likely
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * right_texture_col + col * m_panel_size,
					TEXTURE_WIDTH * row_ref + row * m_panel_size,
					m_panel_size,
					m_panel_size
				));
			}
			else
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * left_texture_col + col * m_panel_size,
					TEXTURE_WIDTH * row_ref + row * m_panel_size,
					m_panel_size,
					m_panel_size
				));
			}
			sprite.setPosition(
				col * static_cast<float>(m_panel_size), row * static_cast<float>(m_panel_size)
			);
			m_blended_texture.draw(sprite);
		}
	}
	m_blended_texture.display();
	m_blended_sprite.setTexture(m_blended_texture.getTexture());
	m_blended_sprite.setPosition(
		(pos_x + m_road_grid.min_x_coord) * TEXTURE_WIDTH_f, 
		(pos_y + m_road_grid.min_y_coord) * TEXTURE_WIDTH_f
	);
	m_render_texture.draw(m_blended_sprite);
}


void Map::blend_uli(
	const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y
)
{
	m_blended_texture.create(TEXTURE_WIDTH, TEXTURE_WIDTH);
	m_blended_texture.clear(sf::Color::Green);
	int row_ref{ uniform_distribution_int(0, 3) };
	for (int row{ 0 }; row < m_panels_per_row; ++row)
	{
		for (int col{ 0 }; col < m_panels_per_row; ++col)
		{
			sf::Sprite sprite;
			sprite.setTexture(m_map_texture);
			if (col + row + uniform_distribution_float(-5.f, 5.f) - 9 > 16)
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * right_texture_col + col * m_panel_size,
					TEXTURE_WIDTH * row_ref + row * m_panel_size,
					m_panel_size,
					m_panel_size
				));
			}
			else
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * left_texture_col + col * m_panel_size,
					TEXTURE_WIDTH * row_ref + row * m_panel_size,
					m_panel_size,
					m_panel_size
				));
			}
			sprite.setPosition(
				col * static_cast<float>(m_panel_size), row * static_cast<float>(m_panel_size)
			);
			m_blended_texture.draw(sprite);
		}
	}
	m_blended_texture.display();
	m_blended_sprite.setTexture(m_blended_texture.getTexture());
	m_blended_sprite.setPosition(
		(pos_x + m_road_grid.min_x_coord) * TEXTURE_WIDTH_f,
		(pos_y + m_road_grid.min_y_coord) * TEXTURE_WIDTH_f
	);
	m_render_texture.draw(m_blended_sprite);
}


void Map::blend_dli(
	const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y
)
{
	m_blended_texture.create(TEXTURE_WIDTH, TEXTURE_WIDTH);
	m_blended_texture.clear(sf::Color::Green);
	int row_ref{ uniform_distribution_int(0, 3) };
	for (int row{ 0 }; row < m_panels_per_row; ++row)
	{
		for (int col{ 0 }; col < m_panels_per_row; ++col)
		{
			sf::Sprite sprite;
			sprite.setTexture(m_map_texture);
			if (row - col + uniform_distribution_float(-5.f, 5.f) + 9 > 0)
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * left_texture_col + col * m_panel_size,
					TEXTURE_WIDTH * row_ref + row * m_panel_size,
					m_panel_size,
					m_panel_size
				));
			}
			else
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * right_texture_col + col * m_panel_size,
					TEXTURE_WIDTH * row_ref + row * m_panel_size,
					m_panel_size,
					m_panel_size
				));
			}
			sprite.setPosition(
				col * static_cast<float>(m_panel_size), row * static_cast<float>(m_panel_size)
			);
			m_blended_texture.draw(sprite);
		}
	}
	m_blended_texture.display();
	m_blended_sprite.setTexture(m_blended_texture.getTexture());
	m_blended_sprite.setPosition(
		(pos_x + m_road_grid.min_x_coord) * TEXTURE_WIDTH_f,
		(pos_y + m_road_grid.min_y_coord) * TEXTURE_WIDTH_f
	);
	m_render_texture.draw(m_blended_sprite);
}


void Map::blend_uri(
	const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y
)
{
	m_blended_texture.create(TEXTURE_WIDTH, TEXTURE_WIDTH);
	m_blended_texture.clear(sf::Color::Green);
	int row_ref{ uniform_distribution_int(0, 3) };
	for (int row{ 0 }; row < m_panels_per_row; ++row)
	{
		for (int col{ 0 }; col < m_panels_per_row; ++col)
		{
			sf::Sprite sprite;
			sprite.setTexture(m_map_texture);
			if (row - col + uniform_distribution_float(-5.f, 5.f) - 9 > 0)
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * left_texture_col + col * m_panel_size,
					TEXTURE_WIDTH * row_ref + row * m_panel_size,
					m_panel_size,
					m_panel_size
				));
			}
			else
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * right_texture_col + col * m_panel_size,
					TEXTURE_WIDTH * row_ref + row * m_panel_size,
					m_panel_size,
					m_panel_size
				));
			}
			sprite.setPosition(
				col * static_cast<float>(m_panel_size), row * static_cast<float>(m_panel_size)
			);
			m_blended_texture.draw(sprite);
		}
	}
	m_blended_texture.display();
	m_blended_sprite.setTexture(m_blended_texture.getTexture());
	m_blended_sprite.setPosition(
		(pos_x + m_road_grid.min_x_coord) * TEXTURE_WIDTH_f,
		(pos_y + m_road_grid.min_y_coord) * TEXTURE_WIDTH_f
	);
	m_render_texture.draw(m_blended_sprite);
}


void Map::blend_dri(
	const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y
)
{
	m_blended_texture.create(TEXTURE_WIDTH, TEXTURE_WIDTH);
	m_blended_texture.clear(sf::Color::Green);
	int row_ref{ uniform_distribution_int(0, 3) };
	for (int row{ 0 }; row < m_panels_per_row; ++row)
	{
		for (int col{ 0 }; col < m_panels_per_row; ++col)
		{
			sf::Sprite sprite;
			sprite.setTexture(m_map_texture);
			if (col + row + uniform_distribution_float(-5.f, 5.f) + 9 > 16)
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * right_texture_col + col * m_panel_size,
					TEXTURE_WIDTH * row_ref + row * m_panel_size,
					m_panel_size,
					m_panel_size
				));
			}
			else
			{
				sprite.setTextureRect(sf::IntRect(
					TEXTURE_WIDTH * left_texture_col + col * m_panel_size,
					TEXTURE_WIDTH * row_ref + row * m_panel_size,
					m_panel_size,
					m_panel_size
				));
			}
			sprite.setPosition(
				col * static_cast<float>(m_panel_size), row * static_cast<float>(m_panel_size)
			);
			m_blended_texture.draw(sprite);
		}
	}
	m_blended_texture.display();
	m_blended_sprite.setTexture(m_blended_texture.getTexture());
	m_blended_sprite.setPosition(
		(pos_x + m_road_grid.min_x_coord) * TEXTURE_WIDTH_f,
		(pos_y + m_road_grid.min_y_coord) * TEXTURE_WIDTH_f
	);
	m_render_texture.draw(m_blended_sprite);
}


void Map::blend_ulo(
	const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y
)
{
	blend_dri(left_texture_col, right_texture_col, pos_x, pos_y);
}


void Map::blend_dlo(
	const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y
)
{
	blend_uri(left_texture_col, right_texture_col, pos_x, pos_y);
}


void Map::blend_uro(
	const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y
)
{
	blend_dli(left_texture_col, right_texture_col, pos_x, pos_y);
}


void Map::blend_dro(
	const int left_texture_col, const int right_texture_col, const int pos_x, const int pos_y
)
{
	blend_uli(left_texture_col, right_texture_col, pos_x, pos_y);
}


void Map::all_one_texture(const int texture_col, const int pos_x, const int pos_y)
{
	sf::Sprite sprite;
	sprite.setTexture(m_map_texture);
	sprite.setTextureRect(sf::IntRect(
		TEXTURE_WIDTH * texture_col,
		TEXTURE_WIDTH * uniform_distribution_int(0, 3),
		TEXTURE_WIDTH,
		TEXTURE_WIDTH
	));
	sprite.setPosition(
		(pos_x + m_road_grid.min_x_coord) * TEXTURE_WIDTH_f, 
		(pos_y + m_road_grid.min_y_coord) * TEXTURE_WIDTH_f
	);
	m_render_texture.draw(sprite);
}


void Map::draw(sf::RenderWindow& window, const float x_move_distance, const float y_move_distance)
{
	m_sprite.move(x_move_distance, y_move_distance);
	window.draw(m_sprite);
}
