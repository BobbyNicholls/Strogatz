#include <iostream>
#include<set>

#include "Entity.h"


Entity::Entity(time_period_t birth_time, const Race* race)
    : m_id{ generate_id() }, 
    m_birth_time{ birth_time }, 
    m_race{ race }, 
    m_sex{ static_cast<bool>(uniform_distribution_int(0, 1)) }
{
    m_links.reserve(5);
    switch (race->get_index())
    {
    case Race::Index::elf:
        m_beliefs[0][0] = Beleif::strong_affinity(); // spiritual / matrialist
        m_beliefs[0][1] = Beleif::strong_aversion(); // violent / pacifist
        m_beliefs[1][0] = Beleif::mild_affinity(); // authoritarian / liberal
        m_beliefs[1][1] = Beleif::strong_affinity(); // conservative / progressive
        break;

    case Race::Index::human:
        m_beliefs[0][0] = Beleif::true_random(); // spiritual / matrialist
        m_beliefs[0][1] = Beleif::true_random(); // violent / pacifist
        m_beliefs[1][0] = Beleif::true_random(); // authoritarian / liberal
        m_beliefs[1][1] = Beleif::true_random(); // conservative / progressive
        break;

    case Race::Index::goblin:
        m_beliefs[0][0] = Beleif::strong_aversion(); // spiritual / matrialist
        m_beliefs[0][1] = Beleif::strong_affinity(); // violent / pacifist
        m_beliefs[1][0] = Beleif::mild_aversion(); // authoritarian / liberal
        m_beliefs[1][1] = Beleif::true_random(); // conservative / progressive
        break;

    case Race::Index::gnome:
        m_beliefs[0][0] = Beleif::strong_aversion(); // spiritual / matrialist
        m_beliefs[0][1] = Beleif::mild_aversion(); // violent / pacifist
        m_beliefs[1][0] = Beleif::true_random(); // authoritarian / liberal
        m_beliefs[1][1] = Beleif::true_random(); // conservative / progressive
        break;

    case Race::Index::troll:
        m_beliefs[0][0] = Beleif::strong_aversion(); // spiritual / matrialist
        m_beliefs[0][1] = Beleif::strong_affinity(); // violent / pacifist
        m_beliefs[1][0] = Beleif::strong_aversion(); // authoritarian / liberal
        m_beliefs[1][1] = Beleif::mild_affinity(); // conservative / progressive
        break;

    case Race::Index::beastmen:
        m_beliefs[0][0] = Beleif::strong_affinity(); // spiritual / matrialist
        m_beliefs[0][1] = Beleif::strong_affinity(); // violent / pacifist
        m_beliefs[1][0] = Beleif::strong_aversion(); // authoritarian / liberal
        m_beliefs[1][1] = Beleif::true_random(); // conservative / progressive
        break;

    case Race::Index::zombie:
        m_beliefs[0][0] = Beleif::true_random(); // spiritual / matrialist
        m_beliefs[0][1] = Beleif::true_random(); // violent / pacifist
        m_beliefs[1][0] = Beleif::true_random(); // authoritarian / liberal
        m_beliefs[1][1] = Beleif::true_random(); // conservative / progressive
        break;

    case Race::Index::dwarf:
        m_beliefs[0][0] = Beleif::mild_aversion(); // spiritual / matrialist
        m_beliefs[0][1] = Beleif::mild_affinity(); // violent / pacifist
        m_beliefs[1][0] = Beleif::strong_affinity(); // authoritarian / liberal
        m_beliefs[1][1] = Beleif::strong_affinity(); // conservative / progressive
        break;

    default:
        m_beliefs[0][0] = Beleif::mild_aversion();
        m_beliefs[0][1] = Beleif::mild_affinity();
        m_beliefs[1][0] = Beleif::strong_aversion();
        m_beliefs[1][1] = Beleif::strong_affinity();
        break;
    }

    normalise_beliefs();
}


void Entity::normalise_beliefs()
{
    /*
    Normalises beliefs to between 0 and 1.
    */
    float belief_sum{
        m_beliefs[0][0] + m_beliefs[0][1] + m_beliefs[1][0] + m_beliefs[1][1]
    };
    m_beliefs[0][0] /= belief_sum;
    m_beliefs[0][1] /= belief_sum;
    m_beliefs[1][0] /= belief_sum;
    m_beliefs[1][1] /= belief_sum;
}


void Entity::add_child(Entity* entity)
{
    m_children.push_back(entity); // m_children does nothing and can contain a dangling ptr, this is a wasteful operation
}


void Entity::add_link(Entity* entity)
{
	m_links.push_back(entity);
}


void Entity::remove_link(Entity* entity_to_remove, bool remove_from_both)
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
    if(remove_from_both) entity_to_remove->remove_link(this, false);
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
    if (this->get_home()) entity->set_home(this->get_home());
    else if (entity->get_home()) this->set_home(entity->get_home());
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


void Entity::update_beliefs(Entity* influencer) // influencer should be const?
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

    // normalise the beliefs to 0-1 (could use func but think his is faster??)
    float result_sum{
        result[0][0] + result[0][1] + result[1][0] + result[1][1]
    };
    m_beliefs[0][0] = result[0][0] / result_sum;
    m_beliefs[0][1] = result[0][1] / result_sum;
    m_beliefs[1][0] = result[1][0] / result_sum;
    m_beliefs[1][1] = result[1][1] / result_sum;
}


void Entity::do_random_walks(std::map<int, int>& map_to_fill, const int steps, const int walks)
{
    std::set<int> visited_ids;
    int current_id;
    Entity* current_entity{ this };
    if (m_links.size() > 0)
    {
        for (int i{ 0 }; i < walks; ++i)
        {
            for (int j{ 0 }; j < steps; ++j)
            {
                current_entity = current_entity->m_links[
                    uniform_distribution_int(0, static_cast<int>(current_entity->m_links.size()) - 1)
                ];
                current_id = current_entity->get_id();
                map_to_fill[current_id] += 1;
                visited_ids.insert(current_id);
            }
            current_entity = this;
        }
    }
}


float Entity::get_abs_belief_diff(Entity* entity) const
{
    float abs_belief_diff{};
    abs_belief_diff += abs(m_beliefs[0][0] - entity->m_beliefs[0][0]);
    abs_belief_diff += abs(m_beliefs[0][1] - entity->m_beliefs[0][1]);
    abs_belief_diff += abs(m_beliefs[1][0] - entity->m_beliefs[1][0]);
    abs_belief_diff += abs(m_beliefs[1][1] - entity->m_beliefs[1][1]);
    return abs_belief_diff;
}


void Entity::set_home(Structure* home_structure)
{
    m_home = home_structure;
}
