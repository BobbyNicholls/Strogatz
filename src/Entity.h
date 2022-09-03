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

#include "Distributions.h"
#include "utils.h"

#include<vector>


class Entity
{
private:
	time_period_t m_birth_time{ 0 }; // is this slower than int?
	bool m_sex;
	bool m_paired { false };
	bool m_leader { false };
	id_t m_id;

	Entity* m_partner{ nullptr };
	std::vector<Entity*> m_children; // this does nothing and can contain dangling pointers
	Entity* m_parents[2]; // this does nothing and can contain dangling pointers
	// uint generation{} // is this worth having?

protected:
	std::vector<Entity*> m_links;
	float m_beliefs[2][2];

public:
	Entity(time_period_t birth_time);

	time_period_t get_birth_time() const { return m_birth_time; }
	id_t get_id() const { return m_id; }
	std::vector<Entity*>& get_links() { return m_links; } // this func should be const?
	Entity* get_partner() const { return m_partner; };
	uint8_t get_sex() const { return m_sex; }
	bool is_paired() const { return m_paired; };

	void add_child(Entity* entity);
	void add_link(Entity* entity);
	void remove_link(Entity* entity, bool remove_from_both=true);
	void add_parents(Entity* entity1, Entity* entity2);
	void add_partner(Entity* entity);
	bool is_linked_to(Entity* entity) const;
	void print_beliefs() const;
	void print_links() const;
	void update_beliefs(Entity* influencer);
	void normalise_beliefs();
	void do_random_walks(std::map<int, int>& map_to_fill, int steps=10, int walks=50);
	float get_abs_belief_diff(Entity* entity);
};


#endif // !STROG_ENTITY_H
