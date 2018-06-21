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
    while (!m_subwindows.isEmpty())
    {
        closeSubWindow(m_subwindows.begin().key());
    }
}

void MainWindow::initMenu()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("File"));

    QAction* action = fileMenu->addAction(tr("New connection"));
    QObject::connect(action, &QAction::triggered,
                     this, &MainWindow::slotNewConnection);

    action = fileMenu->addAction(tr("Debug tools"));
    QObject::connect(action, &QAction::triggered,
                     this, &MainWindow::slotNewDebugConnection);

    action = fileMenu->addAction(tr("Quit"));
    QObject::connect(action, &QAction::triggered,
                     this, &MainWindow::close);
}

void MainWindow::slotNewConnection()
{
    createNewConnection(Mode::Work);
}

void MainWindow::slotNewDebugConnection()
{
    createNewConnection(Mode::Debug);
}

void MainWindow::createNewConnection(MainWindow::Mode mode)
{
    ConnectionOptionsDialog* dlg = new ConnectionOptionsDialog(this);
    if (mode == Mode::Debug)
    {
        dlg->setDebugMode(true);
        dlg->setAddress(QHostAddress(QHostAddress::LocalHost));
    }

    QMdiSubWindow* mdi = m_central->addSubWindow(dlg);
    m_subwindows.insert(dlg, mdi);

    QObject::connect(dlg, &ConnectionOptionsDialog::accepted,
                     this, &MainWindow::slotNewControlPanel);
    QObject::connect(dlg, &ConnectionOptionsDialog::rejected,
                     this, &MainWindow::slotCloseSubWindow);

    dlg->show();
}

void MainWindow::slotNewControlPanel()
{
    ConnectionOptionsDialog* dlg = qobject_cast<ConnectionOptionsDialog*>(sender());
    if (dlg != nullptr)
    {
        ControlPanelWidget* control = new ControlPanelWidget(dlg->isDebugMode(), this);
        QMdiSubWindow* mdi = m_central->addSubWindow(control);
        m_subwindows.insert(control, mdi);

        QObject::connect(control, &ControlPanelWidget::closed,
                         this, &MainWindow::slotCloseSubWindow);
        QObject::connect(control, &ControlPanelWidget::error,
                         this, &MainWindow::slotOnError);

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        bool ok = control->initialize(dlg->address(), dlg->port());
        closeSubWindow(dlg);

        if (ok)
        {
            control->show();
        }
        QApplication::restoreOverrideCursor();
    }
}

void MainWindow::slotCloseSubWindow()
{
    closeSubWindow(qobject_cast<QWidget*>(sender()));
}

void MainWindow::closeSubWindow(QWidget* subwindow)
{
    if (   subwindow != nullptr
        && m_subwindows.contains(subwindow))
    {
        QMdiSubWindow* mdi = m_subwindows.take(subwindow);
        if (mdi != nullptr)
        {
            m_central->removeSubWindow(mdi);
            mdi->deleteLater();
        }
        subwindow->deleteLater();
    }
}

void MainWindow::slotOnError(const QString& message)
{
    QMessageBox::warning(this,
                         tr("Connection error"),
                         message);
    closeSubWindow(qobject_cast<QWidget*>(sender()));

    slotNewConnection();
}
