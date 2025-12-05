#pragma once
#include "GameState.h" 
#include "Position.h"
#include "Board.h"

class IGameListener {
public:
    virtual ~IGameListener() = default;

    virtual void onShipPlaced(const Ship& ship) = 0;
    virtual void onShotFired(const Cell& cell, GameState gameState) = 0;
    virtual void onGameStateChanged(GameState newState) = 0;
};