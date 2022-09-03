#ifndef STROG_GRAPH_H
#define STROG_GRAPH_H

#include <set>
#include<vector>

#include <SFML/Window.hpp>

#include "EntityCircle.h"


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
private:
	std::vector<EntityCircle*> m_entities;
	std::vector<Link*> m_links; // this is leaking memory a lot
	int m_entities_start_size;
	float m_rewire_prob;
	float m_new_edge_prob;
	float m_spawn_chance;
	int m_clique_min_size;
	int m_clique_max_size;
	std::vector<EntityVector> m_entity_vectors;

public:
	Graph(
		const time_period_t start_time,
		const float rewire_prob = 0.02f,
		const float new_edge_prob = 0.01f,
		const float spawn_chance = 0.12f,
		const int entities_start_size = 60,
		const int entities_reserve_limit = 400,
		const int link_limit = 3000,
		const int clique_min_size = 4,
		const int clique_max_size = 10
	);

	float get_rewire_prob() const { return m_rewire_prob; }
	float get_new_edge_prob() const { return m_new_edge_prob; }
	float get_spawn_chance() const { return m_spawn_chance; }
	int get_nr_of_entities() const { return static_cast<int>(m_entities.size()); }

	void link_entities(EntityCircle* entity_from, EntityCircle* entity_to);
	EntityCircle* get_preferential_entity();
	EntityCircle* add_preferential_links(EntityCircle* entity);
	void add_random_edge(const int max_entitiy_iloc);
	void rewire_random_edge();
	void draw_entities(sf::RenderWindow& window, const float move_distance);
	void draw_links(sf::RenderWindow& window);
	void forward_propagate_beliefs();
	void propagate_entities(const time_period_t time_period);
	float death_sigmoid(const int age, const int fifty_pct_age = 85, const int hundred_pct_age = 110);
	void tidy_up_entities();
	void kill_entities(const time_period_t time_period);
	void seed_cliques_and_leaders(const int leaders = 4, const int cliques = 8);
	void make_leader(EntityCircle* seed);
	void form_clique_from_seed(int seed);
	void vectorise_nodes(bool vectorise_all_nodes = true);
};

#endif
