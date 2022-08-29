#include <SFML/Graphics.hpp>

#include "Graph.h"


void Graph::link_entities(EntityCircle* entity_from, EntityCircle* entity_to)
{
    entity_from->add_link(entity_to);
    entity_to->add_link(entity_from);
    m_links.push_back(new Link{ entity_from, entity_to });
}


EntityCircle* Graph::get_preferential_entity()
{
    /*
    Selects an entity "preferentially" from the list of currently existing entities, preferentially means
    that the likelihood of selecting a given entity is proportional to its degree centrality (number of
    connected edges).
    */
    const int links_vector_size{ static_cast<int>(m_links.size()) };
    int link_iloc{ uniform_distribution_int(1, links_vector_size * 2) };

    if (link_iloc > links_vector_size)
    {
        link_iloc -= (links_vector_size + 1);
        return m_links[link_iloc]->to;
    }
    else
    {
        link_iloc -= 1;
        return m_links[link_iloc]->from;
    }
}


EntityCircle* Graph::add_preferential_links(EntityCircle* entity)
{
    /*
    For now, for `entity` node, just add a link to a node chosen at random weighted by number
    of existing edges.
    */
    EntityCircle* chosen_entity{ get_preferential_entity() };

    if (chosen_entity == entity) return chosen_entity;
    if (entity->is_linked_to(chosen_entity)) return chosen_entity;
    link_entities(chosen_entity, entity);
    return chosen_entity;
}


void Graph::add_random_edge(const int max_entitiy_iloc)
{
    const int random_entity_iloc{ uniform_distribution_int(0, max_entitiy_iloc) };

    if (m_entities[random_entity_iloc])
    {
        add_preferential_links(m_entities[random_entity_iloc]);
    }
    m_entities[random_entity_iloc]->set_position_relative_to_links();
}


void Graph::rewire_random_edge()
{
    /*
    This takes a compeltely random link, takes a random end of that link, detaches it from the `old target`
    but keeps the other end attached to the `pivot_node`, then re-attaches it preferentially to a 
    `new_target`.
    */
    const int random_link_iloc{ uniform_distribution_int(0, static_cast<int>(m_links.size()) - 2) };
    const int random_end_iloc{ uniform_distribution_int(0, 1) };

    EntityCircle* pivot_entity{
        random_end_iloc ? m_links[random_link_iloc]->to : m_links[random_link_iloc]->from
    };
    EntityCircle* old_target{
        !random_end_iloc ? m_links[random_link_iloc]->to : m_links[random_link_iloc]->from
    };
    EntityCircle* new_target{ get_preferential_entity() };

    if ((pivot_entity != new_target) && 
        !(new_target->is_linked_to(pivot_entity)) && 
        (new_target != old_target))
    {
        pivot_entity->remove_link(old_target);
        delete m_links[random_link_iloc];
        m_links.erase(m_links.begin() + random_link_iloc);
        link_entities(pivot_entity, new_target);
        // TODO: replace these with a smooth move to destination function
        old_target->set_position_relative_to_links();
        pivot_entity->set_position_relative_to_links();
    }
}


Graph::Graph(
    const time_period_t start_time,
    const float rewire_prob,
    const float new_edge_prob,
    const float spawn_chance,
    const int entities_start_size,
    const int entities_reserve_limit,
    const int link_limit
)
    : m_rewire_prob{ rewire_prob },
    m_new_edge_prob{ new_edge_prob },
    m_spawn_chance{ spawn_chance },
    m_entities_start_size { entities_start_size }
{
    /*
    Creates a Barabasi-Albert graph with slight modification for performance.
    For each edge:
     - add new node with preferential attachment
     - with chance `new_edge_prob` choose random node to attach to other node 
     preferentially
     - with chance `rewire_prob` randomly choose edge, rewire one randomly chosen end 
     preferentially
    */
    // todo: make a set of non-anchors then make it less likely to attch to those?

    m_entities.reserve(entities_reserve_limit);
    m_links.reserve(link_limit);

    m_entities.push_back(get_entity_circle(start_time));
    m_entities.push_back(get_entity_circle(start_time));
    link_entities(m_entities[0], m_entities[1]);
    m_entities[0]->set_position_randomly();
    m_entities[1]->set_position_relative_to_links();

    EntityCircle* chosen_entity{ nullptr };
    std::set<EntityCircle*> link_anchors{ m_entities[0] };
    for (int i{ 2 }; i < m_entities_start_size; ++i)
    {
        EntityCircle* new_entity{ get_entity_circle(start_time) };
        m_entities.push_back(new_entity);
        chosen_entity = add_preferential_links(new_entity);
        if (uniform_distribution_float(0, 1) < m_new_edge_prob)
        {
            add_random_edge(static_cast<int>(m_entities.size()) - 1);
        }
        if (uniform_distribution_float(0, 1) < m_rewire_prob)
        {
            rewire_random_edge();
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
}


void Graph::draw_entities(sf::RenderWindow& window, const float move_distance)
{
    float x_move_distance{ 0.f };
    float y_move_distance{ 0.f };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) x_move_distance += move_distance;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) x_move_distance -= move_distance;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) y_move_distance += move_distance;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) y_move_distance -= move_distance;

    sf::CircleShape shape;
    for (EntityCircle* entity: m_entities)
    {
        if (entity)
        {
            //random_move_entity(shape);
            entity->get_shape().move(x_move_distance, y_move_distance);
            //slingshot_move_entity(entity);
            window.draw(entity->get_shape());
        }
    }
}


void Graph::draw_links(sf::RenderWindow& window)
{
    for (Link* link: m_links)
    {
        const sf::Vector2f& pos0{ link->from->get_shape().getPosition() };
        const sf::Vector2f& pos1{ link->to->get_shape().getPosition() };
        const float radius0{ link->from->get_radius() };
        const float radius1{ link->to->get_radius() };
        sf::Vertex line[2]{
            sf::Vertex(sf::Vector2f(pos0.x + radius0, pos0.y + radius0)),
            sf::Vertex(sf::Vector2f(pos1.x + radius1, pos1.y + radius1))
        };
        window.draw(line, 2, sf::Lines);
    }
}


void Graph::forward_propagate_beliefs()
{
    for (Link* link : m_links)
    {
        link->to->update_beliefs(link->from);
        link->to->update_colour();
    }
}


void Graph::propagate_entities(const time_period_t time_period)
{
    for (Link* link : m_links)
    {
        if (uniform_distribution_float(0, 1) < m_spawn_chance)
        {
            EntityCircle* from_entity{ link->from };
            EntityCircle* to_entity{ link->to };
            if (to_entity->is_paired() && (to_entity->get_partner() == from_entity))
            {
                EntityCircle* child_entity{ get_entity_circle(time_period) };
                std::cout << "Paired entities spawned a child: " << 
                    child_entity->get_id() << '\n';
                m_entities.push_back(child_entity);
                from_entity->add_child(child_entity);
                to_entity->add_child(child_entity);
                child_entity->add_parents(from_entity, to_entity);
                link_entities(from_entity, child_entity);
                link_entities(to_entity, child_entity);
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
    std::cout << "Total graph nodes: " << m_entities.size() << '\n';
}


float Graph::death_sigmoid(const int age, const int fifty_pct_age, const int hundred_pct_age)
{
    if (age > hundred_pct_age) return 1.f;
    return 1 / (1 + exp(-((age - fifty_pct_age) * 0.3f)));;
}


void Graph::tidy_up_entities()
{
    // Removes null pointers from graph.entities
    m_entities.erase(std::remove_if(
        m_entities.begin(), m_entities.end(),
        [](EntityCircle* entity) {
            return (!entity);
        }), m_entities.end());
}


void Graph::kill_entities(const time_period_t time_period)
{
    /*
    Runs through all entities and calculates chance of death by way of a sigmoid function where the x
    value is the age in `time_period`s divided by 10. Rolls a dice and if the roll passes the entity is
    removed from the graph.
    */
    EntityCircle* dead_entity;

    for (int i{ 0 }; i < m_entities.size(); ++i)
    {
        if (m_entities[i] && (uniform_distribution_float(0, 1) <
            death_sigmoid(static_cast<int>((time_period - m_entities[i]->get_birth_time()) / 10))))
        {
            dead_entity = m_entities[i];
            std::cout << "Entity " << dead_entity->get_id() << " has died.\n";
            // delete entries in `links` list containing `dead_entity`
            m_links.erase(std::remove_if(// THIS IS LEAKING MEMORY
                m_links.begin(), m_links.end(),
                [dead_entity](Link* link) {
                    return (link->from == dead_entity || link->to == dead_entity);
                }), m_links.end());
            for (Entity* linked_entity : dead_entity->get_links())
            {
                linked_entity->remove_link(dead_entity, false);
            }
            // put nullptr where the current ptr is in "m_entities"
            m_entities[i] = nullptr;
            // delete the entity, deallocate memory
            delete dead_entity;
        }
    }
    tidy_up_entities();
}
