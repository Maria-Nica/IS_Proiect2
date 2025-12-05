#pragma once

#include "Position.h"
#include "Orientation.h"

namespace LogicUtils
{
	inline Orientation computeOrientation(const Position& head, const Position& second)
	{
		int dx = second.m_x - head.m_x;
		int dy = second.m_y - head.m_y;
		if (dx == -2 && dy == 1)
			return Orientation::Up;
		if (dx == -2 && dy == -1)
			return Orientation::Down;
		if (dx == 1 && dy == 2)
			return Orientation::Left;
		if (dx == -1 && dy == -2)
			return Orientation::Right;
		return Orientation::Up;
	}
}