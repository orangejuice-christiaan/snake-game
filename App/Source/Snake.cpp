#include "Snake.h"

Snake::Snake()
	: m_Transform(),
	m_Radius(1.0f),
	m_Color(LIME),
	m_Segments(),
	m_DesiredSegmentCount(100)
{
}

Snake::~Snake()
{
}

