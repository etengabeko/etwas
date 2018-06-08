#include <QApplication>
#include <QHostAddress>

#include "logger/logger.h"
#include "ui/connectionoptionsdialog.h"
#include "ui/controlpanelwidget.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Logger& logger = Logger::initialize(Logger::Level::Trace);

    ControlPanelWidget ctrl;
    ctrl.show();
    {
        ConnectionOptionsDialog dlg(&ctrl);
        int res = dlg.exec();
        if (res == QDialog::Accepted)
        {
            logger.debug(app.tr("Accepted: '%1:%2'").arg(dlg.address()).arg(dlg.port()));
            ctrl.initialize(QHostAddress(dlg.address()),
                            dlg.port());
        }
        else
        {
            logger.debug(app.tr("Rejected"));
            return EXIT_FAILURE;
        }
    }
    return app.exec();
}
