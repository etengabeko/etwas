#include <QApplication>

#include "logger/logger.h"
#include "ui/connectionoptionsdialog.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Logger& logger = Logger::initialize(Logger::Level::Trace);

    ConnectionOptionsDialog dlg;

    int res = dlg.exec();
    if (res == QDialog::Accepted)
    {
        logger.debug(app.tr("Accepted: '%1:%2'").arg(dlg.address()).arg(dlg.port()));
    }
    else
    {
        logger.debug(app.tr("Rejected"));
    }

    return EXIT_SUCCESS;
}
