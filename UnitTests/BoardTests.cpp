#include "pch.h"
#include <gtest/gtest.h>
#include "Board.h"
#include "Ship.h"
#include "Position.h"
#include "CellState.h"
#include "Orientation.h"

TEST(BoardTests, DefaultBoardEmpty)
{
    Board b;
    EXPECT_EQ(b.getSize(), 10);
    EXPECT_EQ(b.getShipsCount(), 0);
    EXPECT_EQ(b.getCellState(Position(0, 0)), CellState::Empty);
}

TEST(BoardTests, PlaceShipAndCellState)
{
    Board b;
    Ship s(Position(3, 3), Orientation::Up);
    bool placed = b.placeShip(s);
    EXPECT_TRUE(placed);
    EXPECT_EQ(b.getShipsCount(), 1);

    // Check that parts are marked as Ship
    for (const auto &part : s.getParts())
    {
        const Position pos = part.getPosition();
        EXPECT_EQ(b.getCellState(pos), CellState::Ship);
    }
}

TEST(BoardTests, PreventOverlap)
{
    Board b;
    Ship s1(Position(3, 3), Orientation::Up);
    Ship s2(Position(3, 3), Orientation::Up);
    EXPECT_TRUE(b.placeShip(s1));
    EXPECT_FALSE(b.placeShip(s2));
    EXPECT_EQ(b.getShipsCount(), 1);
}

TEST(BoardTests, ReceiveShotHitAndMiss)
{
    Board b;
    Ship s(Position(4, 4), Orientation::Up);
    b.placeShip(s);

    // shoot a ship part
    Position shipPos = s.getParts().front().getPosition();
    bool hit = b.receiveShot(shipPos);
    EXPECT_TRUE(hit);
    EXPECT_EQ(b.getCellState(shipPos), CellState::Hit);

    // shoot an empty cell
    Position emptyPos(0, 0);
    if (b.getCellState(emptyPos) == CellState::Empty)
    {
        bool result = b.receiveShot(emptyPos);
        EXPECT_FALSE(result);
        EXPECT_EQ(b.getCellState(emptyPos), CellState::Miss);
    }
}

TEST(BoardTests, AllShipsSunk)
{
    Board b;
    Ship s(Position(1, 1), Orientation::Up);
    b.placeShip(s);

    // hit every part
    for (const auto &part : s.getParts())
        b.receiveShot(part.getPosition());

    EXPECT_TRUE(b.allShipsSunk());
}

// Merged from BoardInvalidTests.cpp
TEST(BoardInvalidTests, PlaceShipNegativeStartFails)
{
    Board b;
    Ship s(Position(-1, 0), Orientation::Up);
    EXPECT_FALSE(b.placeShip(s));
    EXPECT_EQ(b.getShipsCount(), 0);
}

TEST(BoardInvalidTests, PlaceShipOverflowFails)
{
    Board b;
    // start near bottom-right to cause overflow with base offsets
    Ship s(Position(9, 9), Orientation::Up);
    EXPECT_FALSE(b.placeShip(s));
    EXPECT_EQ(b.getShipsCount(), 0);
}

// Merged from BoardShotTests.cpp
TEST(BoardShotTests, ReceiveShotInvalidPositionsReturnFalse)
{
    Board b;
    EXPECT_FALSE(b.receiveShot(Position(-1, 0)));
    EXPECT_FALSE(b.receiveShot(Position(10, 10)));
}

TEST(BoardShotTests, RepeatedShotDoesNotChangeState)
{
    Board b;
    Ship s(Position(5, 5), Orientation::Up);
    ASSERT_TRUE(b.placeShip(s));

    Position target = s.getParts().front().getPosition();
    bool first = b.receiveShot(target);
    EXPECT_TRUE(first);
    EXPECT_EQ(b.getCellState(target), CellState::Hit);

    // second shot should not change the state and should return false (already hit)
    bool second = b.receiveShot(target);
    EXPECT_FALSE(second);
    EXPECT_EQ(b.getCellState(target), CellState::Hit);
}
