#pragma once
#include <string>
#include <vector>
#include <memory>
#include "IPlayer.h"
#include "IBoard.h"
#include "Ship.h"

class Player : public IPlayer {
private:
    std::string m_name;
    std::shared_ptr<IBoard> m_board;

public:
    Player(const std::string& name, std::shared_ptr<IBoard> board);

    const std::string& getName() const override;
    bool placeShip(const Ship& ship) override;
    bool receiveShot(const Position& position) override;
    bool allShipsPlaced(int maxShips) const override;
    bool hasRemainingShips() const override;
    void reset() override;

    std::shared_ptr<IBoard> getBoard() const override;
};
