#include "pch.h"
#include <gtest/gtest.h>
#include "Cell.h"
#include "CellState.h"

TEST(CellTests, CellStoresPositionAndState)
{
    Cell c{Position(2, 4), CellState::Ship};
    EXPECT_EQ(c.position.m_x, 2);
    EXPECT_EQ(c.position.m_y, 4);
    EXPECT_EQ(c.state, CellState::Ship);
}

// Merged from CellStateTests.cpp
TEST(CellStateTests, EnumValuesPresent)
{
    // Basic smoke test: ensure enum compiles and values accessible
    EXPECT_EQ(static_cast<int>(CellState::Empty) <= static_cast<int>(CellState::Miss), true);
}
