#ifndef STROG_STRUCTURE_H
#define STROG_STRUCTURE_H

#include <SFML/Graphics.hpp>

// should this really be its own class? I think so.
// Each entity should have a "home" structure, this will be a pointer to an instance of this class.
// Each map will contain a list of these structures, which is how they will be instantiated, as part of a
// map.

class Structure
{
public:

	enum Index
	{
		home,
		work,
		social,

		max_structures,
	};

private:
	sf::Vector2f m_location{};
	Structure::Index m_index;

public:
	Structure() = delete;
	Structure(float x, float y, Structure::Index index);

};

#endif
