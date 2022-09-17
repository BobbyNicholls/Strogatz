#include "Distributions.h"
#include "Map.h"

const int MAP_GRID_WIDTH{ 200 };
const int MAP_GRID_HEIGHT{ 140 };

Map::Map(sf::Texture& map_texture)
	: m_map_texture{ map_texture }
{
	fill_map();
}


void Map::fill_map()
{
	int row{ 0 };
	int col{ 0 };
	m_render_texture.create(MAP_GRID_WIDTH * 64, MAP_GRID_HEIGHT * 64);
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
		if ( ((col > (MAP_GRID_WIDTH * 0.5f + 3)) && (col <  (MAP_GRID_WIDTH * 0.5f + 9))) ||
			 ((row > (MAP_GRID_HEIGHT * 0.5f + 3)) && (row < (MAP_GRID_HEIGHT * 0.5f + 9))) )
			sprite.setTextureRect(sf::IntRect(64 * 5, 64 * uniform_distribution_int(0, 3), 64, 64));
		else if (col == (MAP_GRID_WIDTH * 0.5f + 3))
		{
			blend_textures(5, 4);
			sprite = m_blended_sprite;
		}
		else if (col == (MAP_GRID_WIDTH * 0.5f + 9))
		{
			blend_textures(4, 5);
			sprite = m_blended_sprite;
		}
		else
		{
			sprite.setTextureRect(sf::IntRect(64 * 4, 64 * uniform_distribution_int(4, 7), 64, 64));
			sprite.setColor(sf::Color(0, 150, 0));
		}
		sprite.setPosition(col * 64.f, row * 64.f);
		m_render_texture.draw(sprite);
		++col;
	}
	m_render_texture.display();
	m_sprite.setTexture(m_render_texture.getTexture());
	m_sprite.setPosition(-MAP_GRID_WIDTH * 64.f * 0.5f, -MAP_GRID_HEIGHT * 64.f * 0.5f);
}


void Map::draw(sf::RenderWindow& window, const float x_move_distance, const float y_move_distance)
{
	m_sprite.move(x_move_distance, y_move_distance);
	window.draw(m_sprite);
}


void Map::blend_textures(const int left_col, const int right_col)
{
	m_blended_texture.create(64, 64);
	m_blended_texture.clear(sf::Color::Green);
	for (int row{ 0 }; row < 16; ++row)
	{
		for (int col{ 0 }; col < 16; ++col)
		{
			sf::Sprite sprite;
			sprite.setTexture(m_map_texture);
			if (uniform_distribution_float(0, 1) < (col / 8.f))
			{
				sprite.setTextureRect(
					sf::IntRect(64 * left_col + col * 4, 64 * uniform_distribution_int(0, 3) + row * 4, 4, 4)
				);
				if (left_col == 4) sprite.setColor(sf::Color(0, 150, 0));
			}
			else
			{
				sprite.setTextureRect(
					sf::IntRect(64 * right_col + col * 4, 64 * uniform_distribution_int(0, 3) + row * 4, 4, 4)
				);
				if (right_col == 4) sprite.setColor(sf::Color(0, 150, 0));
			}
			sprite.setPosition(col * 4.f, row * 4.f);
			m_blended_texture.draw(sprite);
		}
	}
	m_blended_texture.display();
	m_blended_sprite.setTexture(m_blended_texture.getTexture());
}
