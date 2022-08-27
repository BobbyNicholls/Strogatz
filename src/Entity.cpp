#include <iostream>

#include "Entity.h"


Entity::Entity(id_t id, time_period_t birth_time)
	:m_id{ id }, m_birth_time{ birth_time }
{
    m_links.reserve(5);
    m_beliefs[0][0] = mild_aversion();
    m_beliefs[0][1] = mild_aversion();
    m_beliefs[1][0] = mild_aversion();
    m_beliefs[1][1] = mild_aversion();
    m_sex = uniform_distribution_int(0, 1);
}


void Entity::add_child(Entity* entity)
{
    m_children.push_back(entity); // m_children does nothing and can contain danling ptr, this is a wasteful operation
}


void Entity::add_link(Entity* entity)
{
	m_links.push_back(entity);
}


void Entity::remove_link(Entity* entity_to_remove, bool double_removal)
{
    if (m_links.size() > 0)
    {
        for (std::vector<Entity*>::iterator it = m_links.begin(); it != m_links.end(); ++it)
        {
            if (*it == entity_to_remove) 
            {
                m_links.erase(it);
                break;
            }
        }
    }
    if (m_partner == entity_to_remove) m_partner = nullptr;
    if(double_removal) entity_to_remove->remove_link(this, false);
}


void Entity::add_parents(Entity* entity1, Entity* entity2)
{
    m_parents[0] = entity1;  // m_parents does nothing and can contain danling ptr, this is a wasteful operation
    m_parents[1] = entity2;
}


void Entity::add_partner(Entity* entity)
{
    m_paired = true;
    m_partner = entity;
    entity->m_paired = true;
    entity->m_partner = this;
}

bool Entity::is_linked_to(Entity* entity) const
{
    for (Entity* link : m_links)
    {
        if (entity == link)
        {
            return true;
        }
    }
    return false;
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


void Entity::update_beliefs(Entity* influencer)
{
    /*
    Use the sum product algorithm to update the belief of the node given
    an `influencer` node, which contains the beleifs that will inflience
    the node.
    */
    float result[2][2];
    for (int i{ 0 }; i < 2; ++i) // row number
    {
        for (int j{ 0 }; j < 2; ++j) // column number
        {
            // traverse row of influencer, column of influencee
            result[i][j] = (
                influencer->m_beliefs[i][0] * m_beliefs[0][j] +
                influencer->m_beliefs[i][1] * m_beliefs[1][j]
                );
        }
    }

    // normalise the beliefs to 0-1
    float result_sum{
        result[0][0] + result[0][1] + result[1][0] + result[1][1]
    };
    m_beliefs[0][0] = result[0][0] / result_sum;
    m_beliefs[0][1] = result[0][1] / result_sum;
    m_beliefs[1][0] = result[1][0] / result_sum;
    m_beliefs[1][1] = result[1][1] / result_sum;
}
