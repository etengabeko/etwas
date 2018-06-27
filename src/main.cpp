#include <QApplication>
#include <QCommandLineParser>

#include "logger/logger.h"
#include "ui/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    QCommandLineOption debugOption("debug", app.tr("Enable Debug tools"));
    parser.addOption(debugOption);
    parser.process(app);

    Logger::initialize(Logger::Level::Trace); // TODO: init from argv

    MainWindow w(parser.isSet(debugOption) ? MainWindow::Mode::Debug
                                           : MainWindow::Mode::Work);
    w.show();

    return app.exec();
}
