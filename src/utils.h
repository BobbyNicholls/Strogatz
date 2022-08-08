#ifndef STROG_UTILS_H
#define STROG_UTILS_H

#include <SFML/Graphics.hpp>

extern const int game_width;
extern const int game_height;
extern const int edge_buffer;

// should use inline variables to save memory if multiple inclusions
const int link_limit{ 1000 };

using id_t = std::uint_fast32_t;

id_t generate_id();


template <typename Entity>
void link_entities(
    Entity* entity_from,
    Entity* entity_to,
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


#endif
