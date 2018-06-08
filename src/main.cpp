#include <QCoreApplication>

#include "logger/logger.h"
#include "protocol/protocol.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    Logger& logger = Logger::initialize(Logger::Level::Trace);
    logger.trace(app.tr("Hello FZK-2 Test!"));

    return EXIT_SUCCESS;
}
