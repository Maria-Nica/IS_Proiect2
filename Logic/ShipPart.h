#pragma once
#include "Position.h"

class ShipPart {
private:
    Position m_position;
    bool m_hit;
    bool m_isHead;

public:
    ShipPart(Position p, bool head = false);


	void markHit();
    bool isHit() const;
    bool isHeadPart() const;
    Position getPosition() const;
};
