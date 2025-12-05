#include "pch.h"
#include <gtest/gtest.h>
#include "Orientation.h"

TEST(OrientationTests, ValuesAreDistinct)
{
    EXPECT_NE(static_cast<int>(Orientation::Up), static_cast<int>(Orientation::Down));
    EXPECT_NE(static_cast<int>(Orientation::Left), static_cast<int>(Orientation::Right));
}
