#ifndef UI_DEVICE_LOG_WIDGET_H
#define UI_DEVICE_LOG_WIDGET_H

#include <ui/subwindow.h>

class QByteArray;

namespace Ui
{
class DeviceLog;
} // Ui

class DeviceLogWidget : public SubWindow
{
    Q_OBJECT

public:
    explicit DeviceLogWidget(QWidget* parent = nullptr);
    ~DeviceLogWidget();

public slots:
    void slotBeginReceiving(quint32 count);
    void slotReceiveMessage(const QByteArray& message);
    void slotEndReceiving();

private slots:
    void slotSave();

private:
    void showProgress(quint32 count);
    void hideProgress();
    void setProgressVisible(bool enabled);

private:
    Ui::DeviceLog* m_ui = nullptr;

};

#endif // UI_DEVICE_LOG_WIDGET_H
