#include "GameFactory.h"
#include "Game.h"
#include "Board.h"
#include "Player.h"
#include <vector>
#include <memory>

GameFactory::GameFactory(const std::string& player1Name,
    const std::string& player2Name)
    : m_player1Name(player1Name), m_player2Name(player2Name)
{
}

std::unique_ptr<IGame> GameFactory::create()
{
    auto board1 = std::make_shared<Board>();
    auto board2 = std::make_shared<Board>();

    auto p1 = std::make_shared<Player>(m_player1Name, board1);
    auto p2 = std::make_shared<Player>(m_player2Name, board2);

    return std::make_unique<Game>(p1, p2);
}