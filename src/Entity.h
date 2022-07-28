#pragma once

class Entity;
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
	int m_age{};
	int m_gender;
	int m_id;

public:
	Entity() = default;
	int get_age() const { return m_age; }
	int get_gender() const { return m_gender; }
	void propagate_beliefs();
};
