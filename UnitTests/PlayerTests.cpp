#include "pch.h"
#include <gtest/gtest.h>
#include "Player.h"
#include "Board.h"
#include "Ship.h"
#include "Position.h"
#include "IBoard.h"
#include "Orientation.h"
#include "CellState.h"

TEST(PlayerTests, PlaceShipThroughPlayer)
{
    auto board = std::make_shared<Board>();
    Player p("Alice", board);

    Ship s(Position(2, 2), Orientation::Left);
    EXPECT_TRUE(p.placeShip(s));
    EXPECT_EQ(p.getBoard()->getShipsCount(), 1);
}

TEST(PlayerTests, ReceiveShotDelegatesToBoard)
{
    auto board = std::make_shared<Board>();
    Player p("Bob", board);

    Ship s(Position(3, 3), Orientation::Up);
    p.placeShip(s);

    Position target = s.getParts().front().getPosition();
    bool result = p.receiveShot(target);
    EXPECT_TRUE(result);
    EXPECT_EQ(p.getBoard()->getCellState(target), CellState::Hit);
}

TEST(PlayerTests, ResetClearsBoard)
{
    auto board = std::make_shared<Board>();
    Player p("Eve", board);
    Ship s(Position(5, 5), Orientation::Up);
    p.placeShip(s);
    EXPECT_EQ(p.getBoard()->getShipsCount(), 1);

    p.reset();
    EXPECT_EQ(p.getBoard()->getShipsCount(), 0);
}

// Merged from PlayerEdgeTests.cpp
TEST(PlayerEdgeTests, NullBoardAllShipsPlacedAndReset)
{
    std::shared_ptr<IBoard> nullBoard = nullptr;
    Player p("NoBoard", nullBoard);

    // allShipsPlaced should return false when board is null
    EXPECT_FALSE(p.allShipsPlaced(1));

    // reset should be safe and not crash when board is null
    p.reset();
}

TEST(PlayerEdgeTests, AllShipsPlacedWithBoard)
{
    auto board = std::make_shared<Board>();
    Player p("HasBoard", board);

    EXPECT_FALSE(p.allShipsPlaced(1));
    Ship s(Position(5, 5), Orientation::Up);
    EXPECT_TRUE(p.placeShip(s));
    EXPECT_TRUE(p.allShipsPlaced(1) == (board->getShipsCount() == 1));
}
