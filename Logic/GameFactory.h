#pragma once
#include "IGameFactory.h"
#include <memory>
#include <string>

class GameFactory : public IGameFactory
{
public:
    GameFactory(const std::string& player1Name = "Player1",
        const std::string& player2Name = "Player2");

    std::unique_ptr<IGame> create() override;

private:
    std::string m_player1Name;
    std::string m_player2Name;
};

