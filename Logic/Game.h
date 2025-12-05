#pragma once
#include "IGame.h"
#include "IPlayer.h"
#include "IBoard.h"
#include "IGameListener.h"
#include "Orientation.h"
#include <vector>
#include <memory>

class Game : public IGame {
public:
    Game(std::shared_ptr<IPlayer> player1, std::shared_ptr<IPlayer> player2, int maxShips=3);
    ~Game() override = default;

    void addListener(std::shared_ptr<IGameListener> listener) override;
    void removeListener(std::shared_ptr<IGameListener> listener) override;
    void notifyShipPlaced(const Ship& ship) override;
    void notifyShotFired(const Cell& cell) override;
    void notifyGameStateChanged(GameState newState) override;

    void startGame() override;
    bool placeShip(const Position& start, int length = 1, Orientation orientation = Orientation::Up) override;
    void shoot(const Position& position) override;
    void switchTurn() override;
    bool isGameOver() const override;
    GameState getState() const override;
    int getMaxShips() const override;
    std::shared_ptr<IPlayer> getPlayer1() const override;
    std::shared_ptr<IPlayer> getPlayer2() const override;
    std::weak_ptr<IPlayer> getCurrentPlayer() const override;
    int getGridSize() const override;

private:
    std::vector<std::weak_ptr<IGameListener>> m_listeners;
    std::shared_ptr<IPlayer> m_player1;
    std::shared_ptr<IPlayer> m_player2;
    std::weak_ptr<IPlayer> m_currentPlayer;
    GameState m_state;
    int m_maxShips;

    void changeState(GameState newState);
};
