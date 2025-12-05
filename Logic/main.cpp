#include <iostream>
#include <memory>
#include "Game.h"
#include "Player.h"
#include "Board.h"
#include "Orientation.h"
#include "GameFactory.h"

int main() {
    std::cout << "========== SIMULARE GAME 2 JUCATORI ==========\n\n";

    GameFactory factory;
    auto game = factory.create();

    game->startGame();

    // ============================
    //   PLASARE AVIOANE
    // ============================

    std::cout << "\n=== Player1 isi plaseaza avioanele ===\n";

    game->placeShip({ 3,3 }, 10, Orientation::Up);
    game->placeShip({ 8,5 }, 10, Orientation::Right);

    auto board1 = game->getPlayer1()->getBoard();
    auto board2 = game->getPlayer2()->getBoard();
    board1->print();

    std::cout << "\n=== Player2 isi plaseaza avioanele ===\n";

    // switch manually since Game expects players to alternate
    game->switchTurn();

    game->placeShip({ 2,2 }, 10, Orientation::Down);   // avion 1
    game->placeShip({ 6,7 }, 10, Orientation::Left);   // avion 2

    board2->print();

    // ============================
    //   SIMULARE TRAGERI
    // ============================

    std::cout << "\n========== ÎNCEP TRAGERILE ==========\n\n";

    std::cout << "\n Runda 1: P1 trage în head-ul lui P2 la (2,2)\n";
    game->shoot({ 2, 2 });
    board2->print();

    std::cout << "\n Runda 2: P2 trage în head-ul lui P1 la (3,3)\n";
    game->shoot({ 3, 3 });

    std::cout << "\n🎯 Runda 3: P1 trage în head-ul avionului 2 P2 la (6,7)\n";
    game->shoot({ 6, 7 });

    std::cout << "\n🎯 Runda 4: P2 trage în head-ul avionului 2 P1 la (8,5)\n";
    game->shoot({ 8, 5 });
    board1->print();

    // ============================
    //   FINAL JOC
    // ============================

    std::cout << "\n========== VERIFICAM CINE A CASTIGAT ==========\n\n";

    if (board1->allShipsSunk() && board2->allShipsSunk())
        std::cout << " Remiza (teoretic imposibil dar logic permis)\n";
    else if (board1->allShipsSunk())
        std::cout << " Player2 castiga! Player1 nu mai are avioane.\n";
    else if (board2->allShipsSunk())
        std::cout << " Player1 castiga! Player2 nu mai are avioane.\n";
    else
        std::cout << "Amandoi mai au avioane (jocul nu s-a terminat)\n";

    return 0;
}
