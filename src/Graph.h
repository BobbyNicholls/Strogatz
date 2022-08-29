#ifndef STROG_GRAPH_H
#define STROG_GRAPH_H

#include <set>
#include<vector>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EntityCircle.h"
#include "utils.h"


const int entities_start_size{ 40 };
const int entities_reserve_limit{ 400 };
const int link_limit{ 3000 };


struct Link
{
	EntityCircle* from{ nullptr };
	EntityCircle* to{ nullptr };
};


//eventually make this a template class?: learncpp.com/cpp-tutorial/template-classes/
class Graph
{
private:
	std::vector<EntityCircle*> m_entities;
	std::vector<Link*> m_links;
	int m_entities_start_size;

public:
	Graph(
		const time_period_t start_time,
		const float rewire_prob = 0.1f,
		const float new_edge_prob = 0.25f,
		const float spawn_chance = 0.12f,
		const int entities_start_size = 0.12f
	);

	float m_rewire_prob;
	float m_new_edge_prob;
	float m_spawn_chance;

	void link_entities(EntityCircle* entity_from, EntityCircle* entity_to);
	EntityCircle* get_preferential_entity();
	EntityCircle* add_preferential_links(EntityCircle* entity);
	void add_random_edge(const int max_entitiy_iloc);
	void rewire_random_edge();
	void draw_entities(sf::RenderWindow& window);
	void draw_links(sf::RenderWindow& window);
	void forward_propagate_beliefs();
	void propagate_entities(const time_period_t time_period);
};

#endif
