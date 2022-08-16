#include "Distributions.h"
#include "EntityCircle.h"

EntityCircle::EntityCircle(
	id_t id,
	float radius,
	float outline_thickness, 
	sf::Color outline_colour
)
	: Entity{ id },
	m_radius{ radius },
	m_outline_thickness{ outline_thickness }, 
	m_outline_colour{ outline_colour }
{
	m_shape.setRadius(m_radius);
	float m_beliefs_sum{
		m_beliefs[0][0] + m_beliefs[0][1] + m_beliefs[1][0]
	};
	m_shape.setFillColor(
		sf::Color(
			static_cast<sf::Uint8>((m_beliefs[0][0]/m_beliefs_sum)*255), 
			static_cast<sf::Uint8>((m_beliefs[0][1]/m_beliefs_sum)*255),
			static_cast<sf::Uint8>((m_beliefs[1][0]/m_beliefs_sum)*255)
			//static_cast<sf::Uint8>((m_beliefs[1][1]/m_beliefs_sum)*255)
		)
	);
	m_shape.setOutlineThickness(m_outline_thickness);
	m_shape.setOutlineColor(m_outline_colour);
	m_shape.setPosition(
		uniform_distribution_float(10, 790), 
		uniform_distribution_float(10, 490)
	);
}
