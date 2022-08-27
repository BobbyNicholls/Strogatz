#include "Graph.h"


float death_sigmoid(int age)
{
    if (age > 110) return 1.f;
    return 1 / (1 + exp(-((age - 85) * 0.3f)));;
}


void kill_entities(Graph& graph, time_period_t time_period)
{
    EntityCircle* dead_entity;
    for (int i{ 1 }; i<graph.entities.size(); ++i)
    {
        if (graph.entities[i] && (uniform_distribution_float(0, 1) <
            death_sigmoid(static_cast<int>((time_period - graph.entities[i]->get_birth_time()) / 10))))
        {
            dead_entity = graph.entities[i];
            std::cout << "Entity " << dead_entity->get_id() << " has died.\n";
            // delete entries in `links` list containing `dead_entity`
            graph.links.erase(std::remove_if(
                graph.links.begin(), graph.links.end(),
                [dead_entity](Link* link) {
                return (link->from==dead_entity || link->to == dead_entity);
            }), graph.links.end());
            for (Entity* linked_entity : dead_entity->get_links()) //is this editing the thing it is iterating over??
            {
                linked_entity->remove_link(dead_entity, false);
            }
            // put nullptr where the current ptr is in "graph.entities" / remove element
            graph.entities[i] = nullptr;
            // delete the entity, deallocate memory
            delete dead_entity;
        }
    }
    tidy_up_entities(graph);
}


void tidy_up_entities(Graph& graph)
{
    graph.entities.erase(std::remove_if(
        graph.entities.begin(), graph.entities.end(),
        [](EntityCircle* entity) {
            return (!entity);
        }), graph.entities.end());
}


EntityCircle* get_preferential_entity(Graph& graph)
{
    int links_vector_size{ static_cast<int>(graph.links.size()) };
    int link_iloc{ uniform_distribution_int(1, links_vector_size * 2) };
    if (link_iloc > links_vector_size)
    {
        link_iloc -= (links_vector_size + 1);
        return graph.links[link_iloc]->to;
    }
    else
    {
        link_iloc -= 1;
        return graph.links[link_iloc]->from;
    }
}


template <typename Entity_t>
void link_entities(Graph& graph, Entity_t* entity_from, Entity_t* entity_to)
{
    entity_from->add_link(entity_to);
    entity_to->add_link(entity_from);
    graph.links.push_back(new Link{ entity_from, entity_to });
}


template <typename Entity_t>
Entity_t* add_preferential_links(Graph& graph, Entity_t* entity)
{
    /*
    For now, for `entity` node, just add a link to a node chosen at random weighted by number
    of existing edges.
    */
    Entity_t* chosen_entity{ get_preferential_entity(graph) };
    if (chosen_entity == entity) return chosen_entity;
    if (entity->is_linked_to(chosen_entity)) return chosen_entity;
    link_entities(graph, chosen_entity, entity);
    return chosen_entity;
}


void add_random_edge(Graph& graph, int max_entitiy_iloc)
{
    int random_entity_iloc{ uniform_distribution_int(0, max_entitiy_iloc) };
    if (graph.entities[random_entity_iloc])
    {
        add_preferential_links(graph, graph.entities[random_entity_iloc]);
    }
    graph.entities[random_entity_iloc]->set_position_relative_to_links();
}


EntityCircle* get_entity_circle(time_period_t time_period)
{
    EntityCircle* entity_pointer{ new (std::nothrow) EntityCircle(time_period) };
    if (!entity_pointer) // handle case where new returned null
    {
        // TODO: error handling here
        std::cerr << "\n\nCould not allocate memory!!!\n\n";
        throw - 1;
    }
    return entity_pointer;
}


void rewire_random_edge(Graph& graph)
{
    int random_edge_iloc{ uniform_distribution_int(0, static_cast<int>(graph.links.size()) - 2)};
    int random_end_iloc{ uniform_distribution_int(0, 1) };

    EntityCircle* pivot_entity{
        random_end_iloc ? graph.links[random_edge_iloc]->to : graph.links[random_edge_iloc]->from
    };
    EntityCircle* old_target{
        !random_end_iloc ? graph.links[random_edge_iloc]->to : graph.links[random_edge_iloc]->from
    };
    EntityCircle* new_target{ get_preferential_entity(graph) };

    if ((pivot_entity != new_target) && 
        !(new_target->is_linked_to(pivot_entity)) && 
        (new_target != old_target))
    {
        pivot_entity->remove_link(old_target);
        graph.links.erase(graph.links.begin() + random_edge_iloc);
        link_entities(graph, pivot_entity, new_target);
        // TODO: replace these with a smooth move to destination function
        old_target->set_position_relative_to_links();
        pivot_entity->set_position_relative_to_links();
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
    graph.entities.reserve(entities_reserve_limit);
    graph.links.reserve(link_limit);

    graph.entities.push_back(get_entity_circle(time_period));
    graph.entities.push_back(get_entity_circle(time_period));
    link_entities(graph, graph.entities[0], graph.entities[1]);
    graph.entities[0]->set_position_randomly();
    graph.entities[1]->set_position_relative_to_links();

    EntityCircle* chosen_entity{ nullptr };
    std::set<EntityCircle*> link_anchors{ graph.entities[0] };
    for (int i{ 2 }; i < entities_start_size; ++i)
    {
        EntityCircle* new_entity{ get_entity_circle(time_period) };
        graph.entities.push_back(new_entity);
        // todo: make a set of non-anchors then make it less likely to attch to those?
        chosen_entity = add_preferential_links(graph, new_entity);
        if (uniform_distribution_float(0, 1) < graph.new_edge_prob)
        {
            add_random_edge(graph, static_cast<int>(graph.entities.size()) - 1);
        }
        if (uniform_distribution_float(0, 1) < graph.rewire_prob)
        {
            rewire_random_edge(graph);
        }
        if (link_anchors.contains(chosen_entity))
        {
            new_entity->set_position_relative_to_links();
        }
        else
        {
            new_entity->set_position_randomly();
            link_anchors.insert(new_entity);
        }
    }

    return graph;
}


void draw_entities(Graph& graph, sf::RenderWindow& window)
{
    sf::CircleShape shape;
    int i{ 0 };
    window.draw(graph.entities[i++]->get_shape());
    while (i != graph.entities.size())
    {
        if (graph.entities[i])
        {
            shape = graph.entities[i]->get_shape();
            random_move_entity(shape);
            //slingshot_move_entity(entity);
            window.draw(shape);
        }
        ++i;
    }
;

}


void draw_links(Graph& graph, sf::RenderWindow& window)
{
    for (Link* link: graph.links)
    {
        sf::Vector2f pos0{ link->from->get_shape().getPosition() };
        sf::Vector2f pos1{ link->to->get_shape().getPosition() };
        float radius0{ link->from->get_radius() };
        float radius1{ link->to->get_radius() };
        sf::Vertex line[2]{
            sf::Vertex(sf::Vector2f(pos0.x + radius0, pos0.y + radius0)),
            sf::Vertex(sf::Vector2f(pos1.x + radius1, pos1.y + radius1))
        };
        window.draw(line, 2, sf::Lines);
    }
}


void forward_propagate_beliefs(Graph& graph)
{
    for (Link* link : graph.links)
    {
        link->to->update_beliefs(link->from);
        link->to->update_colour();
    }
}


void propagate_entities(Graph& graph, time_period_t time_period)
{
    for (Link* link : graph.links)
    {
        if (uniform_distribution_float(0, 1) < graph.spawn_chance)
        {
            EntityCircle* from_entity{ link->from };
            EntityCircle* to_entity{ link->to };
            if (to_entity->is_paired() && (to_entity->get_partner() == from_entity))
            {
                EntityCircle* child_entity{ get_entity_circle(time_period)};
                std::cout << "Paired entities spawned a child: " << 
                    child_entity->get_id() << '\n';
                graph.entities.push_back(child_entity);
                from_entity->add_child(child_entity);
                to_entity->add_child(child_entity);
                child_entity->add_parents(from_entity, to_entity);
                link_entities(graph, from_entity, child_entity);
                link_entities(graph, to_entity, child_entity);
                child_entity->set_position_randomly();
            }

            else if (
                (!from_entity->is_paired() && !to_entity->is_paired()) &&
                (from_entity->get_sex() != to_entity->get_sex())
                )
            {
                std::cout << "Unpaired Entities matched\n";
                to_entity->add_partner(from_entity);
            }
        }
    }
    std::cout << "Total graph nodes: " << graph.entities.size() << '\n';
}
