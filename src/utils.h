#ifndef STROG_UTILS_H
#define STROG_UTILS_H

#include <SFML/Graphics.hpp>

extern const int game_width;
extern const int game_height;
extern const int edge_buffer;

const int link_limit{ 1000 };


int generate_id();


template <typename Entity>
void link_entities(
    Entity* entity_from,
    Entity* entity_to,
    int links[link_limit][2],
    unsigned int& counter
)
{
    int to_id{ entity_to->get_id() };
    int from_id{ entity_from->get_id() };
    entity_from->add_link(to_id);
    entity_to->add_link(from_id);
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
        entity.move((pos.x < edge_buffer) ? move_distance : -move_distance, 0.f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        entity.move((pos.x > (game_width - edge_buffer)) ? -move_distance : move_distance, 0.f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        entity.move(0.f, (pos.y < edge_buffer) ? move_distance : -move_distance);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        entity.move(0.f, (pos.y > (game_height - edge_buffer)) ? -move_distance : move_distance);
    }
}


#endif
