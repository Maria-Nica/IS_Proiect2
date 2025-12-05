#include "Player.h"
#include "Board.h"  

Player::Player(const std::string& name, std::shared_ptr<IBoard> board)
    : m_name(name), m_board(board) {
}

const std::string& Player::getName() const {
    return m_name;
}

bool Player::placeShip(const Ship& ship)
{
    return m_board->placeShip(ship);
}


bool Player::receiveShot(const Position& p)
{
    return m_board->receiveShot(p);
}

bool Player::allShipsPlaced(int maxShips) const {
    if (!m_board) return false;
    return m_board->getShipsCount() == maxShips;
}

bool Player::hasRemainingShips() const
{
    return m_board->allShipsSunk() == false;
}

void Player::reset()
{
    if (m_board)
        m_board->resetBoard();
}

std::shared_ptr<IBoard> Player::getBoard() const {
	return m_board;
}