#include "Game.h"
#include <algorithm>
#include <iostream>
#include "Orientation.h"
#include "Board.h"
#include "Player.h"

Game::Game(std::shared_ptr<IPlayer> player1, std::shared_ptr<IPlayer> player2, int maxShips)
    : m_player1(player1), m_player2(player2), m_currentPlayer(player1),
    m_state(GameState::PlacingShips), m_maxShips(maxShips)
{
}

void Game::addListener(std::shared_ptr<IGameListener> listener) {
    if (!listener) return;

    auto iterator = std::find_if(m_listeners.begin(), m_listeners.end(),
        [&](const std::weak_ptr<IGameListener>& weakL) {
            auto currentListener = weakL.lock();
            return currentListener && currentListener == listener;
        });

    if (iterator == m_listeners.end()) m_listeners.push_back(listener);
}

void Game::removeListener(std::shared_ptr<IGameListener> listener) {
    m_listeners.erase(std::remove_if(m_listeners.begin(), m_listeners.end(),
        [&](const std::weak_ptr<IGameListener>& weakL) {
            auto currentListener = weakL.lock();
            return !currentListener || currentListener == listener;
        }), m_listeners.end());
}

void Game::notifyShipPlaced(const Ship& ship) {
    for (auto iterator = m_listeners.begin(); iterator != m_listeners.end();) {
        if (auto listener = iterator->lock()) {
            listener->onShipPlaced(ship);
            ++iterator;
        }
        else {
            iterator = m_listeners.erase(iterator);
        }
    }
}

void Game::notifyShotFired(const Cell& cell) {
    for (auto iterator = m_listeners.begin(); iterator != m_listeners.end();) {
        if (auto listener = iterator->lock()) {
            listener->onShotFired(cell, m_state);
            ++iterator;
        }
        else {
            iterator = m_listeners.erase(iterator);
        }
    }
}

void Game::notifyGameStateChanged(GameState newState) {
    for (auto iterator = m_listeners.begin(); iterator != m_listeners.end();) {
        if (auto listener = iterator->lock()) {
            listener->onGameStateChanged(newState);
            ++iterator;
        }
        else {
            iterator = m_listeners.erase(iterator);
        }
    }
}


void Game::changeState(GameState newState) {
	m_state = newState;
    notifyGameStateChanged(newState);
}

void Game::startGame() {
    m_currentPlayer = m_player1;
    changeState(GameState::PlacingShips);

    if (m_player1 && m_player1->getBoard()) {
        m_player1->getBoard()->resetBoard();
    }

    if (m_player2 && m_player2->getBoard()) {
        m_player2->getBoard()->resetBoard();
    }
}

bool Game::placeShip(const Position& start, int length, Orientation orientation) {
    if (auto currentPlayerShared = m_currentPlayer.lock()) {
        Ship plane(start, orientation);

        if (currentPlayerShared->placeShip(plane)) {
            notifyShipPlaced(plane);

            return true;
        }
    }

    return false;
}

void Game::shoot(const Position& position) {
    if (isGameOver()) return;

    auto currentShared = m_currentPlayer.lock();
    if (!currentShared) return;

    auto opponent = (currentShared == m_player1) ? m_player2 : m_player1;
    if (!opponent) return;

    auto board = opponent->getBoard();
    if (!board) return;

    bool hit = board->receiveShot(position);

    Cell cell{ position, hit ? CellState::Hit : CellState::Miss };
    notifyShotFired(cell);

    if (board->allShipsSunk()) {
        changeState(GameState::GameOver);
    }
    else {
        changeState(GameState::SwitchingTurn);
        switchTurn();
    }
}


void Game::switchTurn() {
    auto currentShared = m_currentPlayer.lock();
    if (!currentShared) return;

    m_currentPlayer = (currentShared == m_player1) ? m_player2 : m_player1;

    if (m_state == GameState::PlacingShips) {
        if (m_player1 && m_player2
            && m_player1->allShipsPlaced(m_maxShips)
            && m_player2->allShipsPlaced(m_maxShips)) {
            changeState(GameState::InProgress);
        }
    }
}

bool Game::isGameOver() const {
    for (auto player : { m_player1, m_player2 }) {
        if (!player) continue;

        auto board = player->getBoard();
        if (board && board->allShipsSunk()) {
            return true;
        }
    }
    return false;
}

GameState Game::getState() const {
    return m_state;
}

int Game::getMaxShips() const {
    return m_maxShips;
}

std::shared_ptr<IPlayer> Game::getPlayer1() const
{
    return m_player1;
}

std::shared_ptr<IPlayer> Game::getPlayer2() const
{
    return m_player2;
}

std::weak_ptr<IPlayer> Game::getCurrentPlayer() const
{
    return m_currentPlayer;
}

int Game::getGridSize() const
{
    return m_currentPlayer.lock()->getBoard()->getSize();
}
