#ifndef STROG_GRAPH_H
#define STROG_GRAPH_H

#include <set>
#include<vector>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EntityCircle.h"
#include "utils.h"


const int entities_start_size{ 10 };
const int entities_reserve_limit{ 100 };


struct Graph
{
	std::vector<EntityCircle*> entities;
	id_t links[link_limit][2];
	unsigned int link_counter{ 0 };
};


Graph get_graph();
void draw_entities(Graph& graph, sf::RenderWindow& window);
void draw_links(Graph& graph, sf::RenderWindow& window);
void forward_propagate_beliefs(Graph& graph);
void propagate_entities(Graph& graph, float spawn_chance = 0.5f);

#endif
