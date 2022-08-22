#include "Graph.h"

Graph get_graph(time_period_t time_period)
{
    Graph graph{};
    std::set<unsigned int> link_anchors{ 0 };
    unsigned int entity_iloc;
    graph.entities.resize(entities_start_size); // initialise on creation??
    graph.entities.reserve(entities_reserve_limit);
    for (int i{ 0 }; i < entities_start_size; ++i)
    {
        EntityCircle* entity_pointer{ new (std::nothrow) EntityCircle(i, time_period) };
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


void forward_propagate_beliefs(Graph& graph)
{
    for (unsigned int i{ 0 }; i < graph.link_counter; ++i)
    {
        graph.entities[graph.links[i][1]]->update_beliefs(
            graph.entities[graph.links[i][0]]
        );
        graph.entities[graph.links[i][1]]->update_colour();
    }
}


void propagate_entities(Graph& graph, time_period_t time_period, float spawn_chance)
{
    std::cout << "Propagating entities...\n\n";
    for (unsigned int i{ 0 }; i < graph.link_counter; ++i)
    {
        float roll{ uniform_distribution_float(0, 1) };
        if (roll > spawn_chance)
        {
            EntityCircle* from_entity{ graph.entities[graph.links[i][0]] };
            EntityCircle* to_entity{ graph.entities[graph.links[i][1]] };
            if (to_entity->is_paired() && (to_entity->get_partner() == from_entity))
            {
                std::cout << "Entity is paired, spawning child.\n";
                EntityCircle* child_entity{ new (std::nothrow) EntityCircle(
                    static_cast<id_t>(graph.entities.size()), time_period
                )};
                if (!child_entity)
                {
                    // TODO: error handling here
                    std::cerr << "\n\nCould not allocate memory!!!\n\n";
                }
                else
                {
                    graph.entities.push_back(child_entity);
                }
                from_entity->add_child(child_entity);
                to_entity->add_child(child_entity);
                child_entity->add_parents(from_entity, to_entity);
                link_entities(
                    from_entity,
                    child_entity,
                    graph.links,
                    graph.link_counter
                );
                link_entities(
                    to_entity,
                    child_entity,
                    graph.links,
                    graph.link_counter
                );
                child_entity->set_position_relative_to_links();
            }

            else if (
                (!from_entity->is_paired() && !to_entity->is_paired()) &&
                (from_entity->get_sex() != to_entity->get_sex())
                )
            {
                std::cout << "Unpaired Entity found match, pairing entities.\n";
                to_entity->add_partner(from_entity);
            }
        }
    }
}
