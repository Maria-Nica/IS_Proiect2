#include "boardwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QApplication>

BoardWidget::BoardWidget(QWidget* parent, bool isPlacementMode)
	: QWidget(parent)
	, placementMode(isPlacementMode)
{
	setupBoard();
}

void BoardWidget::setupBoard()
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(10);
	mainLayout->setContentsMargins(10, 10, 10, 10);

	QLabel* instructionLabel = new QLabel(this);
	instructionLabel->setText(placementMode ? "Plaseaza avioanele (3 avioane)" : "Tabla de joc");
	instructionLabel->setAlignment(Qt::AlignCenter);
	instructionLabel->setStyleSheet("font-size:14pt; font-weight:bold;");
	mainLayout->addWidget(instructionLabel);

	gridLayout = new QGridLayout();
	gridLayout->setSpacing(2);
	gridLayout->setContentsMargins(0, 0, 0, 0);

	cells.resize(BOARD_SIZE);
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		cells[i].resize(BOARD_SIZE);
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			QPushButton* btn = new QPushButton(this);
			btn->setFixedSize(CELL_SIZE, CELL_SIZE);
			btn->setProperty("cellX", j);
			btn->setProperty("cellY", i);
			btn->setStyleSheet("background-color: #ADD8E6; border:1px solid #333;");
			connect(btn, &QPushButton::clicked, this, &BoardWidget::onCellClicked);
			if (placementMode) btn->installEventFilter(this);
			cells[i][j] = btn;
			gridLayout->addWidget(btn, i, j);
		}
	}

	QWidget* gridContainer = new QWidget(this);
	gridContainer->setLayout(gridLayout);
	int gridWidth = (CELL_SIZE * BOARD_SIZE) + (2 * (BOARD_SIZE - 1));
	gridContainer->setFixedSize(gridWidth, gridWidth);
	mainLayout->addWidget(gridContainer, 0, Qt::AlignCenter);

	if (placementMode)
	{
		rotateButton = new QPushButton("Roteste Avion (Varf: Sus)", this);
		rotateButton->setStyleSheet("font-size:12pt; padding:10px;");
		connect(rotateButton, &QPushButton::clicked, this, &BoardWidget::rotateCurrentShip);
		mainLayout->addWidget(rotateButton);
	}
	else rotateButton = nullptr;

	setLayout(mainLayout);
}

void BoardWidget::setPlacementMode(bool enabled)
{
	placementMode = enabled;
	if (rotateButton)
		rotateButton->setVisible(enabled);
}

void BoardWidget::setInteractive(bool enabled)
{
	interactive = enabled;
	for (int i = 0; i < BOARD_SIZE; ++i)
		for (int j = 0; j < BOARD_SIZE; ++j)
			cells[i][j]->setEnabled(enabled);
}

void BoardWidget::showShips(bool show)
{
	showShipsEnabled = show;
	updateAllCells();
}

void BoardWidget::reset()
{
	updateAllCells();
	placedCount = 0;
}

void BoardWidget::markHit(int x, int y, bool)
{
	updateCell(y, x);
}

void BoardWidget::markMiss(int x, int y)
{
	updateCell(y, x);
}

void BoardWidget::onCellClicked()
{
	if (!interactive)
		return;
	QPushButton* btn = qobject_cast<QPushButton*>(sender());
	if (!btn)
		return;
	int x = btn->property("cellX").toInt();
	int y = btn->property("cellY").toInt();
	if (placementMode && placedCount < 3)
	{
		clearPreview();
		Position headPos(x, y);
		Ship newShip(headPos, currentOrientation);
		if (!canPlaceShip(newShip))
		{
			QMessageBox::warning(this, "Pozitie invalida", "Avionul se suprapune cu alt avion sauiese din tabla!");
			return;
		}
		emit shipPlaced(newShip);
	}
	else
	{
		emit cellClicked(x, y);
	}
}

void BoardWidget::rotateCurrentShip()
{
	switch (currentOrientation)
	{
	case Orientation::Up:
		currentOrientation = Orientation::Right;
		rotateButton->setText("Roteste Avion (Varf: Dreapta)");
		break;
	case Orientation::Right:
		currentOrientation = Orientation::Down;
		rotateButton->setText("Roteste Avion (Varf: Jos)");
		break;
	case Orientation::Down:
		currentOrientation = Orientation::Left;
		rotateButton->setText("Roteste Avion (Varf: Stanga)");
		break;
	case Orientation::Left:
		currentOrientation = Orientation::Up;
		rotateButton->setText("Roteste Avion (Varf: Sus)");
		break;
	}
	if (hasPreview)
		showPreview(currentPreviewPosition.x(), currentPreviewPosition.y());
}

void BoardWidget::updateCell(int row, int col)
{
	if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
		return;
	if (!cells[row][col])
		return;
	int x = col;
	int y = row;
	QString styleSheet = QString("background-color: %1; border:1px solid #333;").arg(getCellColor(x, y));
	cells[row][col]->setStyleSheet(styleSheet);
	cells[row][col]->update();
}

void BoardWidget::updateAllCells()
{
	for (int r = 0; r < BOARD_SIZE; ++r)
		for (int c = 0; c < BOARD_SIZE; ++c)
			updateCell(r, c);
}

bool BoardWidget::canPlaceShip(const Ship& ship) const
{
	if (auto board = boardRef.lock())
		return board->canPlaceShip(ship);
	return false;
}

int BoardWidget::confirmPlacement(const Ship& /*ship*/)
{
	updateAllCells();
	++placedCount;
	return placedCount;
}

bool BoardWidget::eventFilter(QObject* obj, QEvent* event)
{
	if (!placementMode || placedCount >= 3)
		return QWidget::eventFilter(obj, event);
	QPushButton* btn = qobject_cast<QPushButton*>(obj);
	if (!btn)
		return QWidget::eventFilter(obj, event);
	if (event->type() == QEvent::Enter)
	{
		int x = btn->property("cellX").toInt();
		int y = btn->property("cellY").toInt();
		showPreview(x, y);
		return false;
	}
	else if (event->type() == QEvent::Leave)
	{
		clearPreview();
		return false;
	}
	return QWidget::eventFilter(obj, event);
}

void BoardWidget::showPreview(int x, int y)
{
	clearPreview();
	Position headPos(x, y);
	Ship previewShip(headPos, currentOrientation);
	bool isValid = canPlaceShip(previewShip);
	for (const auto& part : previewShip.getParts())
	{
		int px = part.getPosition().m_x;
		int py = part.getPosition().m_y;
		if (px >= 0 && px < BOARD_SIZE && py >= 0 && py < BOARD_SIZE)
		{
			if (!isValid)
				cells[py][px]->setStyleSheet("background-color: rgba(255,0,0,0.3); border:2px solid #FF0000;");
			else if (part.isHeadPart())
				cells[py][px]->setStyleSheet("background-color: rgba(0,255,0,0.5); border:2px solid #00FF00;");
			else
				cells[py][px]->setStyleSheet("background-color: rgba(76,175,80,0.3); border:2px solid #4CAF50;");
		}
	}
	currentPreviewPosition = QPoint(x, y); hasPreview = true;
}

void BoardWidget::clearPreview()
{
	if (!hasPreview)
		return;
	Position headPos(currentPreviewPosition.x(), currentPreviewPosition.y());
	Ship previewShip(headPos, currentOrientation);
	for (const auto& part : previewShip.getParts())
	{
		int px = part.getPosition().m_x;
		int py = part.getPosition().m_y;
		if (px >= 0 && px < BOARD_SIZE && py >= 0 && py < BOARD_SIZE)
			updateCell(py, px);
	}
	hasPreview = false;
}

QString BoardWidget::getCellColor(int x, int y) const
{
	if (auto board = boardRef.lock())
	{
		Cell info = board->getCellInfo(Position(x, y));
		switch (info.state)
		{
		case CellState::Empty:
			return "#ADD8E6";
		case CellState::Ship:
			return (showShipsEnabled && !isEnemyBoard) ? "#808080" : "#ADD8E6";
		case CellState::Hit:
			return info.isHead ? "#8B0000" : "#FF4444";
		case CellState::HeadHit:
			return "#8B0000";
		case CellState::Miss:
			return "#FFFFFF";
		}
		return "#ADD8E6";
	}

	return "#ADD8E6";
}