#pragma once

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QVector>
#include <QPoint>
#include <QEvent>
#include <vector>
#include "CellState.h"
#include "Orientation.h"
#include "Ship.h"
#include "Position.h"
#include "IBoard.h"

class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent = nullptr, bool isPlacementMode = true);

    void setPlacementMode(bool enabled);
    void setInteractive(bool enabled);
    void showShips(bool show);
    void reset();

    bool allShipsPlaced() const { return placedCount >= 3; }
    void setEnemyBoard(bool isEnemy) { isEnemyBoard = isEnemy; }
    void setBoard(std::shared_ptr<IBoard> board) { boardRef = board; }
    std::shared_ptr<IBoard> getBoard() const { return boardRef.lock(); }

    void markHit(int x, int y, bool isHead);
    void markMiss(int x, int y);

    int confirmPlacement(const Ship& ship);
    int getPlacedCount() const { return placedCount; }

signals:
    void cellClicked(int x, int y);
    void shipPlaced(const Ship& ship);

private slots:
    void onCellClicked();
    void rotateCurrentShip();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupBoard();
    void updateCell(int row, int col);
    void updateAllCells();
    bool canPlaceShip(const Ship& ship) const;
    QString getCellColor(int x, int y) const;
    void clearPreview();
    void showPreview(int x, int y);

    static constexpr int BOARD_SIZE = 10;
    static constexpr int CELL_SIZE = 40;

    QVector<QVector<QPushButton*>> cells;

    bool placementMode{ true };
    bool interactive{ true };
    bool showShipsEnabled{ true };
    bool isEnemyBoard{ false };
    Orientation currentOrientation{ Orientation::Up };

    QPoint currentPreviewPosition;
    bool hasPreview{ false };

    QGridLayout* gridLayout{ nullptr };
    QPushButton* rotateButton{ nullptr };

    int placedCount{ 0 };
    std::weak_ptr<IBoard> boardRef;
};