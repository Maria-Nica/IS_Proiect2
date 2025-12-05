#include "pch.h"
#include <gtest/gtest.h>
#include "Ship.h"
#include "ShipPart.h"
#include "Position.h"
#include "Orientation.h"

TEST(ShipTests, PartsCountAndHead)
{
    Ship s(Position(5, 5), Orientation::Up);
    const auto &parts = s.getParts();
    // Ship implementation uses a fixed baseOffsets of 10 parts
    EXPECT_EQ(parts.size(), 10);
    // first part should be head
    EXPECT_TRUE(parts.front().isHeadPart());
}

TEST(ShipTests, ContainsAndHitNonHead)
{
    Ship s(Position(5, 5), Orientation::Up);
    const auto &parts = s.getParts();
    auto pos = parts[1].getPosition();

    EXPECT_TRUE(s.contains(pos));
    s.hit(pos);
    // only that part should be hit (unless it was head)
    int hitCount = 0;
    for (const auto &p : s.getParts())
        if (p.isHit())
            ++hitCount;

    EXPECT_EQ(hitCount, parts[1].isHeadPart() ? parts.size() : 1);
}

TEST(ShipTests, HitHeadSinksAll)
{
    Ship s(Position(2, 2), Orientation::Right);
    const auto &parts = s.getParts();
    auto headPos = parts.front().getPosition();

    EXPECT_FALSE(s.isSunk());
    s.hit(headPos);
    EXPECT_TRUE(s.isSunk());
}

// Rotation-related checks moved here so ship-specific tests live in one file.
TEST(ShipTests, RotationRelations)
{
    Position start(5, 5);

    Ship up(start, Orientation::Up);
    Ship down(start, Orientation::Down);
    Ship left(start, Orientation::Left);
    Ship right(start, Orientation::Right);

    const auto &partsUp = up.getParts();
    const auto &partsDown = down.getParts();
    const auto &partsLeft = left.getParts();
    const auto &partsRight = right.getParts();

    ASSERT_EQ(partsUp.size(), partsDown.size());
    ASSERT_EQ(partsUp.size(), partsLeft.size());
    ASSERT_EQ(partsUp.size(), partsRight.size());

    for (size_t i = 0; i < partsUp.size(); ++i)
    {
        const Position pu = partsUp[i].getPosition();
        const Position pd = partsDown[i].getPosition();
        const Position pl = partsLeft[i].getPosition();
        const Position pr = partsRight[i].getPosition();

        // Down: x same as Up, y mirrored around start.y
        EXPECT_EQ(pd.m_x, pu.m_x);
        EXPECT_EQ(pd.m_y, 2 * start.m_y - pu.m_y);

        int bx = pu.m_x - start.m_x;
        int by = pu.m_y - start.m_y;
        EXPECT_EQ(pl.m_x, start.m_x + by);
        EXPECT_EQ(pl.m_y, start.m_y - bx);

        EXPECT_EQ(pr.m_x, start.m_x - by);
        EXPECT_EQ(pr.m_y, start.m_y + bx);
    }
}

TEST(ShipPartTests, HeadAndHitBehavior)
{
    Position p(1, 2);
    ShipPart head(p, true);
    ShipPart part(p, false);

    EXPECT_TRUE(head.isHeadPart());
    EXPECT_FALSE(part.isHeadPart());

    EXPECT_FALSE(head.isHit());
    head.markHit();
    EXPECT_TRUE(head.isHit());
}
