#include "Graph.h"
#include "utils.h"

id_t generate_id()
{
    static id_t s_entity_id{ 0 };
    return s_entity_id++;
}


bool check_for_double_linkage(Graph& graph)
{
    size_t link_problems[link_limit][2]{};
    bool return_value{ false };
    for (size_t i{ 0 }; i < graph.links.size(); ++i)
    {
        EntityCircle* a{ graph.links[i]->from };
        EntityCircle* b{ graph.links[i]->to };
        if (a == b)
        {
            std::cout << "self linkage: ";
            return_value = true;
            link_problems[i][0] = 1;
        }
        for (size_t j{ i + 1 }; j < graph.links.size(); ++j)
        {
            EntityCircle* a_compare{ graph.links[j]->from };
            EntityCircle* b_compare{ graph.links[j]->to };
            if ((a == a_compare) && (b == b_compare))
            {
                std::cout << "i: " << i << ", j: " << j << ", repeat links: ";
                return_value = true;
                link_problems[i][1] = 1;
            }
        }
    }
    return return_value;
}


bool check_for_entity_position_bug(Graph& graph)
{
    for (EntityCircle* entity : graph.entities)
    {
        if (entity)
        {
            sf::Vector2f pos{ entity->get_shape().getPosition() };
            if (pos.x < 0)
            {
                std::cout << "Problem, pos x is: " << pos.x << '\n';
                return true;
            }
            else if (pos.y < 0)
            {
                std::cout << "Problem, pos y is: " << pos.y << '\n';
                return true;
            }
        }
    }
    return false;
}