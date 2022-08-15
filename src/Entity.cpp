#include <iostream>

#include "Entity.h"


Entity::Entity()
	:m_id{ generate_id() }
{
    m_beliefs[0][0] = mild_affinity();
    m_beliefs[0][1] = mild_aversion();
    m_beliefs[1][0] = mild_aversion();
    m_beliefs[1][1] = mild_aversion();
}


void Entity::propagate_beliefs()
{
	std::cout << "Wololo!\n";
}


void Entity::add_link(Entity* entity)
{
	m_links.push_back(entity);
}


void Entity::print_links() const
{
    std::cout << "Entity " << get_id() << " is linked to: ";
    for (const auto& link: m_links)
    {
        std::cout << link->get_id() << ", ";
    }
    std::cout << '\n';
}


void Entity::print_beliefs() const
{
    std::cout << "Entity " << get_id() << " believes: " << '\n';
    std::cout << "0, 0: " << m_beliefs[0][0] << '\n';
    std::cout << "0, 1: " << m_beliefs[0][1] << '\n';
    std::cout << "1, 0: " << m_beliefs[1][0] << '\n';
    std::cout << "1, 1: " << m_beliefs[1][1] << "\n\n";
}
