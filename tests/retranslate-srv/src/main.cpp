#include <QCoreApplication>
#include <QCommandLineParser>

#include "retranslateserver.h"

namespace
{

QString defaultPortNumber() { return "30000"; }

}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption portOption(QStringList({ "p", "port" }),
                                  app.tr("Listen port"),
                                  app.tr("number"),
                                  ::defaultPortNumber());
    parser.addOption(portOption);
    parser.process(app);

    const quint16 port = parser.isSet(portOption) ? parser.value(portOption).toUShort()
                                                  : ::defaultPortNumber().toUShort();
    test::RetranslateServer srv;
    if (!srv.start(port))
    {
        return EXIT_FAILURE;
    }
    return app.exec();
}
