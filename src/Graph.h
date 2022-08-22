#ifndef STROG_GRAPH_H
#define STROG_GRAPH_H

#include <set>
#include<vector>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EntityCircle.h"
#include "utils.h"


const int entities_start_size{ 35 };
const int entities_reserve_limit{ 200 };


struct Graph
{
	std::vector<EntityCircle*> entities;
	id_t links[link_limit][2];
	unsigned int link_counter{ 0 };
};


Graph get_graph(time_period_t time_period);
void draw_entities(Graph& graph, sf::RenderWindow& window);
void draw_links(Graph& graph, sf::RenderWindow& window);
void forward_propagate_beliefs(Graph& graph);
void propagate_entities(Graph& graph, time_period_t time_period, float spawn_chance = 0.5f);

#endif
