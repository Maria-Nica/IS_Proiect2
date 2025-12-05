#pragma once

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <memory>
#include "IGameListener.h"
#include "IGame.h"
#include "GameState.h"
#include "Position.h"
#include "Ship.h"
#include "boardwidget.h"

class GameUI : public QWidget, public IGameListener
{
    Q_OBJECT

public:
    explicit GameUI(std::unique_ptr<IGame> gameInstance, QWidget* parent = nullptr);
    ~GameUI() override;

    void onShipPlaced(const Ship& ship) override;
    void onShotFired(const Cell& cell, GameState gameState) override;
    void onGameStateChanged(GameState newState) override;

private slots:
    void onPlayer1ShipsPlaced();
    void onPlayer2ShipsPlaced();
    void onContinueButtonClicked();
    void onPlayer1CellClicked(int x, int y);
    void onPlayer2CellClicked(int x, int y);
    void onEndTurnButtonClicked();

private:
    void setupUI();
    void createPlacementScreen();
    void createTransitionScreen();
    void createGameScreen();
    void updateUIForCurrentState();
    void showTransitionScreen(const QString& message);
    void syncBoardDisplay(BoardWidget* boardWidget, std::shared_ptr<IBoard> board, bool showShips);
    void showGameOverMessage(const QString& winner);

    void placeShipFromWidget(BoardWidget* source, const Ship& ship);

    QStackedWidget* stackedWidget{ nullptr };

    QWidget* player1PlacementWidget{ nullptr };
    QWidget* player2PlacementWidget{ nullptr };
    BoardWidget* player1PlacementBoard{ nullptr };
    BoardWidget* player2PlacementBoard{ nullptr };
    QPushButton* player1DoneButton{ nullptr };
    QPushButton* player2DoneButton{ nullptr };

    QWidget* transitionWidget{ nullptr };
    QLabel* transitionLabel{ nullptr };
    QPushButton* transitionButton{ nullptr };

    QWidget* player1GameWidget{ nullptr };
    QWidget* player2GameWidget{ nullptr };
    BoardWidget* player1OwnBoard{ nullptr };
    BoardWidget* player1EnemyBoard{ nullptr };
    BoardWidget* player2OwnBoard{ nullptr };
    BoardWidget* player2EnemyBoard{ nullptr };
    QPushButton* player1EndTurnButton{ nullptr };
    QPushButton* player2EndTurnButton{ nullptr };

    std::unique_ptr<IGame> game;
    std::shared_ptr<IPlayer> player1;
    std::shared_ptr<IPlayer> player2;

    std::shared_ptr<IGameListener> listenerPtr{ nullptr };

    bool isTransitioning{ false };
};