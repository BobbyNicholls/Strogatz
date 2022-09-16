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
- Clique formation and graph evolution with belief checking.
- Make graph a class.
- Refactor and functionalise main.
- Multiple graphs.
- Graph migration.
- Make it so we dont loop through entities and links multiple times unnecessarily.
- Split rendering and event handling out in to seperate threads: 
	https://www.sfml-dev.org/tutorials/2.5/graphics-draw.php#drawing-from-threads
- Data representation of networks.
- Meaningful visual entity representation.
- Entity AI.
- Environments.
- Economy.
- Conflict.
- Seed / parameters yaml.
- Play Frostpunk for inspiration??
- Add a player.
- ...etc
