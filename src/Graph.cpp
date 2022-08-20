#include "Graph.h"

Graph get_graph()
{
    Graph graph{};
    std::set<unsigned int> link_anchors{ 0 };
    unsigned int entity_iloc;
    graph.entities.resize(entity_limit); // initialise on creation??
    for (int i{ 0 }; i < entity_limit; ++i)
    {
        EntityCircle* entity_pointer{ new (std::nothrow) EntityCircle(i) };
        if (!entity_pointer) // handle case where new returned null
        {
            // TODO: error handling here
            std::cerr << "\n\nCould not allocate memory!!!\n\n";
        }
        else
        {
            graph.entities[i] = entity_pointer;
            if (i == 0)
            {
                graph.entities[i]->set_position_randomly();
            }
            else if (i == 1)
            {
                link_entities(
                    graph.entities[0],
                    graph.entities[1],
                    graph.links,
                    graph.link_counter
                );
                graph.entities[i]->set_position_relative_to_links();
            }
            else // todo: make a set of non-anchors then make it less likely to attch to those
            {
                entity_iloc = add_semi_random_links(
                    graph.entities,
                    graph.entities[i],
                    graph.links,
                    graph.link_counter
                );
                if (link_anchors.contains(entity_iloc))
                {
                    graph.entities[i]->set_position_relative_to_links();
                }
                else
                {
                    graph.entities[i]->set_position_randomly();
                    link_anchors.insert(i);
                }
            }
        }
    }

    return graph;
}
