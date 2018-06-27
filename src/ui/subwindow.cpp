#include "subwindow.h"

#include <QCloseEvent>
#include <QDebug> // FIXME

SubWindow::SubWindow(QWidget* parent) :
    QWidget(parent)
{

}

SubWindow::~SubWindow()
{
    qDebug() << "deleted";
}

void SubWindow::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    emit closed();
}
