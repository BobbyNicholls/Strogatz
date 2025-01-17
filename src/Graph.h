#ifndef STROG_GRAPH_H
#define STROG_GRAPH_H

#include <set>
#include <vector>

#include <SFML/Window.hpp>

#include "EntityCircle.h"
#include "Structure.h"


struct Link
{
	EntityCircle* from{ nullptr };
	EntityCircle* to{ nullptr };
};


struct EntityVector
{
	std::vector<int> entity_vector;
};


//eventually make this a template class?: learncpp.com/cpp-tutorial/template-classes/
class Graph
{
	friend class Map;
private:
	std::vector<EntityCircle*> m_entities;
	std::vector<EntityCircle*> m_leaders; // TODO: entities not having m_leader set to true
	std::vector<Link*> m_links; // this is leaking memory a lot
	std::vector<Structure> m_structures;
	float m_rewire_prob;
	float m_new_edge_prob;
	float m_spawn_chance;
	int m_entities_start_size;
	int m_link_limit;
	int m_clique_min_size;
	int m_clique_max_size;
	const float m_min_x; // determines the start location of the map's texture
	const float m_max_x;
	const float m_min_y; // determines the start location of the map's texture
	const float m_max_y;
	std::vector<EntityVector> m_entity_vectors;
	// these are initialised to confusing values so that they will be updated correctly in the Graph constructor
	float m_min_entity_x_pos{ static_cast<float>(game_width) };
	float m_max_entity_x_pos{ static_cast<float>(-game_width) };
	float m_min_entity_y_pos{ static_cast<float>(game_height) };
	float m_max_entity_y_pos{ static_cast<float>(-game_height) };

public:
	std::vector<sf::Vector2f> m_anchor_points;

	Graph(
		const time_period_t start_time,
		const Races* races,
		const float min_x,
		const float max_x,
		const float min_y,
		const float max_y,
		const float rewire_prob = 0.004f,
		const float new_edge_prob = 0.00005f,
		const float spawn_chance = 0.12f,
		const int entities_start_size = 10,
		const int entities_reserve_limit = 400,
		const int link_limit = 3000,
		const int clique_min_size = 3,
		const int clique_max_size = 7
	);

	float get_rewire_prob() const { return m_rewire_prob; }
	float get_new_edge_prob() const { return m_new_edge_prob; }
	float get_spawn_chance() const { return m_spawn_chance; }
	int get_nr_of_entities() const { return static_cast<int>(m_entities.size()); }
	bool is_near_link_limit() const { return m_links.size() > static_cast<int>(0.9*m_link_limit); }
	float get_min_entity_x_pos() const { return m_min_entity_x_pos; };
	float get_max_entity_x_pos() const { return m_max_entity_x_pos; };
	float get_min_entity_y_pos() const { return m_min_entity_y_pos; };
	float get_max_entity_y_pos() const { return m_max_entity_y_pos; };
	float get_min_x() const { return m_min_x; };
	float get_max_x() const { return m_max_x; };
	float get_min_y() const { return m_min_y; };
	float get_max_y() const { return m_max_y; };
	float get_width() const { return m_max_x - m_min_x; };
	float get_height() const { return m_max_y - m_min_y; };

	void link_entities(EntityCircle* entity_from, EntityCircle* entity_to);
	EntityCircle* get_preferential_entity() const;
	EntityCircle* get_belief_compatible_entity(EntityCircle* entity) const;
	EntityCircle* add_preferential_links(EntityCircle* entity);
	void add_random_edge(const int max_entitiy_iloc);
	void rewire_random_edge();
	void draw_entities(
		sf::RenderWindow& window, const float x_move_distance, const float y_move_distance
	);
	void draw_links(sf::RenderWindow& window);
	void forward_propagate_beliefs();
	void propagate_entities(const time_period_t time_period);
	float death_sigmoid(const int age, const int fifty_pct_age = 85, const int hundred_pct_age = 110);
	void tidy_up_entities();
	void kill_entities(const time_period_t time_period);
	void seed_cliques_and_leaders(const int leaders = 3, const int cliques = 4);
	void make_leader(EntityCircle* seed);
	void form_clique_from_seed(const int seed);
	void vectorise_nodes(const bool vectorise_all_nodes = true);
	void reserve_more_links(const float increment_fraction=1.5);
	void check_entities_have_homes();
};

#endif
