#pragma once
#include "IGameListener.h"
#include "GameState.h"
#include "Position.h"
#include "Orientation.h"
#include "IPlayer.h"
#include <string>
#include <memory>

class IGame {
public:
    virtual ~IGame() = default;

    virtual void addListener(std::shared_ptr<IGameListener> listener) = 0;
    virtual void removeListener(std::shared_ptr<IGameListener> listener) = 0;
    virtual void notifyShipPlaced(const Ship& ship) = 0;
    virtual void notifyShotFired(const Cell& cell) = 0;
    virtual void notifyGameStateChanged(GameState newState) = 0;

    virtual void startGame() = 0;
    virtual bool placeShip(const Position& start, int length = 1, Orientation orientation = Orientation::Up) = 0;
    virtual void shoot(const Position& position) = 0;
    virtual void switchTurn() = 0;
    virtual bool isGameOver() const = 0;

    virtual GameState getState() const = 0;
    virtual int getMaxShips() const = 0;
    virtual std::shared_ptr<IPlayer> getPlayer1() const = 0;
    virtual std::shared_ptr<IPlayer> getPlayer2() const = 0;
    virtual std::weak_ptr<IPlayer> getCurrentPlayer() const = 0;
    virtual int getGridSize() const = 0;
};