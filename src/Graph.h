#ifndef STROG_GRAPH_H
#define STROG_GRAPH_H

#include <set>
#include<vector>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EntityCircle.h"
#include "utils.h"


const int entities_start_size{ 50 };
const int entities_reserve_limit{ 200 };
const int link_limit{ 1000 };


struct Link
{
	EntityCircle* from{ nullptr };
	EntityCircle* to{ nullptr };
};


struct Graph
{
	std::vector<EntityCircle*> entities;
	std::vector<Link*> links;
	float rewire_prob{ 0.15f };
	float new_edge_prob{ 0.15f };
	float spawn_chance{ 0.07f };
};


Graph get_barabasi_albert_graph(time_period_t time_period);
void add_random_edge(Graph& graph, int max_entitiy_iloc);
void draw_entities(Graph& graph, sf::RenderWindow& window);
void draw_links(Graph& graph, sf::RenderWindow& window);
void forward_propagate_beliefs(Graph& graph);
// spawn chance is set to 50% for hypter accelerated development but for a reasonable spawn
// rate over 1000 periods set to 3-6% (should review):
void propagate_entities(Graph& graph, time_period_t time_period);
void rewire_random_edge(Graph& graph);

#endif
