#ifndef UI_CONTROL_PANEL_WIDGET_H
#define UI_CONTROL_PANEL_WIDGET_H

#include <memory>

#include <QWidget>

class QByteArray;
class QCloseEvent;
class QHostAddress;
template <typename T> class QSharedPointer;

namespace ioservice
{
class InputController;
class OutputController;
class Transport;
} // ioservice

namespace protocol
{
class AbstractMessage;
} // protocol

namespace Ui
{
class ControlPanel;
} // Ui

class ControlPanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanelWidget(bool isDebugMode = false, QWidget* parent = nullptr);
    ~ControlPanelWidget();

    bool initialize(const QHostAddress& address, quint16 port);

signals:
    void closed();
    void error(const QString& message);

private slots:
    void slotChangeReceiveMessagesType(bool checked);

    void slotReceiveBytes(const QByteArray& bytes);
    void slotReceiveMessage(const QSharedPointer<protocol::AbstractMessage>& message);

    void slotSendMessage(const QSharedPointer<protocol::AbstractMessage>& message);

private:
    void closeEvent(QCloseEvent* event);

private:
    Ui::ControlPanel* m_ui = nullptr;
    const bool m_isDebugMode;

    std::unique_ptr<ioservice::Transport> m_transport;
    std::unique_ptr<ioservice::InputController> m_inCtrl;
    std::unique_ptr<ioservice::OutputController> m_outCtrl;

};

#endif // UI_CONTROL_PANEL_WIDGET_H
