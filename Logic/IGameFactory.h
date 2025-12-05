#pragma once
#include <memory>
#include "IGame.h"

class IGameFactory {
public:
    virtual ~IGameFactory() = default;

    virtual std::unique_ptr<IGame> create() = 0;
};