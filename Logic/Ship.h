#pragma once
#include <vector>
#include "ShipPart.h"
#include "Position.h"
#include "Orientation.h"

class Ship {
private:
    std::vector<ShipPart> m_parts;
    static Position rotate(const Position& position, Orientation orientation);

public:
    Ship(const Position& start, Orientation orientation);

    bool contains(const Position& position) const;
    void hit(const Position& position);
    bool isSunk() const;

    const std::vector<ShipPart>& getParts() const;
};
