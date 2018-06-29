#include <QApplication>
#include <QCommandLineParser>

#include "ui/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    QCommandLineOption debugOption("debug", app.tr("Enable Debug tools"));
    parser.addOption(debugOption);
    parser.process(app);

    MainWindow w(parser.isSet(debugOption) ? MainWindow::Mode::Debug
                                           : MainWindow::Mode::Work);
    w.show();

    return app.exec();
}
