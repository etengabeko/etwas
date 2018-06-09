#include <QApplication>
#include <QCursor>
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

    ConnectionOptionsDialog* dlg = new ConnectionOptionsDialog(&ctrl);
    QObject::connect(&ctrl, &ControlPanelWidget::error,
                     dlg, &ConnectionOptionsDialog::setErrorString);
    bool ok = false;
    do
    {
        int res = dlg->exec();
        if (res == QDialog::Accepted)
        {
            logger.debug(app.tr("Accepted: '%1:%2'")
                         .arg(dlg->address().toString())
                         .arg(dlg->port()));
            app.setOverrideCursor(QCursor(Qt::WaitCursor));
            ok = ctrl.initialize(dlg->address(), dlg->port());
            app.restoreOverrideCursor();
        }
        else
        {
            logger.debug(app.tr("Rejected"));
            break;
        }
    }
    while (!ok);

    delete dlg;
    dlg = nullptr;

    return (ok ? app.exec() : EXIT_SUCCESS);
}
