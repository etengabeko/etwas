#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QCursor>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QString>

#include "ui/connectionoptionsdialog.h"
#include "ui/controlpanelwidget.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_central(new QMdiArea(this))
{
    setWindowTitle(tr("FZK-2 Test"));
    setCentralWidget(m_central);

    initMenu();
}

MainWindow::~MainWindow()
{

}

void MainWindow::initMenu()
{
    QMenu* controlsMenu = menuBar()->addMenu(tr("Controls"));
    QAction* newControlAction = controlsMenu->addAction(tr("New connection"));
    QObject::connect(newControlAction, &QAction::triggered,
                     this, &MainWindow::slotNewConnection);
}

void MainWindow::slotNewConnection()
{
    ConnectionOptionsDialog* dlg = new ConnectionOptionsDialog(this);
    m_central->addSubWindow(dlg);

    QObject::connect(dlg, &ConnectionOptionsDialog::accepted,
                     this, &MainWindow::slotNewControl);
    QObject::connect(dlg, &ConnectionOptionsDialog::rejected,
                     [dlg, this]() { m_central->removeSubWindow(dlg); dlg->deleteLater(); });

    dlg->show();
}

void MainWindow::slotNewControl()
{
    ConnectionOptionsDialog* dlg = qobject_cast<ConnectionOptionsDialog*>(sender());
    if (dlg != nullptr)
    {
        ControlPanelWidget* control = new ControlPanelWidget(this);
        m_central->addSubWindow(control);

        QObject::connect(control, &ControlPanelWidget::closed,
                         [control, this]() { m_central->removeSubWindow(control); });
        QObject::connect(control, &ControlPanelWidget::error,
                         this, &MainWindow::slotOnError);

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        bool ok = control->initialize(dlg->address(), dlg->port());
        m_central->removeSubWindow(dlg);
        dlg->deleteLater();
        if (ok)
        {
            control->show();
        }
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::slotOnError(const QString& message)
{
    QMessageBox::warning(this,
                         tr("Connection error"),
                         message);
    ControlPanelWidget* control = qobject_cast<ControlPanelWidget*>(sender());
    if (control != nullptr)
    {
        m_central->removeSubWindow(control);
    }
}
