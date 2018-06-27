#ifndef UI_CONTROL_PANEL_WIDGET_H
#define UI_CONTROL_PANEL_WIDGET_H

#include <memory>

#include <QHash>
#include <QVector>
#include <QWidget>

class QByteArray;
class QCloseEvent;
class QThread;
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
enum class ButtonState;

namespace incoming  { class Message; }
namespace outcoming { class Message; }
} // protocol

class DisplayControlWidget;
class DisplayOptionsWidget;

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

    void initialize();

signals:
    void closed();

private slots:
    void slotBreakInitialization();
    void slotRetryInitialization();

    void slotOnConnect();
    void slotOnDisconnect();
    void slotOnError();

    void slotOnBytesReceive(const QByteArray& bytes);
    void slotOnBytesSend(const QByteArray& bytes);

    void slotReceiveMessage(const QSharedPointer<protocol::AbstractMessage>& message);
    void slotSendMessage(const QSharedPointer<protocol::AbstractMessage>& message);

    void slotChangeActiveControl(bool enabled);
    void slotOptionsClose();

    void slotSendDeviceIdentity();
    void slotChangeButtonsState(bool enabled);

private:
    void closeEvent(QCloseEvent* event);

    void makeDebugConfiguration(int buttonsCount);
    void makeConfiguration(const protocol::incoming::Message& message);

    void createControls();
    void createControl(quint8 controlId, int row, int column);
    void initConnectionsForControl(DisplayControlWidget* control);
    void removeAllContols();

    void processMessage(const protocol::incoming::Message& message);
    void processMessage(const protocol::outcoming::Message& message);

    void applyButtonsStates(const QVector<protocol::ButtonState>& states);

private:
    Ui::ControlPanel* m_ui = nullptr;
    const bool m_isDebugMode;

    QThread* m_recvThread = nullptr;

    std::unique_ptr<ioservice::Transport> m_transport;
    std::unique_ptr<ioservice::InputController> m_inCtrl;
    std::unique_ptr<ioservice::OutputController> m_outCtrl;

    QVector<quint8> m_controlIds;
    QHash<quint8, DisplayControlWidget*> m_controlWidgets;

    DisplayOptionsWidget* m_optionsWidget = nullptr;
    DisplayControlWidget* m_activeControl = nullptr;

};

#endif // UI_CONTROL_PANEL_WIDGET_H
