#include <QCoreApplication>

#include "logger/logger.h"
#include "protocol/protocol.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    Q_UNUSED(app);

    Logger& logger = Logger::initialize(Logger::Level::Trace);
    logger.trace("Hello ФЗК-2 Тест!");

    return EXIT_SUCCESS;
}
