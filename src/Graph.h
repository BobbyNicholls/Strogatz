#ifndef STROG_GRAPH_H
#define STROG_GRAPH_H

#include <set>
#include<vector>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EntityCircle.h"
#include "utils.h"


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

#endif
