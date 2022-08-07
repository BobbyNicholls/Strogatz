#include <iostream>

#include "Entity.h"


Entity::Entity()
	:m_id{ generate_id() }
{}


void Entity::propagate_beliefs()
{
	std::cout << "Wololo!\n";
}


void Entity::add_link(int id)
{
	m_links.push_back(id);
}


void Entity::print_links()
{
    std::cout << "Entity " << get_id() << " is linked to: ";
    for (int i{ 0 }; i < m_links.size(); ++i)
    {
        std::cout << m_links[i] << ", ";
    }
    std::cout << '\n';
}
