#ifndef STROG_GRAPH_H
#define STROG_GRAPH_H

#include <set>
#include<vector>

#include "EntityCircle.h"
#include "utils.h"


struct Graph
{
	std::vector<EntityCircle*> entities;
	id_t links[link_limit][2];
	unsigned int link_counter{ 0 };
};


Graph get_graph();

#endif