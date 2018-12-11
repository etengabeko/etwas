#ifndef UI_CONTROL_PANEL_WIDGET_H
#define UI_CONTROL_PANEL_WIDGET_H

#include <memory>

#include <QHash>
#include <QString>
#include <QVector>

#include <ui/subwindow.h>

class QByteArray;
class QCloseEvent;
class QStatusBar;
class QThread;
class QTimer;
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
enum class MessageDirection;

namespace incoming  { class Message; }
namespace outcoming { class Message; }
} // protocol

namespace settings
{
class DisplaySettings;
} // settings

namespace storage
{
class ImageStorage;
} // storage

class Logger;
class DisplayControlWidget;
class DisplayOptionsWidget;
class ImageStorageWidget;

namespace Ui
{
class ControlPanel;
} // Ui

class ControlPanelWidget : public SubWindow
{
    Q_OBJECT

public:
    explicit ControlPanelWidget(bool isDebugMode,
                                QWidget* parent = nullptr);
    ~ControlPanelWidget();

    void initialize();

    void setVerbose(bool enabled);

signals:
    void subwindowCreated(SubWindow* child);
    void subwindowRaised(SubWindow* child);
    void subwindowClosed(SubWindow* child);

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
    void slotApplySelectedImage(quint8 imageIndex);
    void slotOptionsClose();
    void slotImagesClose();

    void slotSendPing();
    void slotSendDeviceIdentity();
    void slotSendImagesData();
    void slotChangeDeviceAddress();
    void slotChangeButtonsState(bool enabled);

    void slotActiveControlImageFirstChange(bool enabled);
    void slotActiveControlImageSecondChange(bool enabled);
    void slotActiveControlImageFirstChange(int imageIndex);
    void slotActiveControlImageSecondChange(int imageIndex);
    void slotActiveControlBlinkingChange(bool enabled);
    void slotActiveControlTimeOnChange(int msec);
    void slotActiveControlTimeOffChange(int msec);
    void slotActiveControlBrightChange(int level);
    void slotActiveControlImageFirstSelect();
    void slotActiveControlImageSecondSelect();

    void slotLoadImages();

    void slotChangeControlsColumnsCount(int count);

    void slotCloseConnection();

    void slotLoadConfiguration();
    void slotSaveConfiguration();

private:
    void makeDebugConfiguration(int buttonsCount);
    void makeConfiguration(const protocol::incoming::Message& message);
    void makeConfiguration(const QVector<settings::DisplaySettings>& displays);

    void createControls();
    void createControl(quint8 controlId);
    void initConnectionsForControl(DisplayControlWidget* control);
    void removeAllContols();

    void showImagesWidget();

    void processMessage(const protocol::incoming::Message& message);
    void processMessage(const protocol::outcoming::Message& message);

    QString createReceivedBytesLogMessage(const QByteArray& bytes,
                                          bool isPartial = false) const;
    QString createSentBytesLogMessage(const QByteArray& bytes,
                                      bool isPartial = false) const;

    void logMessage(const QString& message, protocol::MessageDirection direction);

    void applyButtonsStates(const QVector<protocol::ButtonState>& states);

    void createDisplayOptionsMessage();
    void createDisplayImagesMessage();
    void createBlinkOptionsMessage();
    void createBrightOptionsMessage();
    quint8 findActiveControlId(bool* ok) const;

    void stopTransport();
    void setNotConnectedState();

    void showPingCounterMessage();

    void closeEvent(QCloseEvent* event);

private:
    Ui::ControlPanel* m_ui = nullptr;
    const bool m_isDebugMode;
    bool m_isVerbose = false;

    QThread* m_recvThread = nullptr;
    std::unique_ptr<Logger> m_logger;

    QString m_currentAddress;
    quint16 m_currentPort = 0;

    ioservice::Transport* m_transport = nullptr;
    std::unique_ptr<ioservice::InputController> m_inCtrl;
    std::unique_ptr<ioservice::OutputController> m_outCtrl;

    std::unique_ptr<storage::ImageStorage> m_imgStorage;

    QVector<quint8> m_controlIds;
    QHash<int, DisplayControlWidget*> m_controlWidgets;

    DisplayOptionsWidget* m_optionsWidget = nullptr;
    DisplayControlWidget* m_activeControl = nullptr;
    ImageStorageWidget*   m_imagesWidget  = nullptr;

    enum class SelectedImage
    {
        Unset,
        First,
        Second
    };

    SelectedImage m_lastSelected = SelectedImage::Unset;

    quint64 m_pingCounter = 0;
    QTimer* m_pingTimer = nullptr;
    QStatusBar* m_statusBar = nullptr;

};

#endif // UI_CONTROL_PANEL_WIDGET_H
