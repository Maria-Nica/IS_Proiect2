#pragma once
#pragma once
#include <string>
#include <memory>
#include "Position.h"
#include "Orientation.h"
#include "IBoard.h"

class IPlayer {
public:
    virtual ~IPlayer() = default;

    virtual const std::string& getName() const = 0;

    virtual bool placeShip(const Ship& ship) = 0;
    virtual bool receiveShot(const Position& position) = 0;
    virtual bool allShipsPlaced(int maxShips) const = 0;
    virtual bool hasRemainingShips() const = 0;

    virtual void reset() = 0;

    virtual std::shared_ptr<IBoard> getBoard() const = 0;
};
