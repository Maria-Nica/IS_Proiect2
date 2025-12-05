#include "Board.h"
#include <iostream>

Board::Board() {
    resetBoard();
}

bool Board::placeShip(const Ship& plane)
{
    if (placeShipParts(plane.getParts())) {
        m_ships.push_back(plane);
        return true;
    }
    return false;
}

bool Board::receiveShot(const Position& position)
{
    if (!isValid(position))
        return false;

    Cell& cell = cells[position.m_y][position.m_x];

    if (cell.state == CellState::Ship)
    {
        cell.state = CellState::Hit;

        for (auto& ship : m_ships)
            if (ship.contains(position))
                ship.hit(position);

        return true; 
    }
    else if (cell.state == CellState::Empty)
    {
        cell.state = CellState::Miss;
    }

    return false;
}

int Board::getShipsCount() const {
    return m_ships.size();
}

int Board::getSize() const {
    return SIZE;
}

const std::vector<Ship>& Board::getShips() const
{
    return m_ships;
}

CellState Board::getCellState(const Position& position) const
{
    if (!isValid(position))
        return CellState::Empty;

    return cells[position.m_y][position.m_x].state;
}

void Board::resetBoard()
{
    for (int y =0; y < SIZE; ++y)
        for (int x =0; x < SIZE; ++x)
            cells[y][x] = { {x, y}, CellState::Empty, false };

    m_ships.clear();
}

bool Board::placeShipParts(const std::vector<ShipPart>& parts)
{
    for (auto& part : parts) {
        if (!isValid(part.getPosition()) || cells[part.getPosition().m_y][part.getPosition().m_x].state != CellState::Empty)
            return false;
    }

    for (auto& part : parts) {
        cells[part.getPosition().m_y][part.getPosition().m_x].state = CellState::Ship;
        if (part.isHeadPart()) cells[part.getPosition().m_y][part.getPosition().m_x].isHead = true;
    }

    return true;
}

bool Board::isValid(const Position& position) const
{
    return position.m_x >=0 && position.m_x < SIZE && position.m_y >=0 && position.m_y < SIZE;
}

void Board::print() const
{
    std::cout << "\n ";
    for (int x =0; x < SIZE; ++x)
        std::cout << x << " ";
    std::cout << "\n";

    for (int y =0; y < SIZE; ++y)
    {
        std::cout << (y <10 ? " " : "") << y << " ";
        for (int x =0; x < SIZE; ++x)
        {
            char cell;
            switch (cells[y][x].state)
            {
            case CellState::Empty: cell = '.'; break;
            case CellState::Ship:  cell = 'S'; break;
            case CellState::Hit:   cell = 'X'; break;
            case CellState::Miss:  cell = 'o'; break;
            default: cell = '?'; break;
            }
            std::cout << cell << ' ';
        }
        std::cout << "\n";
    }

    std::cout << "\nLegenda: "
        << "'.' = gol, "
        << "'S' = nava, "
        << "'X' = lovit, "
        << "'o' = ratat\n";
}

bool Board::allShipsSunk() const
{
	for (const auto& ship : m_ships)
	{
		if (!ship.isSunk())
			return false;
	}
	return true;
}

Cell Board::getCellInfo(const Position& p) const {
 if (!isValid(p)) return Cell{ p, CellState::Empty, false };
 return cells[p.m_y][p.m_x];
}

bool Board::canPlaceShip(const Ship& ship) const {
 for (const auto& part : ship.getParts()) {
 if (!isValid(part.getPosition())) return false;
 if (cells[part.getPosition().m_y][part.getPosition().m_x].state != CellState::Empty) return false;
 }
 return true;
}
