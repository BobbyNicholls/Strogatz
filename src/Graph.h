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


class Graph
{
private:
	std::vector<EntityCircle*> m_entities;
	std::vector<Link*> m_links;

public:
	Graph(
		const time_period_t start_time,
		const float rewire_prob = 0.1f,
		const float new_edge_prob = 0.25f,
		const float spawn_chance = 0.12f
	);

	float m_rewire_prob;
	float m_new_edge_prob;
	float m_spawn_chance;
};


Graph get_barabasi_albert_graph(time_period_t time_period);
void add_random_edge(Graph& graph, int max_entitiy_iloc);
void draw_entities(Graph& graph, sf::RenderWindow& window);
void draw_links(Graph& graph, sf::RenderWindow& window);
void forward_propagate_beliefs(Graph& graph);
void propagate_entities(Graph& graph, time_period_t time_period);
void kill_entities(Graph& graph, time_period_t time_period);
void rewire_random_edge(Graph& graph);

#endif
