#include <iostream>

#include "Entity.h"

void Entity::propagate_beliefs()
{
	std::cout << "Wololo!\n";
}

Entity::Entity()
	:m_id{ generate_id() }
{}

