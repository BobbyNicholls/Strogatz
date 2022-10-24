#ifndef STROG_UTILS_H
#define STROG_UTILS_H

#include <iostream>
#include <SFML/Graphics.hpp>

#include "Distributions.h"

extern const int edge_buffer;
extern const int game_height;
extern const int game_width;

using id_t = std::uint_fast32_t;
using time_period_t = std::uint_fast16_t;


//bool check_for_double_linkage(Graph& graph);
//bool check_for_entity_position_bug(Graph& graph);
id_t generate_id();
sf::Vector2f get_movement(const float move_distance);
void load_texture(sf::Texture& texture);


template <typename Shape_t>
void random_move_entity(
    Shape_t& shape,
    float buffer_distance = 15.f,
    int move_range = 1
)
{
    const sf::Vector2f& pos{ shape.getPosition() };
    if (pos.x > (game_width - edge_buffer)) // make this a case switch somehow?
    {
        shape.move(-buffer_distance, 0.f);
    }
    else if (pos.x < edge_buffer)
    {
        shape.move(buffer_distance, 0.f);
    }
    else if (pos.y > (game_height - edge_buffer))
    {
        shape.move(0.f, -buffer_distance);
    }
    else if (pos.y < edge_buffer)
    {
        shape.move(0.f, buffer_distance);
    }
    else
    {
        float x_move{ uniform_distribution_float(-move_range, move_range) };
        float y_move{ uniform_distribution_float(-move_range, move_range) };
        shape.move(x_move, y_move);
    }
}

#endif
