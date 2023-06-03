#include <cassert>
#include <SFML/Graphics.hpp>

#include "Graph.h"

// this is the chance of an entity choosing popularity over belief compatibility when being connected to
// the graph or having their edge rewired.
float POPULARITY_PROB{ 0.3f };


Graph::Graph(
    const time_period_t start_time,
    const Races* races,
    const float min_x,
    const float max_x,
    const float min_y,
    const float max_y,
    const float rewire_prob,
    const float new_edge_prob,
    const float spawn_chance,
    const int entities_start_size,
    const int entities_reserve_limit,
    const int link_limit,
    const int clique_min_size,
    const int clique_max_size
)
    : m_min_x{ min_x },
    m_max_x{ max_x },
    m_min_y{ min_y },
    m_max_y{ max_y },
    m_rewire_prob{ rewire_prob },
    m_new_edge_prob{ new_edge_prob },
    m_spawn_chance{ spawn_chance },
    m_entities_start_size{ entities_start_size },
    m_clique_min_size{ clique_min_size },
    m_clique_max_size{ clique_max_size },
    m_link_limit{ link_limit }
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

    sf::Vector2f pos{};
    assert((m_min_x < m_max_x) && "Graph min_x must be smaller than max_x");
    assert((m_min_y < m_max_y) && "Graph min_y must be smaller than max_y");
    set_offset(m_min_x, m_min_y);
    m_entities.reserve(entities_reserve_limit);
    m_links.reserve(m_link_limit);
    m_entity_vectors.reserve(100);

    m_entities.push_back(get_entity_circle(start_time, races->get_random_race()));
    m_entities.push_back(get_entity_circle(start_time, races->get_random_race()));
    link_entities(m_entities[0], m_entities[1]);
    m_entities[0]->set_position_randomly(m_min_x, m_max_x, m_min_y, m_max_y);
    m_entities[1]->set_position_randomly(m_min_x, m_max_x, m_min_y, m_max_y)->move_to_links();

    EntityCircle* chosen_entity{ nullptr };
    std::set<EntityCircle*> link_anchors{ m_entities[0] };
    for (int i{ 2 }; i < m_entities_start_size; ++i)
    {
        EntityCircle* new_entity{ get_entity_circle(start_time, races->get_random_race()) };
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
            new_entity->set_position_relative_to_links(m_min_x, m_max_x, m_min_y, m_max_y);
        }
        else
        {
            new_entity->set_position_randomly(m_min_x, m_max_x, m_min_y, m_max_y);
            link_anchors.insert(new_entity);
        }
        pos = new_entity->get_shape().getPosition();
        if (pos.x > m_max_entity_x_pos) m_max_entity_x_pos = pos.x;
        else if (pos.x < m_min_entity_x_pos) m_min_entity_x_pos = pos.x;
        if (pos.y > m_max_entity_y_pos) m_max_entity_y_pos = pos.y;
        else if (pos.y < m_min_entity_y_pos) m_min_entity_y_pos = pos.y;
    }
    seed_cliques_and_leaders();
    m_anchor_points.reserve(link_anchors.size());
    m_structures.reserve(link_anchors.size());
    for (EntityCircle* anchor : link_anchors)
    {
        m_anchor_points.push_back(anchor->get_shape().getPosition());
        m_structures.push_back(Structure(
            static_cast<float>(m_anchor_points.back().x),
            static_cast<float>(m_anchor_points.back().y),
            Structure::Index::home
        ));
        anchor->set_home(&(m_structures.back()));
    }
}


void Graph::link_entities(EntityCircle* entity_from, EntityCircle* entity_to)
{
    if (!entity_from->is_linked_to(entity_to))
    {
        entity_from->add_link(entity_to);
        entity_to->add_link(entity_from);
        m_links.push_back(new Link{ entity_from, entity_to });
    }
}


EntityCircle* Graph::get_preferential_entity() const
{
    /*
    Selects an entity "preferentially" from the list of currently existing entities, preferentially means
    that the likelihood of selecting a given entity is proportional to its degree centrality (number of
    connected edges).
    */
    std::cout << "PREFERNTIAL LINK" << '\n';
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


EntityCircle* Graph::get_belief_compatible_entity(EntityCircle* entity) const
{
    /*
    Gets the most belief compatible entity for the input `entity` that the input 
    `entity` is not already linked to.
    */
    std::cout << "BELIEF LINK" << '\n';
    EntityCircle* chosen_entity{};
    float min_belief_distance{ 9999.f };
    float current_belief_distance;
    for (EntityCircle* candidate_entity : m_entities)
    {
        if (!entity->is_linked_to(candidate_entity) and !(entity==candidate_entity))
        {
            current_belief_distance = entity->get_abs_belief_diff(candidate_entity);
            if (current_belief_distance < min_belief_distance)
            {
                min_belief_distance = current_belief_distance;
                chosen_entity = candidate_entity;
            }
        }
    }
    return chosen_entity;
}


EntityCircle* Graph::add_preferential_links(EntityCircle* entity)
{
    /*
    For now, for `entity` node, just add a link to a node chosen at random weighted by number
    of existing edges.
    */
    EntityCircle* chosen_entity;
    if (uniform_distribution_float(0, 1) < POPULARITY_PROB) chosen_entity = get_preferential_entity();
    else chosen_entity = get_belief_compatible_entity(entity);

    if (chosen_entity == entity) return chosen_entity;
    if (!chosen_entity) return chosen_entity;
    link_entities(chosen_entity, entity);
    return chosen_entity;
}


void Graph::add_random_edge(const int max_entitiy_iloc)
{
    std::cout << "Adding random edge." << '\n';
    const int random_entity_iloc{ uniform_distribution_int(0, max_entitiy_iloc) };
    EntityCircle* linked_entity;

    if (m_entities[random_entity_iloc])
    {
        linked_entity = add_preferential_links(m_entities[random_entity_iloc]);
        m_entities[random_entity_iloc]->move_to_entity(linked_entity);
    }
}


void Graph::rewire_random_edge()
{
    /*
    This takes a compeltely random link, takes a random end of that link, detaches it from the `old target`
    but keeps the other end attached to the `pivot_node`, then re-attaches it preferentially to a 
    `new_target`.
    */
    std::cout << "Rewiring random edge." << '\n';
    EntityCircle* new_target;
    const int random_link_iloc{ uniform_distribution_int(0, static_cast<int>(m_links.size()) - 2) };
    const int random_end_iloc{ uniform_distribution_int(0, 1) };

    EntityCircle* pivot_entity{
        random_end_iloc ? m_links[random_link_iloc]->to : m_links[random_link_iloc]->from
    };
    EntityCircle* old_target{
        !random_end_iloc ? m_links[random_link_iloc]->to : m_links[random_link_iloc]->from
    };

    if (uniform_distribution_float(0, 1) < POPULARITY_PROB) new_target = get_preferential_entity();
    else new_target = get_belief_compatible_entity(pivot_entity);

    if ((pivot_entity != new_target) && 
        !(new_target->is_linked_to(pivot_entity)) && 
        (new_target != old_target))
    {
        pivot_entity->remove_link(old_target);
        delete m_links[random_link_iloc];
        m_links.erase(m_links.begin() + random_link_iloc);
        link_entities(pivot_entity, new_target);
        old_target->move_to_links();
        pivot_entity->move_to_entity(new_target);
    }
}


void Graph::draw_entities(
    sf::RenderWindow& window, const float x_move_distance, const float y_move_distance
)
{
    sf::CircleShape shape;
    for (EntityCircle* entity: m_entities)
    {
        if (entity)
        {
            random_move_entity(shape);
            entity->get_shape().move(x_move_distance, y_move_distance);
            if (entity->is_pathing()) entity->move_along_path();
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
            from_entity->move_to_home(m_current_offset.x, m_current_offset.y);
            EntityCircle* to_entity{ link->to };
            to_entity->move_to_home(m_current_offset.x, m_current_offset.y);
            std::cout << "Entities are going home\n";
            if (to_entity->is_paired() && (to_entity->get_partner() == from_entity))
            {
                EntityCircle* child_entity{ get_entity_circle(time_period, to_entity->get_race()) };
                std::cout << "Paired entities spawned a child: " << 
                    child_entity->get_id() << '\n';
                m_entities.push_back(child_entity);
                from_entity->add_child(child_entity);
                to_entity->add_child(child_entity);
                child_entity->add_parents(from_entity, to_entity);
                link_entities(from_entity, child_entity);
                link_entities(to_entity, child_entity);
                child_entity->set_home(from_entity->get_home());
                child_entity->set_position_to_home();
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


void Graph::form_clique_from_seed(const int seed)
{
    int clique_size{ uniform_distribution_int(m_clique_min_size, m_clique_max_size) };
    std::vector<EntityCircle*> clique(clique_size);
    float abs_error;
    float abs_belief_error;
    float max_abs_error{ 0.f };
    int m_entities_size{ static_cast<int>(m_entities.size()) };
    std::vector<std::tuple<float, float, int>> abs_errors(m_entities_size);
    EntityVector& seed_vector = m_entity_vectors[seed];

    for (int i{ 0 }; i < m_entities_size; ++i)
    {
        abs_error = 0.f;
        if (i != seed)
        {
            for (int j{ 0 }; j < m_entities_size; ++j)
            {
                abs_error += abs(
                    seed_vector.entity_vector[j] - m_entity_vectors[i].entity_vector[j]
                );
            }
        }
        if (abs_error > max_abs_error) max_abs_error = abs_error;
        abs_belief_error = m_entities[seed]->get_abs_belief_diff(m_entities[i]);
        abs_errors[i] = std::tuple<float, float, int>{ abs_belief_error, abs_error, i };
    }
    for (std::tuple<float, float, int>& abs_err : abs_errors)
    {
        std::get<1>(abs_err) = (std::get<1>(abs_err)/max_abs_error) + std::get<0>(abs_err);
    }
    std::sort(
        abs_errors.begin(), abs_errors.end(),
        [](std::tuple<float, float, int>& t1, std::tuple<float, float, int>& t2) {
            return std::get<1>(t1) < std::get<1>(t2);
        }
    );
    clique[0] = m_entities[seed];
    for (int i{ 1 }; i < clique_size; ++i) clique[i] = m_entities[std::get<2>(abs_errors[i])];
    for (int i{ 0 }; i < clique_size; ++i)
    {
        for (int j{ i+1 }; j < clique_size; ++j)
        {
            if (!clique[i]->is_linked_to(clique[j])) link_entities(clique[i], clique[j]);
        }
    }
    for (int i{ 1 }; i < clique_size; ++i) clique[i]->move_to_entity(clique[0]);
}


void Graph::make_leader(EntityCircle* leader)
{
    std::cout << "Entity " << leader->get_id() << " is a leader with " <<
        leader->get_links().size() << " links.\n";
    m_leaders.push_back(leader);
}


void Graph::vectorise_nodes(const bool vectorise_all_nodes)
{
    /*
    Create a map with the keys being the ids of all nodes in the graph and the values being all 0s.
    For each entity do a number of random walks, count the IDs of nodes visited in the map.
    Now each node/entity in the Graph is represented by a map, with a value vector that can be compared.
    Do the inner product / cosine similarity / MAD to compare these vectors for similarity.
    Now compare the belief vectors.
    */
    if (vectorise_all_nodes) std::cout << "Vectorising all nodes..\n";
    
    m_entity_vectors.resize(0);
    std::map<int, int> map_to_fill;
    int m_entities_size{ static_cast<int>(m_entities.size()) };
    std::vector<int> entity_ids;
    entity_ids.reserve(m_entities_size);
    for (EntityCircle* entity : m_entities) entity_ids.push_back(entity->get_id());
    for (EntityCircle* entity : m_entities)
    {
        EntityVector entity_vector;
        entity_vector.entity_vector.reserve(m_entities_size);
        for (int id : entity_ids) map_to_fill[id] = 0;
        entity->do_random_walks(map_to_fill);
        for (std::map<int, int>::iterator it{ map_to_fill.begin() }; it != map_to_fill.end(); ++it)
        {
            entity_vector.entity_vector.push_back(it->second);
        }
        m_entity_vectors.push_back(entity_vector);
    }
}


void Graph::seed_cliques_and_leaders(const int leaders, const int cliques)
{
    /*
    Leaders are assigned based on highest degree centrality.
    The graph is seeded with random belief values, the leaders need to be assigned distinct 
    belief systems which are specific to them, and dependent on race/faction (NOT IMPLMENETED
    YET).
    Clique formation starts with a random seed, a node to form a clique around, the closest N
    nodes in terms of both belief compatibility and graph closeness are then attached to the
    node and each other.
    */
    std::cout << "Forming " << cliques << " cliques and " << leaders << " leaders.\n";
    std::vector<int> clique_seed_ilocs(m_entities.size());
    for (int i{ 0 }; i < m_entities.size(); ++i)
    {
        clique_seed_ilocs[i] = i;
    }
    shuffle_vector(clique_seed_ilocs);
    clique_seed_ilocs.resize(cliques);
    vectorise_nodes();
    for (int i : clique_seed_ilocs) form_clique_from_seed(i);
    // loop through enities, find leaders, and connect cliques
    std::sort(
        m_entities.begin(), m_entities.end(), 
        [](EntityCircle* e1, EntityCircle* e2) { 
            return e1->get_links().size() > e2->get_links().size(); 
        }
    );
    for (int i{ 0 }; i < leaders; ++i) make_leader(m_entities[i]);
}


void Graph::reserve_more_links(const float increment_fraction)
{
    m_link_limit = static_cast<int>(m_link_limit * increment_fraction);
    m_links.reserve(m_link_limit);
}


void Graph::check_entities_have_homes()
{
    std::cout << "Checking entity homes:\n";
    int homeless_count{};
    for (EntityCircle* entity : m_entities)
    {
        std::cout << "Entity " << entity->get_id();
        if (entity->get_home()) std::cout << " has a home\n";
        else
        {
            std::cout << " is homeless\n";
            ++homeless_count;
        }
    }
    std::cout << homeless_count << " total entities are homeless :(\n";
}


void Graph::update_offset(const float x, const float y)
{
    m_current_offset.x += x;
    m_current_offset.y += y;
}


void Graph::set_offset(const float x, const float y)
{
    m_current_offset.x = x;
    m_current_offset.y = y;
}


void Graph::snap_entities_to_grid(const RoadGrid& road_grid) const
{
    /*
    The steps are the following:
        1) Translate the x,y coords of the location of the EntityCircle to a grid reference
        2) Iterate over the grid to find the nearest non-0 grid reference
        3) Translate this grid reference back to x,y coords (reverse of step 1)
        4) Move to the coords discovered in step 3)
    */

    sf::Vector2f pos{};
    sf::Vector2f grid_ref{};

    for (EntityCircle* entity : m_entities)
    {
        pos = entity->get_shape().getPosition();
        grid_ref = translate_coords_to_grid_ref(pos);
        grid_ref = road_grid.find_nearest_non_zero_ref(grid_ref);
        pos = translate_grid_ref_to_coords(grid_ref);
        entity->move_to_destination(pos);
    }
}
