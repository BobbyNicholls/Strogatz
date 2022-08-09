#include "Distributions.h"
#include "EntityCircle.h"

EntityCircle::EntityCircle(
	float radius,
	float outline_thickness, 
	sf::Color fill_colour,
	sf::Color outline_colour
)
	: m_radius{ radius },
	m_outline_thickness{ outline_thickness }, 
	m_fill_colour{ fill_colour }, 
	m_outline_colour{ outline_colour }
{
	m_shape.setRadius(m_radius);
	m_shape.setFillColor(m_fill_colour);
	m_shape.setOutlineThickness(m_outline_thickness);
	m_shape.setOutlineColor(m_outline_colour);
	m_shape.setPosition(
		static_cast<float>(uniform_distribution(10, 790)), 
		static_cast<float>(uniform_distribution(10, 490))
	);
}
