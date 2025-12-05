#pragma once
#include <vector>
#include "IBoard.h"
#include "Ship.h"
#include "Position.h"
#include "CellState.h"
#include "Cell.h"
class Board : public IBoard {
private:
    static constexpr int SIZE = 10;
    Cell cells[SIZE][SIZE];
    std::vector<Ship> m_ships;

public:
    Board();

    bool receiveShot(const Position& position) override;
    void resetBoard() override;
    CellState getCellState(const Position& position) const override;
    int getShipsCount() const;
    int getSize() const;
    const std::vector<Ship>& getShips() const;

    bool placeShip(const Ship& plane) override;

    void print() const override;

    bool allShipsSunk() const override;

    // New
    Cell getCellInfo(const Position& p) const override;
    bool canPlaceShip(const Ship& ship) const override;

private:
    bool placeShipParts(const std::vector<ShipPart>& parts);
    bool isValid(const Position& position) const;

};
