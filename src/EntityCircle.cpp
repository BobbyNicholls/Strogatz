#include "Distributions.h"
#include "EntityCircle.h"


EntityCircle::EntityCircle(
	id_t id,
	time_period_t birth_time,
	float radius,
	float outline_thickness, 
	sf::Color outline_colour
)
	: Entity{ id, birth_time },
	m_radius{ radius },
	m_outline_thickness{ outline_thickness }, 
	m_outline_colour{ outline_colour }
{
	m_shape.setRadius(m_radius);
	update_colour();
	m_shape.setOutlineThickness(m_outline_thickness);
	m_shape.setOutlineColor(m_outline_colour);

}


void EntityCircle::update_colour()
{

	float beliefs_sum{
		m_beliefs[0][0] + m_beliefs[0][1] + m_beliefs[1][0]
	};

	m_shape.setFillColor(
		sf::Color(
			static_cast<sf::Uint8>((m_beliefs[0][0] / beliefs_sum) * 255),
			static_cast<sf::Uint8>((m_beliefs[0][1] / beliefs_sum) * 255),
			static_cast<sf::Uint8>((m_beliefs[1][0] / beliefs_sum) * 255)
			//static_cast<sf::Uint8>((m_beliefs[1][1]/beliefs_sum)*255)
		)
	);

}


void EntityCircle::set_position_randomly()
{
	m_shape.setPosition(
		uniform_distribution_float(10, 790),
		uniform_distribution_float(10, 490)
	);
}


void EntityCircle::set_position_relative_to_links(int offset)
{
	/*
	Makes the position of the node the average position of all nodes connected to
	this node plus some randomised offset.
	*/
	float x_sum{ 0 };
	float y_sum{ 0 };
	for (auto* link : m_links)
	{
		EntityCircle* link_ec_t{ static_cast<EntityCircle*>(link) };
		const sf::Vector2f& link_pos{ link_ec_t->get_shape().getPosition() };
		x_sum += link_pos.x;
		y_sum += link_pos.y;
	}

	m_shape.setPosition(
		(x_sum / m_links.size()) + uniform_distribution_float(-offset, offset),
		(y_sum / m_links.size()) + uniform_distribution_float(-offset, offset)
	);
}
