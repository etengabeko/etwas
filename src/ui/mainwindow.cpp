#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QCursor>
#include <QDialog>
#include <QFile>
#include <QLabel>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QString>
#include <QTextEdit>
#include <QVBoxLayout>

#include "ui/controlpanelwidget.h"
#include "ui/subwindow.h"

namespace
{

QString customAboutApp()
{
    QFile f(":/app_about.html");
    const bool ok = f.open(QFile::ReadOnly);
    Q_ASSERT(ok);

    return (ok ? QString::fromUtf8(f.readAll())
               : QString::null);
}

}

MainWindow::MainWindow(Mode mode, QWidget* parent) :
    QMainWindow(parent),
    m_central(new QMdiArea(this))
{
    setWindowTitle(QApplication::applicationName());
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

void MainWindow::enableVerboseMode()
{
    m_verboseMode = true;
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

    QMenu* helpMenu = menuBar()->addMenu(tr("Help"));

    action = helpMenu->addAction(tr("About application..."));
    QObject::connect(action, &QAction::triggered,
                     this, &MainWindow::slotShowAbout);
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
    control->setVerbose(m_verboseMode);
    slotAddSubWindow(control);

    QObject::connect(control, &SubWindow::closed,
                     this, &MainWindow::slotCloseSubWindow);
    QObject::connect(control, &ControlPanelWidget::subwindowCreated,
                     this, &MainWindow::slotAddSubWindow);
    QObject::connect(control, &ControlPanelWidget::subwindowRaised,
                     this, &MainWindow::slotRaiseSubWindow);
    QObject::connect(control, &ControlPanelWidget::subwindowClosed,
                     this, &MainWindow::slotRemoveSubWindow);

    control->show();
    control->initialize();
}

void MainWindow::slotShowAbout()
{
    QDialog dlg;

    QLabel* appNameLabel = new QLabel(tr("Application: %1")
                                      .arg(QApplication::applicationName()),
                                      &dlg);
    QLabel* appVersionLabel = new QLabel(tr("Version: %1")
                                         .arg(QApplication::applicationVersion()),
                                         &dlg);
    QTextEdit* appAboutTextEdit = new QTextEdit(&dlg);
    appAboutTextEdit->setReadOnly(true);
    appAboutTextEdit->setHtml(::customAboutApp());

    dlg.setLayout(new QVBoxLayout(&dlg));
    dlg.layout()->addWidget(appNameLabel);
    dlg.layout()->addWidget(appVersionLabel);
    dlg.layout()->addWidget(appAboutTextEdit);

    dlg.exec();
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

void MainWindow::slotRaiseSubWindow(SubWindow* subwindow)
{
    if (   subwindow != nullptr
        && m_subwindows.contains(subwindow))
    {
        QMdiSubWindow* mdi = m_subwindows[subwindow];
        m_central->setActiveSubWindow(mdi);
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
