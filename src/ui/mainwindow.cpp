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
#include "ui/subwindow.h"

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
        slotRemoveSubWindow(m_subwindows.begin().key());
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
    ControlPanelWidget* control = new ControlPanelWidget((mode == Mode::Debug),
                                                         QString::null, // TODO
                                                         this);
    slotAddSubWindow(control);

    QObject::connect(control, &SubWindow::closed,
                     this, &MainWindow::slotCloseSubWindow);
    QObject::connect(control, &ControlPanelWidget::subwindowCreated,
                     this, &MainWindow::slotAddSubWindow);
    QObject::connect(control, &ControlPanelWidget::subwindowClosed,
                     this, &MainWindow::slotRemoveSubWindow);

    control->show();
    control->initialize();
}

void MainWindow::slotAddSubWindow(SubWindow* subwindow)
{
    if (   subwindow != nullptr
        && !m_subwindows.contains(subwindow))
    {
        QMdiSubWindow* mdi = m_central->addSubWindow(subwindow);
        m_subwindows.insert(subwindow, mdi);
    }
}

void MainWindow::slotCloseSubWindow()
{
    slotRemoveSubWindow(qobject_cast<SubWindow*>(sender()));
}

void MainWindow::slotRemoveSubWindow(SubWindow* subwindow)
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
    }
}
