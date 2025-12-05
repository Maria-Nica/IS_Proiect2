#pragma once

#include "Position.h"
#include "CellState.h"
struct Cell {
    Position position;
    CellState state;
    bool isHead{false};
};