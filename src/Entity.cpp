#include <iostream>

#include "Entity.h"


Entity::Entity(id_t id)
	:m_id{ id }
{
    m_beliefs[0][0] = mild_aversion();
    m_beliefs[0][1] = mild_aversion();
    m_beliefs[1][0] = mild_aversion();
    m_beliefs[1][1] = mild_aversion();
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
