#ifndef STROG_UTILS_H
#define STROG_UTILS_H

#include <iostream>
#include <SFML/Graphics.hpp>

#include "Distributions.h"

extern const int edge_buffer;
extern const int game_height;
extern const int game_width;

const int entity_limit{ 5 };
const int link_limit{ 1000 };

using id_t = std::uint_fast32_t;


id_t generate_id();


template <typename e_t>
void link_entities(
    e_t* entity_from,
    e_t* entity_to,
    id_t links[link_limit][2],
    unsigned int& counter
)
{
    id_t from_id{ entity_from->get_id() };
    id_t to_id{ entity_to->get_id() };
    entity_from->add_link(entity_to);
    entity_to->add_link(entity_from);
    links[counter][0] = from_id;
    links[counter++][1] = to_id;
}


template <typename Entity_t>
unsigned int add_semi_random_links(
    const std::vector<Entity_t*>& entities,
    Entity_t* entity,
    id_t links[link_limit][2],
    unsigned int& counter
)
{
    /*
    For now, for `entity` node, just add a link to a node chosen at random weighted by number
    of existing edges.
    */
    unsigned int link_iloc{ 
        static_cast<unsigned int>(uniform_distribution_int(1, counter*2)) 
    };
    unsigned int entity_iloc;
    if (link_iloc > counter)
    {
        link_iloc -= (counter + 1);
        entity_iloc = links[link_iloc][1];
    }
    else
    {
        link_iloc -= 1;
        entity_iloc = links[link_iloc][0];
    }

    link_entities(entities[entity_iloc], entity, links, counter);
    return entity_iloc;
}


template <typename Shape_t>
void keyboard_move_entity(
    Shape_t& shape,
    const float move_speed,
    const float time_counter
)
{
    sf::Vector2f pos{ shape.getPosition() };
    float move_distance{ move_speed * time_counter };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        shape.move((pos.x < edge_buffer) ? 
            move_distance : -move_distance, 0.f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        shape.move((pos.x > (game_width - edge_buffer)) ? 
            -move_distance : move_distance, 0.f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        shape.move(0.f, (pos.y < edge_buffer) ? 
            move_distance : -move_distance);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        shape.move(0.f, (pos.y > (game_height - edge_buffer)) ? 
            -move_distance : move_distance);
    }
}


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
    float attraction_percent = 0.001f
)
{
    float x_sum{};
    float y_sum{};
    sf::Vector2f pos{ entity->get_shape().getPosition() };
    auto links{ entity->get_links() };

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
