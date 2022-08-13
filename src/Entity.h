/*
This creates the Entity object, the core base class for all nodes in the network.
It contains information regarding things every entity must have:
	m_age (uint8_t): This is expressed in "iterations" of the programme.
	m_gender (uint8_t): This is expressed as an int, considering change to bool for size.
	m_id (int): The id integer of the Entity, all Entities must be unique, this is used to identify the Entity's relationships (parents, children, links)
	m_links (std::vector<int>): A variable length vector of ids representing the Entities this Entity is linked to.
	m_children (std::vector<int>): A variable length vector of ids representing the children produced by this Entity.
	m_parents (int* array[2]): A fixed length array of 2 integers, representing the ids of this Entity's parents.
	m_beliefs (int* array[2][2]): A 2x2 fixed size 2d array representing the beliefs of the Entity according to this structure:
		beliefs:
		 - individualism-heirarchy
		 - godless-religious
		 - structure-freedom
		 - communal-independent
*/

#ifndef STROG_ENTITY_H
#define STROG_ENTITY_H

#include "utils.h"

#include<vector>

enum Genders
{
	male,
	female,
	other,

    max_genders,
};


class Entity
{
private:
	std::uint_fast8_t m_age; // is this slower than int?
	uint8_t m_gender; // make this bool?
	id_t m_id;
	std::vector<Entity*> m_children;
	Entity* m_parents[2]; // ref wrapper here too??
	float* m_beliefs[2][2];
	// uint generation{} // is this worth having?

protected:
	std::vector<Entity*> m_links;

public:
	Entity();
	std::uint_fast8_t get_age() const { return m_age; }
	uint8_t get_gender() const { return m_gender; }
	id_t get_id() const { return m_id; }
	void add_link(Entity* entity);
	void print_links() const;
	void propagate_beliefs();
};



#endif // !STROG_ENTITY_H
