#include "pch.h"
#include <gtest/gtest.h>
#include "Position.h"

TEST(PositionTests, DefaultConstructor)
{
    Position p;
    EXPECT_EQ(p.m_x, 0);
    EXPECT_EQ(p.m_y, 0);
}

TEST(PositionTests, ParamConstructor)
{
    Position p(3, 7);
    EXPECT_EQ(p.m_x, 3);
    EXPECT_EQ(p.m_y, 7);
}
