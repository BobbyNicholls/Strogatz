#include "Structure.h"

Structure::Structure(float x, float y, Structure::Index index)
	: m_index{ index }
{
	m_location.x = x;
	m_location.y = y;
}
