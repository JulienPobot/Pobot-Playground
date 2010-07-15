#include <QtGui/QApplication>
#include "mainwindow.h"



int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    new MainWindow();

    return app.exec();
}
