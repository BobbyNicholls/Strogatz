#include "Distributions.h"
#include "Map.h"

constexpr int MAP_GRID_WIDTH{ 200 };
constexpr int MAP_GRID_HEIGHT{ 140 };
constexpr int TEXTURE_WIDTH{ 64 };
constexpr float TEXTURE_WIDTH_f{ 64.f };


enum Texture
{
	grass,
	sand,
	stone,
	mud,
};


Map::Map(sf::Texture& map_texture)
	: m_map_texture{ map_texture }
{
	fill_map();
}


void Map::fill_map()
{
	int row{ 0 };
	int col{ 0 };
	m_render_texture.create(MAP_GRID_WIDTH * TEXTURE_WIDTH, MAP_GRID_HEIGHT * TEXTURE_WIDTH);
	m_render_texture.clear(sf::Color::Green);

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
	build_road(95, 5, 110, 70);
	build_road(105, 5, 120, 80);
	build_road(125, 5, 145, 90);
	m_render_texture.display();
	m_sprite.setTexture(m_render_texture.getTexture());
	m_sprite.setPosition(
		-MAP_GRID_WIDTH * TEXTURE_WIDTH_f * 0.5f, -MAP_GRID_HEIGHT * TEXTURE_WIDTH_f * 0.5f
	);
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
	const int neatness_scaler{ 8 };
	int row_ref{ uniform_distribution_int(0, 3) };
	float probability_denominator{ (static_cast<float>(panels_per_row) - neatness_scaler) };
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
			if (uniform_distribution_float(0, 1) < ((*col_or_row_ptr) / probability_denominator))
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


void Map::build_road(
	const int origin_x, const int origin_y, const int destination_x, const int destination_y
)
{
	const int x_diff{ destination_x - origin_x };
	const int y_diff{ destination_y - origin_y };
	// travel the y-axis first
	int increment{ (y_diff > 0) ? 1 : -1 };
	if (y_diff != 0)
	{
		int i{ increment };
		while (i != (y_diff-increment))
		{
			blend_textures(Texture::grass, Texture::stone, true);
			m_blended_sprite.setPosition(
				(static_cast<float>(origin_x) - 1) * TEXTURE_WIDTH,
				(static_cast<float>(origin_y) + i) * TEXTURE_WIDTH
			);
			m_render_texture.draw(m_blended_sprite);
			blend_textures(Texture::stone, Texture::grass, true);
			m_blended_sprite.setPosition(
				(static_cast<float>(origin_x)) * TEXTURE_WIDTH,
				(static_cast<float>(origin_y) + i) * TEXTURE_WIDTH
			);
			m_render_texture.draw(m_blended_sprite);
			i += increment;
		}
	}
	if (x_diff != 0)
	{
		increment = (x_diff > 0) ? 1 : -1;
		int j{ increment };
		while (j != (x_diff-increment))
		{
			blend_textures(Texture::grass, Texture::stone, false);
			m_blended_sprite.setPosition(
				(static_cast<float>(origin_x) + j) * TEXTURE_WIDTH,
				(static_cast<float>(destination_y) - 1) * TEXTURE_WIDTH
			);
			m_render_texture.draw(m_blended_sprite);
			blend_textures(Texture::stone, Texture::grass, false);
			m_blended_sprite.setPosition(
				(static_cast<float>(origin_x) + j) * TEXTURE_WIDTH,
				(static_cast<float>(destination_y)) * TEXTURE_WIDTH
			);
			m_render_texture.draw(m_blended_sprite);
			j += increment;
		}
	}
}
