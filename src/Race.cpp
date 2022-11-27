#include "Race.h"

Race::Race(Race::Index index)
	: m_index{ index }, m_name{ index_to_name_map[index] }
{}


Races::Races(int nr_of_races)
{
	std::vector<int> race_vector(Race::Index::max_races);
	for (int i{ 0 }; i < Race::Index::max_races; ++i) race_vector[i] = i;
	shuffle_vector(race_vector);
	race_vector.resize(nr_of_races);

	m_races.reserve(nr_of_races);
	for (int i{ 0 }; i < nr_of_races; ++i)
	{
		m_races.push_back(new Race{ static_cast<Race::Index>(race_vector[i]) });
	}
}


std::map<Race::Index, std::string> Race::index_to_name_map{
		{Race::Index::elf, "Elf"},
		{Race::Index::human, "Human"},
		{Race::Index::goblin, "Goblin"},
		{Race::Index::gnome, "Gnome"},
		{Race::Index::troll , "Troll"},
		{Race::Index::beastmen, "Beastmen"},
		{Race::Index::zombie, "Zombie"},
		{Race::Index::dwarf, "Dwarf"},
};
