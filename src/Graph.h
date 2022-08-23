#ifndef STROG_GRAPH_H
#define STROG_GRAPH_H

#include <set>
#include<vector>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EntityCircle.h"
#include "utils.h"


const int entities_start_size{ 40 };
const int entities_reserve_limit{ 200 };


struct Graph
{
	std::vector<EntityCircle*> entities;
	id_t links[link_limit][2];
	unsigned int link_counter{ 0 };
};


Graph get_barabasi_albert_graph(
	time_period_t time_period, float new_edge_prob = 0.1f, float rewire_prob = 0.2f
);
Graph get_graph(time_period_t time_period);
void draw_entities(Graph& graph, sf::RenderWindow& window);
void draw_links(Graph& graph, sf::RenderWindow& window);
void forward_propagate_beliefs(Graph& graph);
// spawn chance is set to 50% for hypter accelerated development but for a reasonable spawn
// rate over 1000 periods set to 3-6% (should review):
void propagate_entities(Graph& graph, time_period_t time_period, float spawn_chance = 0.03f);

#endif
