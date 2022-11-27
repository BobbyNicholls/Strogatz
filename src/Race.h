#ifndef STROG_RACE_H
#define STROG_RACE_H

#include <map>
#include<string>

#include "Distributions.h"

class Race
{
public:
	enum Index
	{
		elf,
		human,
		goblin,
		gnome,
		troll,
		beastmen,
		zombie,
		dwarf,

		max_races,
	};
	
	// move out of class? gets instantited for each instance?
	static std::map<Index, std::string> index_to_name_map;

private:
	std::string m_name{};
	Index m_index{};

public:
	Race() = delete;
	Race(Race::Index index);
};


class Races
{
private:
	std::vector<Race*> m_races;

public:
	Races(int nr_of_races);
};

#endif // !STROG_RACE_H
