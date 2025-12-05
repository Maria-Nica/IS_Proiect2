#include "pch.h"
#include "gmock/gmock.h"
#include "GameFactory.h"
#include "Game.h"
#include "Position.h"
#include "Orientation.h"
#include "Cell.h"
#include "CellState.h"
#include "IGameListener.h"
#include <memory>
#include <vector>
#include <optional>

class MockListener : public IGameListener {
public:
    MOCK_METHOD(void, onShipPlaced, (const Ship& ship), (override));
    MOCK_METHOD(void, onShotFired, (const Cell& cell, GameState state), (override));
    MOCK_METHOD(void, onGameStateChanged, (GameState newState), (override));
};


class GameTest : public ::testing::Test {
protected:
    void SetUp() override {
        GameFactory factory;
        game = factory.create();
        listener = std::make_shared<MockListener>();
        game->addListener(listener);
    }

    std::unique_ptr<IGame> game;
    std::shared_ptr<MockListener> listener;
};


namespace {
    std::optional<Position> placeOneShipSafely(IGame* game) {
        int gridSize = game->getGridSize();

        for (int x = 0; x < gridSize; x++) {
            for (int y = 0; y < gridSize; y++) {
                for (auto o : { Orientation::Up, Orientation::Down, Orientation::Left, Orientation::Right }) {
                    if (game->placeShip(Position(x, y), 1, o)) {
                        return Position(x, y);
                    }
                }
            }
        }
        return std::nullopt;
    }

    void placeAllShips(IGame* game) {
        auto placeForCurrentPlayer = [game]() {
            for (int i = 0; i < game->getMaxShips(); i++) {
                ASSERT_TRUE(placeOneShipSafely(game));
            }
            };

        placeForCurrentPlayer();
        game->switchTurn();
        placeForCurrentPlayer();
        game->switchTurn();
    }

    void shootAllShipsOfPlayer(IGame* game, std::shared_ptr<IPlayer> targetPlayer) {
        auto board = targetPlayer->getBoard();
        for (const auto& ship : board->getShips()) {
            for (const auto& part : ship.getParts()) {
                game->shoot(part.getPosition());
            }
        }
    }

    std::optional<Position> findEmptyPosition(std::shared_ptr<IPlayer> player) {
        auto board = player->getBoard();
        int gridSize = board->getSize();

        for (int x = 0; x < gridSize; ++x) {
            for (int y = 0; y < gridSize; ++y) {
                Position pos(x, y);
                if (board->getCellState(pos) == CellState::Empty) {
                    return pos;
                }
            }
        }
        return std::nullopt;
    }

    std::optional<Position> findShipPartPosition(std::shared_ptr<IPlayer> player) {
        auto board = player->getBoard();
        int size = board->getSize();

        for (int x = 0; x < size; ++x) {
            for (int y = 0; y < size; ++y) {
                Position pos(x, y);
                if (board->getCellState(pos) == CellState::Ship) {
                    return pos;
                }
            }
        }

        return std::nullopt;
    }
}

TEST_F(GameTest, CreateAndStartGame) {
    ASSERT_NE(game, nullptr);
    EXPECT_NE(game->getPlayer1(), nullptr);
    EXPECT_NE(game->getPlayer2(), nullptr);

    EXPECT_CALL(*listener, onGameStateChanged(GameState::PlacingShips)).Times(1);
    game->startGame();
    EXPECT_EQ(game->getState(), GameState::PlacingShips);
}

// Listener notification tests
TEST_F(GameTest, ListenerNotifiedDuringShipPlacement) {
    game->startGame();
    EXPECT_CALL(*listener, onShipPlaced(testing::_)).Times(testing::AtLeast(1));
    placeOneShipSafely(game.get());
}

TEST_F(GameTest, MultipleListenersNotifiedOnShipPlaced) {
    auto l2 = std::make_shared<MockListener>();
    game->addListener(l2);

    game->startGame();
    EXPECT_CALL(*listener, onShipPlaced(testing::_)).Times(testing::AtLeast(1));
    EXPECT_CALL(*l2, onShipPlaced(testing::_)).Times(testing::AtLeast(1));
    placeOneShipSafely(game.get());
}

TEST_F(GameTest, ListenerNotNotifiedAfterRemoved) {
    game->removeListener(listener);

    game->startGame();
    EXPECT_CALL(*listener, onShipPlaced(testing::_)).Times(0);
    placeOneShipSafely(game.get());
}

TEST_F(GameTest, HandleExpiredListeners) {
    auto temp = std::make_shared<MockListener>();
    game->addListener(temp);

    game->startGame();
    EXPECT_CALL(*listener, onShipPlaced(testing::_)).Times(testing::AtLeast(1));
    placeOneShipSafely(game.get());
}

// State transitions tests
TEST_F(GameTest, PlacingShipsStateAfterOneShip) {
    game->startGame();
    EXPECT_EQ(game->getState(), GameState::PlacingShips);
}

TEST_F(GameTest, StateInProgressAfterAllShips) {
    game->startGame();
    EXPECT_CALL(*listener, onGameStateChanged(GameState::InProgress)).Times(1);
    placeAllShips(game.get());
    EXPECT_EQ(game->getState(), GameState::InProgress);
}

TEST_F(GameTest, ShootUpdatesCellAndNotifies) {
    game->startGame();
    placeAllShips(game.get());

    auto targetPos = Position(0, 0);
    EXPECT_CALL(*listener, onShotFired(testing::_, testing::_)).Times(testing::AtLeast(1));
    game->shoot(targetPos);
}

TEST_F(GameTest, ShootHitsShipUpdatesCellState) {
    game->startGame();

    placeOneShipSafely(game.get());
    game->switchTurn();
    auto p2Pos = placeOneShipSafely(game.get());
    game->switchTurn();

    ASSERT_TRUE(p2Pos.has_value());
    game->shoot(p2Pos.value());

    auto p2Board = game->getPlayer2()->getBoard();
    EXPECT_EQ(p2Board->getCellState(p2Pos.value()), CellState::Hit);
}

TEST_F(GameTest, ShootMissUpdatesCellState) {
    game->startGame();

    auto p1Pos = placeOneShipSafely(game.get());
    game->switchTurn();
    auto p2Pos = placeOneShipSafely(game.get());
    game->switchTurn();

    auto missPos = findEmptyPosition(game->getPlayer2());
    ASSERT_TRUE(missPos.has_value()); 

    game->shoot(missPos.value());

    auto p2Board = game->getPlayer2()->getBoard();
    EXPECT_EQ(p2Board->getCellState(missPos.value()), CellState::Miss);
}

TEST_F(GameTest, RepeatedShotDoesNotChangeCellState) {
    game->startGame();

    placeOneShipSafely(game.get());
    game->switchTurn();
    placeOneShipSafely(game.get());
    game->switchTurn();

    auto target = findShipPartPosition(game->getPlayer2());
    ASSERT_TRUE(target.has_value());

    game->shoot(target.value());

    auto p2Board = game->getPlayer2()->getBoard();
    auto firstState = p2Board->getCellState(target.value());
    EXPECT_EQ(firstState, CellState::Hit);

    game->shoot(target.value());
    auto secondState = p2Board->getCellState(target.value());
    EXPECT_EQ(secondState, CellState::Hit);
}

TEST_F(GameTest, TurnSwitchingAfterShoot) {
    game->startGame();
    placeAllShips(game.get());

    auto currentPlayerBefore = game->getCurrentPlayer().lock();
    game->shoot(Position(5, 5));
    auto currentPlayerAfter = game->getCurrentPlayer().lock();

    EXPECT_NE(currentPlayerBefore, currentPlayerAfter);
}

// Game over tests
TEST_F(GameTest, GameOverWhenOpponentShipsSunk) {
    game->startGame();
    placeAllShips(game.get());

    auto currentPlayer = game->getCurrentPlayer().lock();
    auto opponentPlayer = (currentPlayer == game->getPlayer1()) ? game->getPlayer2() : game->getPlayer1();

    shootAllShipsOfPlayer(game.get(), opponentPlayer);

    EXPECT_TRUE(game->isGameOver());
    EXPECT_EQ(game->getState(), GameState::GameOver);
}

TEST_F(GameTest, ResetClearsAndResetsPlacement) {
    game->startGame();

    placeOneShipSafely(game.get());
    game->switchTurn();
    placeOneShipSafely(game.get());

    game->startGame();

    auto p1Board = game->getPlayer1()->getBoard();
    auto p2Board = game->getPlayer2()->getBoard();

    EXPECT_EQ(p1Board->getShipsCount(), 0);
    EXPECT_EQ(p2Board->getShipsCount(), 0);

    int gridSize = p1Board->getSize();
    for (int x = 0; x < gridSize; ++x) {
        for (int y = 0; y < gridSize; ++y) {
            EXPECT_EQ(p1Board->getCellState(Position(x, y)), CellState::Empty);
            EXPECT_EQ(p2Board->getCellState(Position(x, y)), CellState::Empty);
        }
    }

    EXPECT_EQ(game->getState(), GameState::PlacingShips);
    EXPECT_TRUE(placeOneShipSafely(game.get()));
}
