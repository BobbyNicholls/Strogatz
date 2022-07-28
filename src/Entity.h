#ifndef STROG_ENTITY_H
#define STROG_ENTITY_H

#include "utils.h"

enum Genders
{
	male,
	female,
	other,

    max_genders,
};


class Entity
{
protected:
	uint8_t m_age{};
	int m_gender{};
	int m_id{};

public:
	Entity()
		:m_id{ generate_id() }
	{}
	uint8_t get_age() const { return m_age; }
	int get_gender() const { return m_gender; }
	int get_id() const { return m_id; }
	void propagate_beliefs();
};

#endif // !STROG_ENTITY_H