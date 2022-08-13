#ifndef STROG_UTILS_H
#define STROG_UTILS_H

#include <iostream>
#include <SFML/Graphics.hpp>

#include "Distributions.h"

extern const int game_width;
extern const int game_height;
extern const int edge_buffer;

// should use inline variables to save memory if multiple inclusions
const int link_limit{ 1000 };
const int entity_limit{ 150 };

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


template <typename EntityList_t, typename Entity_t>
void add_semi_random_links(
    EntityList_t entities[entity_limit],
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
        static_cast<unsigned int>(uniform_distribution(1, counter*2)) 
    };
    if (link_iloc > counter)
    {
        link_iloc -= (counter + 1);
        link_entities(entities[links[link_iloc][1]], entity, links, counter);
    }
    else
    {
        link_iloc -= 1;
        link_entities(entities[links[link_iloc][0]], entity, links, counter);
    }
}


template <typename Entity>
void keyboard_move_entity(
    Entity& entity,
    const float move_speed,
    const float time_counter
)
{
    sf::Vector2f pos{ entity.getPosition() };
    float move_distance{ move_speed * time_counter };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        entity.move((pos.x < edge_buffer) ? 
            move_distance : -move_distance, 0.f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        entity.move((pos.x > (game_width - edge_buffer)) ? 
            -move_distance : move_distance, 0.f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        entity.move(0.f, (pos.y < edge_buffer) ? 
            move_distance : -move_distance);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        entity.move(0.f, (pos.y > (game_height - edge_buffer)) ? 
            -move_distance : move_distance);
    }
}


template <typename Entity>
void random_move_entity(
    Entity& entity
)
{
    sf::Vector2f pos{ entity.getPosition() };
    if (pos.x > (game_width - edge_buffer)) // make this a case switch somehow?
    {
        entity.move(-25.f, 0.f);
    }
    else if (pos.x < edge_buffer)
    {
        entity.move(25.f, 0.f);
    }
    else if (pos.y > (game_height - edge_buffer))
    {
        entity.move(0.f, -25.f);
    }
    else if (pos.y < edge_buffer)
    {
        entity.move(0.f, 25.f);
    }
    else
    {
        float x_move{ static_cast<float>(uniform_distribution(-3, 3)) };
        float y_move{ static_cast<float>(uniform_distribution(-3, 3)) };
        entity.move(x_move, y_move);
    }
}


template <typename e_t>
void slingshot_move_entity(
    e_t* entity,
    float attraction_percent = 0.008f
)
{
    float x_sum{};
    float y_sum{};

    sf::Vector2f pos{ entity->m_shape.getPosition() };
    auto links{ entity->get_links() };
    for (auto link : links)
    {
        e_t* link_e_t = static_cast<e_t*>(link);
        sf::Vector2f link_pos{ link_e_t->m_shape.getPosition() };
        x_sum += link_pos.x;
        y_sum += link_pos.y;
    }
    entity->m_shape.move(
        ((x_sum / links.size()) - pos.x) * attraction_percent,
        ((y_sum / links.size()) - pos.y) * attraction_percent
    );
}


#endif
