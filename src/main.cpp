#include <QApplication>

#include "logger/logger.h"
#include "ui/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Logger::initialize(Logger::Level::Trace); // TODO: init from argv

    MainWindow w;
    w.show();

    return app.exec();
}
