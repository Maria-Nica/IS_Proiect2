#include "gameui.h"
#include <QMessageBox>
#include <QApplication>
#include "IBoard.h"
#include "IPlayer.h"
#include "Position.h"
#include "Ship.h"
#include "qt_helpers.h"
#include "PositionUtils.h"
#include <iostream>

GameUI::GameUI(std::unique_ptr<IGame> gameInstance, QWidget* parent)
	: QWidget(parent)
	, isTransitioning(false)
	, game(std::move(gameInstance))
{
	player1 = game->getPlayer1();
	player2 = game->getPlayer2();

	listenerPtr = std::shared_ptr<IGameListener>(this, [](IGameListener*) {});
	game->addListener(listenerPtr);

	setupUI();
	game->startGame();

	player1PlacementBoard->setBoard(player1->getBoard());
	player2PlacementBoard->setBoard(player2->getBoard());
	player1OwnBoard->setBoard(player1->getBoard());
	player2OwnBoard->setBoard(player2->getBoard());
	player1EnemyBoard->setBoard(player2->getBoard());
	player2EnemyBoard->setBoard(player1->getBoard());

	updateUIForCurrentState();
}

GameUI::~GameUI() {}

void GameUI::onShipPlaced(const Ship&)
{
	updateUIForCurrentState();
}

void GameUI::onShotFired(const Cell& cell, GameState)
{
	auto currentPlayer = game->getCurrentPlayer().lock();
	std::shared_ptr<IBoard> targetBoard = (currentPlayer == player1) ? player2->getBoard() : player1->getBoard();

	bool isHead = false;
	if (cell.state == CellState::Hit && targetBoard)
	{
		Cell info = targetBoard->getCellInfo(cell.position);
		isHead = info.isHead;
	}

	auto syncAll = [this]() {
		syncBoardDisplay(player1OwnBoard, player1->getBoard(), true);
		syncBoardDisplay(player2OwnBoard, player2->getBoard(), true);
		syncBoardDisplay(player1EnemyBoard, player2->getBoard(), false);
		syncBoardDisplay(player2EnemyBoard, player1->getBoard(), false);
		};
	syncAll();

	QApplication::processEvents();
}

void GameUI::onGameStateChanged(GameState newState)
{
	if (newState == GameState::GameOver)
	{
		if (!player1->hasRemainingShips())
			showGameOverMessage("Jucator2");
		else if (!player2->hasRemainingShips())
			showGameOverMessage("Jucator1");
	}
	else
	{
		updateUIForCurrentState();
	}
}

void GameUI::setupUI()
{
	stackedWidget = new QStackedWidget(this);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(stackedWidget);
	setLayout(mainLayout);

	createPlacementScreen();
	createTransitionScreen();
	createGameScreen();
}

void GameUI::createPlacementScreen()
{
	player1PlacementWidget = new QWidget();
	QVBoxLayout* p1Layout = new QVBoxLayout(player1PlacementWidget);
	p1Layout->setAlignment(Qt::AlignCenter);

	QLabel* p1Label = new QLabel("Jucator1 - Plaseaza Avioanele");
	p1Label->setStyleSheet(UiStyles::largeLabelStyle());
	p1Label->setAlignment(Qt::AlignCenter);
	p1Layout->addWidget(p1Label);

	player1PlacementBoard = new BoardWidget(player1PlacementWidget, true);
	p1Layout->addWidget(player1PlacementBoard, 0, Qt::AlignCenter);

	player1DoneButton = createStyledButton(player1PlacementWidget, "Gata - Treci la Jucator2",
		UiStyles::greenButton(), 300);
	player1DoneButton->setEnabled(false);
	connect(player1DoneButton, &QPushButton::clicked, this, &GameUI::onPlayer1ShipsPlaced);
	connect(player1PlacementBoard, &BoardWidget::shipPlaced, this, [this](const Ship& ship) { placeShipFromWidget(player1PlacementBoard, ship); });
	p1Layout->addWidget(player1DoneButton, 0, Qt::AlignCenter);

	stackedWidget->addWidget(player1PlacementWidget);

	player2PlacementWidget = new QWidget();
	QVBoxLayout* p2Layout = new QVBoxLayout(player2PlacementWidget);
	p2Layout->setAlignment(Qt::AlignCenter);

	QLabel* p2Label = new QLabel("Jucator2 - Plaseaza Avioanele");
	p2Label->setStyleSheet(UiStyles::largeLabelStyle());
	p2Label->setAlignment(Qt::AlignCenter);
	p2Layout->addWidget(p2Label);

	player2PlacementBoard = new BoardWidget(player2PlacementWidget, true);
	p2Layout->addWidget(player2PlacementBoard, 0, Qt::AlignCenter);

	player2DoneButton = createStyledButton(player2PlacementWidget, "Gata - Incepe Jocul",
		UiStyles::greenButton(), 300);
	player2DoneButton->setEnabled(false);
	connect(player2DoneButton, &QPushButton::clicked, this, &GameUI::onPlayer2ShipsPlaced);
	connect(player2PlacementBoard, &BoardWidget::shipPlaced, this, [this](const Ship& ship) { placeShipFromWidget(player2PlacementBoard, ship); });
	p2Layout->addWidget(player2DoneButton, 0, Qt::AlignCenter);

	stackedWidget->addWidget(player2PlacementWidget);
}

void GameUI::createTransitionScreen()
{
	transitionWidget = new QWidget();
	transitionWidget->setStyleSheet("background-color: #808080;");

	QVBoxLayout* layout = new QVBoxLayout(transitionWidget);
	layout->setAlignment(Qt::AlignCenter);

	transitionLabel = new QLabel("Treci laptopul la celalalt jucator");
	transitionLabel->setStyleSheet(UiStyles::headerLabelStyle());
	transitionLabel->setAlignment(Qt::AlignCenter);
	layout->addWidget(transitionLabel);

	transitionButton = createStyledButton(transitionWidget, "Continua",
		UiStyles::blueButton(), 200);
	connect(transitionButton, &QPushButton::clicked, this, &GameUI::onContinueButtonClicked);
	layout->addWidget(transitionButton, 0, Qt::AlignCenter);

	stackedWidget->addWidget(transitionWidget);
}

void GameUI::createGameScreen()
{
	player1GameWidget = new QWidget();
	QVBoxLayout* p1MainLayout = new QVBoxLayout(player1GameWidget);

	QLabel* p1TitleLabel = new QLabel("Jucator1 - Runda Ta");
	p1TitleLabel->setStyleSheet(UiStyles::titleStyle());
	p1TitleLabel->setAlignment(Qt::AlignCenter);
	p1MainLayout->addWidget(p1TitleLabel);

	QHBoxLayout* p1BoardsLayout = new QHBoxLayout();

	QVBoxLayout* p1OwnLayout = new QVBoxLayout();
	QLabel* p1OwnLabel = new QLabel("Avioanele Tale");
	p1OwnLabel->setStyleSheet(UiStyles::sectionLabelStyle());
	p1OwnLabel->setAlignment(Qt::AlignCenter);
	p1OwnLayout->addWidget(p1OwnLabel);

	player1OwnBoard = new BoardWidget(player1GameWidget, false);
	player1OwnBoard->setInteractive(false);
	player1OwnBoard->setEnemyBoard(false);
	p1OwnLayout->addWidget(player1OwnBoard);

	QVBoxLayout* p1EnemyLayout = new QVBoxLayout();
	QLabel* p1EnemyLabel = new QLabel("Avioanele Inamicului");
	p1EnemyLabel->setStyleSheet(UiStyles::sectionLabelStyle());
	p1EnemyLabel->setAlignment(Qt::AlignCenter);
	p1EnemyLayout->addWidget(p1EnemyLabel);

	player1EnemyBoard = new BoardWidget(player1GameWidget, false);
	player1EnemyBoard->setEnemyBoard(true);
	player1EnemyBoard->showShips(false);
	connect(player1EnemyBoard, &BoardWidget::cellClicked, this, &GameUI::onPlayer1CellClicked);
	p1EnemyLayout->addWidget(player1EnemyBoard);

	p1BoardsLayout->addLayout(p1OwnLayout);
	p1BoardsLayout->addLayout(p1EnemyLayout);
	p1MainLayout->addLayout(p1BoardsLayout);

	player1EndTurnButton = createStyledButton(player1GameWidget, "Termina Runda",
		UiStyles::orangeButton(), 300);
	player1EndTurnButton->setEnabled(false);
	connect(player1EndTurnButton, &QPushButton::clicked, this, &GameUI::onEndTurnButtonClicked);
	p1MainLayout->addWidget(player1EndTurnButton, 0, Qt::AlignCenter);

	stackedWidget->addWidget(player1GameWidget);

	player2GameWidget = new QWidget();
	QVBoxLayout* p2MainLayout = new QVBoxLayout(player2GameWidget);

	QLabel* p2TitleLabel = new QLabel("Jucator2 - Runda Ta");
	p2TitleLabel->setStyleSheet(UiStyles::titleStyle());
	p2TitleLabel->setAlignment(Qt::AlignCenter);
	p2MainLayout->addWidget(p2TitleLabel);

	QHBoxLayout* p2BoardsLayout = new QHBoxLayout();

	QVBoxLayout* p2OwnLayout = new QVBoxLayout();
	QLabel* p2OwnLabel = new QLabel("Avioanele Tale");
	p2OwnLabel->setStyleSheet(UiStyles::sectionLabelStyle());
	p2OwnLabel->setAlignment(Qt::AlignCenter);
	p2OwnLayout->addWidget(p2OwnLabel);

	player2OwnBoard = new BoardWidget(player2GameWidget, false);
	player2OwnBoard->setInteractive(false);
	player2OwnBoard->setEnemyBoard(false);
	p2OwnLayout->addWidget(player2OwnBoard);

	QVBoxLayout* p2EnemyLayout = new QVBoxLayout();
	QLabel* p2EnemyLabel = new QLabel("Avioanele Inamicului");
	p2EnemyLabel->setStyleSheet(UiStyles::sectionLabelStyle());
	p2EnemyLabel->setAlignment(Qt::AlignCenter);
	p2EnemyLayout->addWidget(p2EnemyLabel);

	player2EnemyBoard = new BoardWidget(player2GameWidget, false);
	player2EnemyBoard->setEnemyBoard(true);
	player2EnemyBoard->showShips(false);
	connect(player2EnemyBoard, &BoardWidget::cellClicked, this, &GameUI::onPlayer2CellClicked);
	p2EnemyLayout->addWidget(player2EnemyBoard);

	p2BoardsLayout->addLayout(p2OwnLayout);
	p2BoardsLayout->addLayout(p2EnemyLayout);
	p2MainLayout->addLayout(p2BoardsLayout);

	player2EndTurnButton = createStyledButton(player2GameWidget, "Termina Runda",
		UiStyles::orangeButton(), 300);
	player2EndTurnButton->setEnabled(false);
	connect(player2EndTurnButton, &QPushButton::clicked, this, &GameUI::onEndTurnButtonClicked);
	p2MainLayout->addWidget(player2EndTurnButton, 0, Qt::AlignCenter);

	stackedWidget->addWidget(player2GameWidget);
}

void GameUI::onPlayer1ShipsPlaced()
{
	if (player1->allShipsPlaced(game->getMaxShips()))
	{
		isTransitioning = true;

		showTransitionScreen("Trece laptopul la Jucator2");

		game->switchTurn();
	}
	else
	{
		QMessageBox::warning(this, "Avertisment", "Inca nu s-au plasat toate avioanele.");
	}
}

void GameUI::onPlayer2ShipsPlaced()
{
	if (player2->allShipsPlaced(game->getMaxShips()))
	{
		isTransitioning = true;

		syncBoardDisplay(player1OwnBoard, player1->getBoard(), true);
		syncBoardDisplay(player2OwnBoard, player2->getBoard(), true);
		syncBoardDisplay(player1EnemyBoard, player2->getBoard(), false);
		syncBoardDisplay(player2EnemyBoard, player1->getBoard(), false);

		showTransitionScreen("Gata! Treci laptopul la Jucator1 pentru a incepe");

		game->switchTurn();
	}
	else
	{
		QMessageBox::warning(this, "Avertisment", "Inca nu s-au plasat toate avioanele.");
	}
}

void GameUI::onContinueButtonClicked()
{
	isTransitioning = false;
	updateUIForCurrentState();
}

void GameUI::onEndTurnButtonClicked()
{
	auto currentPlayer = game->getCurrentPlayer().lock();
	if (currentPlayer == player1)
	{
		player1EnemyBoard->setInteractive(false);
		player1EndTurnButton->setEnabled(false);
	}
	else if (currentPlayer == player2)
	{
		player2EnemyBoard->setInteractive(false);
		player2EndTurnButton->setEnabled(false);
	}
	isTransitioning = true;
	updateUIForCurrentState();
}

void GameUI::onPlayer1CellClicked(int x, int y)
{
	auto currentPlayer = game->getCurrentPlayer().lock();
	if (currentPlayer != player1)
		return;

	Position shotPos(x, y);
	CellState cellState = player2->getBoard()->getCellState(shotPos);
	if (cellState == CellState::Hit || cellState == CellState::Miss)
	{
		QMessageBox::information(this, "Celula deja atacata", "Ai atacat deja aceasta celula!");
		return;
	}

	game->shoot(shotPos);
	syncBoardDisplay(player1EnemyBoard, player2->getBoard(), false);
	QApplication::processEvents();
	player1EndTurnButton->setEnabled(true);
	player1EnemyBoard->setInteractive(false);
}

void GameUI::onPlayer2CellClicked(int x, int y)
{
	auto currentPlayer = game->getCurrentPlayer().lock();
	if (currentPlayer != player2)
		return;

	Position shotPos(x, y);
	CellState cellState = player1->getBoard()->getCellState(shotPos);
	if (cellState == CellState::Hit || cellState == CellState::Miss)
	{
		QMessageBox::information(this, "Celula deja atacata", "Ai atacat deja aceasta celula!");
		return;
	}

	game->shoot(shotPos);
	syncBoardDisplay(player2EnemyBoard, player1->getBoard(), false);
	QApplication::processEvents();
	player2EndTurnButton->setEnabled(true);
	player2EnemyBoard->setInteractive(false);
}

void GameUI::syncBoardDisplay(BoardWidget* boardWidget, std::shared_ptr<IBoard> board, bool showShips)
{
	if (!boardWidget || !board)
		return;

	boardWidget->setBoard(board);
	boardWidget->showShips(showShips);
	QApplication::processEvents();
}

void GameUI::showGameOverMessage(const QString& winner)
{
	QMessageBox::information(this, "Joc Terminat!", QString("Castigator: %1!\n\nToate avioanele au fost distruse.").arg(winner));
	close();
}

void GameUI::updateUIForCurrentState()
{
	GameState state = game->getState();
	auto currentPlayer = game->getCurrentPlayer().lock();

	switch (state)
	{
	case GameState::PlacingShips:
		if (isTransitioning)
			return;
		if (currentPlayer == player1)
			stackedWidget->setCurrentWidget(player1PlacementWidget);
		else if (currentPlayer == player2)
			stackedWidget->setCurrentWidget(player2PlacementWidget);
		break;
	case GameState::InProgress:
	case GameState::SwitchingTurn:
		if (isTransitioning)
		{
			QString playerName = (currentPlayer == player1) ? "Jucator1" : "Jucator2";
			showTransitionScreen("Treci laptopul la " + playerName);
		}
		else
		{
			if (currentPlayer == player1)
			{
				stackedWidget->setCurrentWidget(player1GameWidget);
				player1EnemyBoard->setInteractive(true);
				player1EndTurnButton->setEnabled(false);
			}
			else if (currentPlayer == player2)
			{
				stackedWidget->setCurrentWidget(player2GameWidget);
				player2EnemyBoard->setInteractive(true);
				player2EndTurnButton->setEnabled(false);
			}
		}
		break;
	case GameState::GameOver:
		break;
	}
}

void GameUI::showTransitionScreen(const QString& message)
{
	transitionLabel->setText(message);
	stackedWidget->setCurrentWidget(transitionWidget);
}

void GameUI::placeShipFromWidget(BoardWidget* source, const Ship& ship)
{
	const auto& parts = ship.getParts();
	if (parts.empty()) return;
	const Position& headPos = parts[0].getPosition();
	Orientation orient = Orientation::Up;
	if (parts.size() > 1)
		orient = LogicUtils::computeOrientation(headPos, parts[1].getPosition());

	if (!game->placeShip(headPos, 1, orient))
	{
		QMessageBox::warning(this, "Eroare", "Nu s-a putut plasa avionul in Logic!");
		return;
	}

	int newCount = source->confirmPlacement(ship);
	if (source == player1PlacementBoard)
	{
		if (newCount >= game->getMaxShips())
			player1DoneButton->setEnabled(true);
	}
	else if (source == player2PlacementBoard)
	{
		if (newCount >= game->getMaxShips())
			player2DoneButton->setEnabled(true);
	}

	syncBoardDisplay(player1OwnBoard, player1->getBoard(), true);
	syncBoardDisplay(player2OwnBoard, player2->getBoard(), true);
}