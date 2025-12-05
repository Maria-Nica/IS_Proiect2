#pragma once
#include <vector>
#include "Position.h"
#include "CellState.h"
#include "Ship.h"
#include "Cell.h"

class IBoard {
public:
    virtual ~IBoard() = default;

    virtual bool placeShip(const Ship& ship) = 0;

    virtual bool receiveShot(const Position& p) = 0;
    virtual void resetBoard() = 0;
    virtual CellState getCellState(const Position& p) const = 0;
    virtual int getShipsCount() const = 0;
    virtual int getSize() const = 0;
    virtual const std::vector<Ship>& getShips() const = 0;

    virtual void print() const = 0;

    virtual bool allShipsSunk() const = 0;

    virtual Cell getCellInfo(const Position& p) const = 0;

    virtual bool canPlaceShip(const Ship& ship) const = 0;
};
