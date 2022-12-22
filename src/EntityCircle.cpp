#include "Distributions.h"
#include "EntityCircle.h"


extern const int edge_buffer;
extern const int game_height;
extern const int game_width;


EntityCircle* get_entity_circle(const time_period_t time_period, const Race* race)
{
	EntityCircle* entity_pointer{ new (std::nothrow) EntityCircle(time_period, race) };

	if (!entity_pointer) // handle case where new returned null
	{
		// TODO: error handling here
		std::cerr << "\n\nCould not allocate memory!!!\n\n";
		throw - 1;
	}
	return entity_pointer;
}


EntityCircle::EntityCircle(
	time_period_t birth_time,
	const Race* race,
	float radius,
	float outline_thickness, 
	sf::Color outline_colour
)
	: Entity{ birth_time, race },
	m_radius{ radius },
	m_outline_thickness{ outline_thickness }, 
	m_outline_colour{ outline_colour }
{
	m_shape.setRadius(m_radius);
	m_shape.setPointCount(3 + race->get_index());
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


EntityCircle* EntityCircle::set_position_randomly(
	const int min_x, 
	const int max_x,
	const int min_y,
	const int max_y
)
{
	m_shape.setPosition(
		uniform_distribution_float(min_x, max_x),
		uniform_distribution_float(min_y, max_y)
	);
	return this;
}


EntityCircle* EntityCircle::move_to_links(const int offset)
{
	/*
	Makes the position of the node the average position of all nodes connected to
	this node plus some randomised offset.
	*/
	if (!is_pathing() && (m_links.size() > 0))
	{
		float x_sum{ 0 };
		float y_sum{ 0 };
		for (auto* link : m_links)
		{
			EntityCircle* link_ec_t{ static_cast<EntityCircle*>(link) };
			const sf::Vector2f& link_pos{ link_ec_t->get_shape().getPosition() };
			x_sum += link_pos.x;
			y_sum += link_pos.y;
		}
		move_to_destination(
			(x_sum / m_links.size()) + uniform_distribution_float(0, offset), 
			(y_sum / m_links.size()) + uniform_distribution_float(0, offset)
		);
	}
	return this;
}


void EntityCircle::set_position_relative_to_links(
	const int min_x,
	const int max_x,
	const int min_y,
	const int max_y,
	const int offset
)
{
	/*
	Makes the position of the node the average position of all nodes connected to
	this node plus some randomised offset.
	*/
	float x_sum{ 0 };
	float y_sum{ 0 };
	if (m_links.size() > 0)
	{
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
	else
	{
		set_position_randomly(min_x, max_x, min_y, max_y);
	}
}


void EntityCircle::move_to_destination(
	const float destination_x, 
	const float destination_y, 
	const float speed
)
{
	const sf::Vector2f& pos{ m_shape.getPosition() };
	float x_diff{ destination_x - pos.x };
	float y_diff{ destination_y - pos.y };
	int steps{ static_cast<int>(sqrt((x_diff * x_diff) + (y_diff * y_diff)) / speed) };
	if (steps)
	{
		m_pathing.steps = steps;
		m_pathing.x_move = x_diff / steps;
		m_pathing.y_move = y_diff / steps;
	}
}


void EntityCircle::move_to_entity(const EntityCircle* entity, const int offset)
{
	const sf::Vector2f& pos{ entity->m_shape.getPosition() };
	move_to_destination(
		pos.x + uniform_distribution_float(0, offset),
		pos.y + uniform_distribution_float(0, offset)
	);
}


void EntityCircle::move_along_path()
{
	m_shape.move(m_pathing.x_move, m_pathing.y_move);
	--m_pathing.steps;
}
