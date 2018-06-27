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

#include "ui/controlpanelwidget.h"

MainWindow::MainWindow(Mode mode, QWidget* parent) :
    QMainWindow(parent),
    m_central(new QMdiArea(this))
{
    setWindowTitle(tr("FZK-2 Test"));
    setCentralWidget(m_central);

    initMenu(mode);
}

MainWindow::~MainWindow()
{
    while (!m_subwindows.isEmpty())
    {
        closeSubWindow(m_subwindows.begin().key());
    }
}

void MainWindow::initMenu(Mode mode)
{
    QMenu* fileMenu = menuBar()->addMenu(tr("File"));

    QAction* action = fileMenu->addAction(tr("New connection"));
    QObject::connect(action, &QAction::triggered,
                     this, &MainWindow::slotNewControlPanel);

    if (mode == Mode::Debug)
    {
        action = fileMenu->addAction(tr("Debug tools"));
        QObject::connect(action, &QAction::triggered,
                         this, &MainWindow::slotNewControlPanelDebug);
    }

    action = fileMenu->addAction(tr("Quit"));
    QObject::connect(action, &QAction::triggered,
                     this, &MainWindow::close);
}

void MainWindow::slotNewControlPanel()
{
    createNewControlPanel(Mode::Work);
}

void MainWindow::slotNewControlPanelDebug()
{
    createNewControlPanel(Mode::Debug);
}

void MainWindow::createNewControlPanel(MainWindow::Mode mode)
{
    ControlPanelWidget* control = new ControlPanelWidget((mode == Mode::Debug), this);
    QMdiSubWindow* mdi = m_central->addSubWindow(control);
    m_subwindows.insert(control, mdi);

    QObject::connect(control, &ControlPanelWidget::closed,
                     this, &MainWindow::slotCloseSubWindow);

    control->show();
    control->initialize();
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
