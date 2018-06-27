#ifndef UI_SUBWINDOW_H
#define UI_SUBWINDOW_H

#include <QWidget>

class QCloseEvent;

class SubWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SubWindow(QWidget* parent = nullptr);
    ~SubWindow() override;

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent* event) override;

};

#endif // UI_SUBWINDOW_H
