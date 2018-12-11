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
    parser.addHelpOption();
    QCommandLineOption debugOption("debug",
                                   app.translate("main", "Enable Debug tools"));
    parser.addOption(debugOption);
    QCommandLineOption verboseOption({"v", "verbose"},
                                     app.translate("main", "Enable verbose mode"));
    parser.addOption(verboseOption);
    parser.process(app);

    MainWindow w(parser.isSet(debugOption) ? MainWindow::Mode::Debug
                                           : MainWindow::Mode::Work);
    if (parser.isSet(verboseOption))
    {
        w.enableVerboseMode();
    }
    w.show();

    return app.exec();
}
