#include <iostream>

#include "Entity.h"


Entity::Entity()
	:m_id{ generate_id() }
{}


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
