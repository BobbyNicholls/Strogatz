/*
This is a derivative of the Entity object, designed to represent the entity as a circle shape.
The features in addition to what is included in Entity are:
    TODO
*/

#ifndef STROG_ENTITY_CIRCLE_H
#define STROG_ENTITY_CIRCLE_H

#include "Entity.h"

struct Pathing
{
    float x_move{};
    float y_move{};
    int steps{};
};


class EntityCircle : public Entity
{
private:
    float m_radius;
    float m_outline_thickness;
    sf::Color m_outline_colour;
    sf::CircleShape m_shape;
    // this is going to create an absurd number of repetitions
    std::vector<sf::Vector2f> m_move_commands;
    Pathing m_pathing{};

public:
    EntityCircle(
        time_period_t birth_time,
        float radius = 10.f,
        float outline_thickness = 1.f,
        sf::Color outline_colour = sf::Color(250, 150, 100)
    );
    float get_radius() const { return m_radius; }
    std::vector<Entity*>& get_links() { return m_links; }
    sf::CircleShape& get_shape() { return m_shape; };
    bool is_pathing() { return static_cast<bool>(m_pathing.steps); };

    void update_colour();
    EntityCircle* set_position_randomly();
    EntityCircle* move_to_links(const int offset = 10);
    void set_position_relative_to_links(const int offset = 15);
    void move_to_destination(const float destination_x, const float destination_y);
    void move_to_entity(const EntityCircle* entity, const int offset = 10);
    void move_along_path();
};

EntityCircle* get_entity_circle(const time_period_t time_period);

#endif
