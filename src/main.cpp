#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTranslator>

#include "ui/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QString::fromUtf8(QFile(":/app_name.txt").readAll()));
    app.setApplicationVersion(QString::fromUtf8(QFile(":/app_version.txt").readAll()));

    QTranslator translator;
    translator.load(":/tr_ru");
    app.installTranslator(&translator);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption debugOption("debug",
                                   app.translate("main", "Enable Debug tools"));
    parser.addOption(debugOption);
    QCommandLineOption verboseOption("verbose",
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
