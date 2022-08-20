#include "Graph.h"

Graph get_graph()
{
    Graph graph{};
    std::set<unsigned int> link_anchors{ 0 };
    unsigned int entity_iloc;
    graph.entities.resize(entity_limit); // initialise on creation??
    graph.entities.reserve(entity_reserve_limit);
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


void forward_propagate_beliefs(Graph& graph)
{
    graph.entities[0]->print_beliefs();
    for (unsigned int i{ 0 }; i < graph.link_counter; ++i)
    {
        graph.entities[graph.links[i][1]]->update_beliefs(
            graph.entities[graph.links[i][0]]
        );
        graph.entities[graph.links[i][1]]->update_colour();
        graph.entities[graph.links[i][1]]->print_beliefs();
    }
}


void draw_links(Graph& graph, sf::RenderWindow& window)
{
    for (unsigned int i{ 0 }; i < graph.link_counter; ++i)
    {
        sf::Vector2f pos0{ graph.entities[graph.links[i][0]]->get_shape().getPosition() };
        sf::Vector2f pos1{ graph.entities[graph.links[i][1]]->get_shape().getPosition() };
        float radius0{ graph.entities[graph.links[i][0]]->get_radius() };
        float radius1{ graph.entities[graph.links[i][0]]->get_radius() };
        sf::Vertex line[2]{
            sf::Vertex(sf::Vector2f(pos0.x + radius0, pos0.y + radius0)),
            sf::Vertex(sf::Vector2f(pos1.x + radius1, pos1.y + radius1))
        };
        window.draw(line, 2, sf::Lines);
    }
}


void draw_entities(Graph& graph, sf::RenderWindow& window)
{
    int i{ 0 };
    window.draw(graph.entities[i++]->get_shape());
    while (i != graph.entities.size())
    {
        random_move_entity(graph.entities[i]->get_shape());
        slingshot_move_entity(graph.entities[i]);
        window.draw(graph.entities[i++]->get_shape());
    }
}
