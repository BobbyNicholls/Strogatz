## Puspose of this repo:
- For me to learn C++.
- To create compute and memory efficient graph theoretical simulators of social networks.
- To be able to perform Graph ML over arbitrary graph structures, treating the social networks as if they were probabilistic models, or Bayesian Networks, thereby enabling beleif propagation and Bayesian Inference and other graph operations over the network.
- To visualise dynamic networks somehow.

## Strogatz consists of:
- Distributions.h: for drawing vlaues from probability distributions
- Distributions.cpp: for drawing vlaues from probability distributions
- Entity.h: for the Entity base class from which units / nodes are generated.
- Entity.cpp: for the Entity base class from which units / nodes are generated.
- EntityCircle.h: for the EntityCircle derived class, represents Entities as circles.
- EntityCircle.cpp: for the EntityCircle derived class, represents Entities as circles.
- Text.h: text display functions
- Text.cpp: text display functions
- utils.h: for general utilities
- utils.cpp: for general utilities
- main.cpp

# To do:
- Refactor and functionalise main.
- Map related ativities on the individual graph level:
  - Put anchor points on the map where entities return to and congregate.
  - Explorers and returners??
  - Have tiles blend in to each other more naturally.
  - Generate the map according to the anchors.
  - Housing, places of work, etc.
- Add routing so entities travel along sensible paths
- Add factions
- Environments: Have maps exist inside a wider world
- Multiple graphs.
- Graph migration.
- Make it so we dont loop through entities and links multiple times unnecessarily.
- Split rendering and event handling out in to seperate threads: 
	https://www.sfml-dev.org/tutorials/2.5/graphics-draw.php#drawing-from-threads
- Multi-thread processes in general.
- Data representation of networks.
- Character models: Meaningful visual entity representation.
- Entity AI.
- Economy.
- Conflict.
- Seed / parameters yaml.
- Play Frostpunk for inspiration??
- Play Rimworld for inspiration??
- Add a player.
- ...etc


Issues:
Currently positioning of entities in grid is set via the `game_width` and `game_height` in main.cpp and
not by the dimension of the graph which is weird.
This means that `EntityCirclce.set_position_randomly()` should have min and max values for both width and
height that corresponds to the limits of the graph, which should then translate to the map.
Current we have a situation where the default parameter values of the graph constructor determine the 
parameters of the graph, which is fine, but global variables determine the parameter of the map and the 
map's position on the world map is determined by these global variables essentially, really this should be
determined in the graph then passed to the map.