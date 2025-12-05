#include "Ship.h"

Ship::Ship(const Position& start, Orientation o) {
    std::vector<Position> baseOffsets = {
        {0, 0},                         
        {-2, 1}, {-1, 1}, {0, 1}, {1, 1}, {2, 1}, 
        {0, 2},                        
        {-1, 3}, {0, 3}, {1, 3}         
    };

    for (size_t i = 0; i < baseOffsets.size(); ++i) {
        Position rotated = rotate(baseOffsets[i], o);
        Position finalPos(start.m_x + rotated.m_x, start.m_y + rotated.m_y);
        m_parts.emplace_back(finalPos, i == 0); 
    }
}

Position Ship::rotate(const Position& position, Orientation orientation) {
    switch (orientation) {
    case Orientation::Up:    return position;
    case Orientation::Down:  return { position.m_x, -position.m_y };
    case Orientation::Left:  return { position.m_y, -position.m_x };
    case Orientation::Right: return { -position.m_y, position.m_x };
    }
    return position;
}

bool Ship::contains(const Position& position) const {
    for (const auto& part : m_parts)
        if (part.getPosition().m_x == position.m_x && part.getPosition().m_y == position.m_y)
            return true;
    return false;
}

void Ship::hit(const Position& position) {
    for (auto& part : m_parts) {
        if (part.getPosition().m_x == position.m_x && part.getPosition().m_y == position.m_y) {
            part.markHit();

            if (part.isHeadPart()) {
                for (auto& all : m_parts)
                    all.markHit();
                return;
            }
        }
    }
}

bool Ship::isSunk() const {
    for (const auto& part : m_parts)
        if (!part.isHit())
            return false;
    return true;
}

const std::vector<ShipPart>& Ship::getParts() const {
	return m_parts;
}