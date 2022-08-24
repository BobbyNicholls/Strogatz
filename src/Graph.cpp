#include "Graph.h"


unsigned int get_preferential_entity_iloc(
    id_t links[link_limit][2],
    unsigned int& counter
)
{
    unsigned int link_iloc{
        static_cast<unsigned int>(uniform_distribution_int(1, counter * 2))
    };
    if (link_iloc > counter)
    {
        link_iloc -= (counter + 1);
        return links[link_iloc][1];
    }
    else
    {
        link_iloc -= 1;
        return links[link_iloc][0];
    }
}


template <typename Entity_t>
unsigned int add_preferential_links(
    const std::vector<Entity_t*>& entities,
    Entity_t* entity,
    id_t links[link_limit][2],
    unsigned int& counter
)
{
    /*
    For now, for `entity` node, just add a link to a node chosen at random weighted by number
    of existing edges.
    */
    unsigned int entity_iloc{ get_preferential_entity_iloc(links, counter) };
    Entity_t* chosen_entity{ entities[entity_iloc] };
    if (entity->is_linked_to(chosen_entity)) return entity_iloc;
    link_entities(chosen_entity, entity, links, counter);
    return entity_iloc;
}


void add_random_edge(Graph& graph, int max_entitiy_iloc)
{
    int random_entity_iloc{ uniform_distribution_int(0, max_entitiy_iloc) };
    add_preferential_links(
        graph.entities,
        graph.entities[random_entity_iloc],
        graph.links,
        graph.link_counter
    );
}


EntityCircle* get_entity_circle(id_t id, time_period_t time_period)
{
    EntityCircle* entity_pointer{ new (std::nothrow) EntityCircle(id, time_period) };
    if (!entity_pointer) // handle case where new returned null
    {
        // TODO: error handling here
        std::cerr << "\n\nCould not allocate memory!!!\n\n";
        throw - 1;
    }
    else return entity_pointer;
}


void rewire_random_edge(Graph& graph)
{
    int random_edge_iloc{ uniform_distribution_int(0, graph.link_counter - 1) };
    int random_end_iloc{ uniform_distribution_int(0, 1) };
    // wont always be source! only if random_end_iloc=0
    unsigned int source_iloc{ graph.links[random_edge_iloc][random_end_iloc] };
    unsigned int new_target_iloc{ get_preferential_entity_iloc(graph.links, graph.link_counter) };
    EntityCircle* source{ graph.entities[source_iloc] };
    EntityCircle* new_target{ graph.entities[new_target_iloc] };

    if ((source_iloc != new_target_iloc) && !new_target->is_linked_to(source))
    {
        // 1: remove existing edge
        id_t old_target_iloc{ graph.links[random_edge_iloc][1 - random_end_iloc] };
        EntityCircle* old_target{ graph.entities[old_target_iloc] };
        source->remove_link(old_target);
        // 2: add new edge
        link_entities(
            source,
            new_target,
            graph.links,
            random_edge_iloc
        );
        // TODO: replace these with a smooth move to destination function
        old_target->set_position_relative_to_links();
        source->set_position_relative_to_links();
    }
}


Graph get_barabasi_albert_graph(time_period_t time_period)
{
    /*
    For each edge:
     - add new node with preferential attachment
     - with chance `new_edge_prob` choose random node to attach to other node 
     preferentially
     - with chance `rewire_prob` randomly choose edge, rewire one randomly chosen end 
     preferentially
    */
    Graph graph{};
    std::set<unsigned int> link_anchors{ 0 };
    unsigned int entity_iloc;
    graph.entities.resize(entities_start_size); // initialise on creation??
    graph.entities.reserve(entities_reserve_limit);

    graph.entities[0] = get_entity_circle(0, time_period);
    graph.entities[0]->set_position_randomly();
    graph.entities[1] = get_entity_circle(1, time_period);
    link_entities(
        graph.entities[0],
        graph.entities[1],
        graph.links,
        graph.link_counter
    );

    for (int i{ 2 }; i < entities_start_size; ++i)
    {

        graph.entities[i] = get_entity_circle(i, time_period);

        // todo: make a set of non-anchors then make it less likely to attch to those?
        entity_iloc = add_preferential_links(
            graph.entities,
            graph.entities[i],
            graph.links,
            graph.link_counter
        );
        if (uniform_distribution_float(0, 1) < graph.new_edge_prob)
        {
            add_random_edge(graph, i-1);
        }
        if (uniform_distribution_float(0, 1) < graph.rewire_prob)
        {
            rewire_random_edge(graph);
        }
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

    return graph;
}


void draw_entities(Graph& graph, sf::RenderWindow& window)
{
    int i{ 0 };
    window.draw(graph.entities[i++]->get_shape());
    while (i != graph.entities.size())
    {
        random_move_entity(graph.entities[i]->get_shape());
        //slingshot_move_entity(graph.entities[i]);
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


void propagate_entities(Graph& graph, time_period_t time_period)
{
    for (unsigned int i{ 0 }; i < graph.link_counter; ++i)
    {
        float roll{ uniform_distribution_float(0, 1) };
        if (roll < graph.spawn_chance)
        {
            EntityCircle* from_entity{ graph.entities[graph.links[i][0]] };
            EntityCircle* to_entity{ graph.entities[graph.links[i][1]] };
            if (to_entity->is_paired() && (to_entity->get_partner() == from_entity))
            {
                std::cout << "Paired entity spawning child.\n";
                EntityCircle* child_entity{ get_entity_circle(
                    static_cast<id_t>(graph.entities.size()-1), time_period
                )};
                graph.entities.push_back(child_entity);
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
    std::cout << "Total graph nodes: " << graph.entities.size() << '\n';
}
