#include "subwindow.h"

#include <QCloseEvent>

SubWindow::SubWindow(QWidget* parent) :
    QWidget(parent)
{

}

SubWindow::~SubWindow()
{

}

void SubWindow::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    emit closed();
}
