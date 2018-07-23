#include <QApplication>
#include <QCommandLineParser>
#include <QTranslator>

#include "ui/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QTranslator translator;
    translator.load(":/tr_ru");
    app.installTranslator(&translator);

    QCommandLineParser parser;
    QCommandLineOption debugOption("debug", app.translate("main", "Enable Debug tools"));
    parser.addOption(debugOption);
    parser.process(app);

    MainWindow w(parser.isSet(debugOption) ? MainWindow::Mode::Debug
                                           : MainWindow::Mode::Work);
    w.show();

    return app.exec();
}
