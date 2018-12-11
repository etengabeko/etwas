#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTranslator>

#include "ui/mainwindow.h"

namespace
{

QString readFileFromResources(const QString& fileName)
{
    QFile f(fileName);
    const bool ok = f.open(QFile::ReadOnly);
    Q_ASSERT(ok);

    return (ok ? QString::fromUtf8(f.readAll()).trimmed()
               : QString::null);
}

QString customAppName()
{
    return readFileFromResources(":/app_name.txt");
}

QString customAppVersion()
{
    return readFileFromResources(":/app_version.txt");
}

}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(::customAppName());
    app.setApplicationVersion(::customAppVersion());

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
