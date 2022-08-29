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


template <typename e_t>
void slingshot_move_entity(
    e_t* entity,
    const float attraction_percent = 0.001f
)
{
    float x_sum{};
    float y_sum{};
    const sf::Vector2f& pos{ entity->get_shape().getPosition() };
    const auto& links{ entity->get_links() };

    for (const auto& link : links)
    {
        e_t* link_e_t{ static_cast<e_t*>(link) };
        const sf::Vector2f& link_pos{ link_e_t->get_shape().getPosition() };
        x_sum += link_pos.x;
        y_sum += link_pos.y;
    }
    entity->get_shape().move(
        ((x_sum / links.size()) - pos.x) * attraction_percent,
        ((y_sum / links.size()) - pos.y) * attraction_percent
    );
}


#endif
