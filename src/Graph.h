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


struct Graph
{
	std::vector<EntityCircle*> entities;
	std::vector<Link*> links;
	float rewire_prob{ 0.1f };
	float new_edge_prob{ 0.25f };
	float spawn_chance{ 0.12f };
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
