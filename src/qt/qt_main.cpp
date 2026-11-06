#include <QApplication>

#include "MainWindow.h"

using namespace chess;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("chess_qt"));
    QApplication::setOrganizationName(QStringLiteral("chess-engine"));

    MainWindow window;
    window.show();
    return app.exec();
}
