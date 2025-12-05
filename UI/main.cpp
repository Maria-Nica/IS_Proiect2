#include "gameui.h"
#include <QApplication>
#include "IGameFactory.h"
#include "GameFactory.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    std::unique_ptr<IGameFactory> factory = std::make_unique<GameFactory>("Player1", "Player2");
    auto game = factory->create();

    GameUI gameWindow(std::move(game));
    gameWindow.resize(1400, 800);
    gameWindow.setWindowTitle("Jocul Avioane");
    gameWindow.show();

    return a.exec();
}
