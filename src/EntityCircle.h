/*
This is a derivative of the Entity object, designed to represent the entity as a circle shape.
The features in addition to what is included in Entity are:
    TODO
*/

#ifndef STROG_ENTITY_CIRCLE_H
#define STROG_ENTITY_CIRCLE_H

#include "Entity.h"

class EntityCircle : public Entity
{
private:
    float m_radius;
    // By default, the outline is extruded outwards from the shape (e.g. if you have a circle 
    // with a radius of 10 and an outline thickness of 5, the total radius of the circle will
    // be 15). You can make it extrude towards the center of the shape instead, by setting a 
    // negative thickness.
    float m_outline_thickness;
    sf::Color m_outline_colour;
    sf::CircleShape m_shape;

public:
    EntityCircle(
        id_t id,
        float radius = 10.f,
        float outline_thickness = 1.f,
        sf::Color outline_colour = sf::Color(250, 150, 100)
    );
    float get_radius() const { return m_radius; }
    std::vector<Entity*>& get_links() { return m_links; }
    sf::CircleShape& get_shape() { return m_shape; };
    void update_colour();
    void set_position_randomly();
    void set_position_relative_to_links(int offset = 20);
};

#endif
