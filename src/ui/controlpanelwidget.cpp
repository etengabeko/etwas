#include "controlpanelwidget.h"
#include "ui_controlpanel.h"

#include <algorithm>

#include <QByteArray>
#include <QGridLayout>
#include <QHostAddress>
#include <QMovie>
#include <QScrollBar>
#include <QSharedPointer>
#include <QThread>

#include "ioservice/inputcontroller.h"
#include "ioservice/outputcontroller.h"
#include "ioservice/transport.h"
#include "logger/logger.h"
#include "protocol/protocol.h"
#include "protocol/types.h"
#include "settings/settings.h"
#include "ui/connectionoptionsdialog.h"
#include "ui/displaycontrolwidget.h"
#include "ui/displayoptionswidget.h"

namespace
{

int firmwareVersion() { return 0x02; }
int controlsGridColumnCount() { return 3; } // TODO
quint8 debugControlsCount() { return 9; }

QString titleString() { return qApp->tr("Device"); }

}

ControlPanelWidget::ControlPanelWidget(bool isDebugMode, QWidget* parent) :
    SubWindow(parent),
    m_ui(new Ui::ControlPanel()),
    m_isDebugMode(isDebugMode),
    m_recvThread(new QThread())
{
    m_ui->setupUi(this);

    m_ui->deviceIdentityFrame->hide();
    if (m_ui->workingPage->layout() == nullptr)
    {
        m_ui->workingPage->setLayout(new QGridLayout());
    }

    QMovie* preloadMovie = new QMovie(this);
    preloadMovie->setFileName(":/preload.gif");
    m_ui->preloadImageLabel->setMovie(preloadMovie);
    preloadMovie->start();

    QObject::connect(m_ui->clearButton, &QPushButton::clicked,
                     m_ui->logTextEdit, &QPlainTextEdit::clear);
    QObject::connect(m_ui->breakButton, &QPushButton::clicked,
                     this, &ControlPanelWidget::slotBreakInitialization);
    QObject::connect(m_ui->retryButton, &QPushButton::clicked,
                     this, &ControlPanelWidget::slotRetryInitialization);
    QObject::connect(m_ui->deviceIdentityButton, &QPushButton::clicked,
                     this, &ControlPanelWidget::slotSendDeviceIdentity);
}

ControlPanelWidget::~ControlPanelWidget()
{
    emit subwindowClosed(m_optionsWidget);

    m_recvThread->quit();
    m_recvThread->wait();

    delete m_recvThread;
    m_recvThread = nullptr;

    m_inCtrl.reset();
    m_outCtrl.reset();
    m_transport.reset();

    removeAllContols();

    delete m_ui;
    m_ui = nullptr;
}

void ControlPanelWidget::removeAllContols()
{
    QHashIterator<quint8, DisplayControlWidget*> it(m_controlWidgets);
    while (it.hasNext())
    {
        DisplayControlWidget* each = it.next().value();
        delete each;
    }

    m_controlWidgets.clear();
    m_controlIds.clear();
}

void ControlPanelWidget::initialize()
{
    using namespace ioservice;
    using protocol::MessageDirection;

    m_ui->stackedWidget->setCurrentWidget(m_ui->preloadPage);
    m_ui->preloadImageLabel->hide();
    m_ui->breakButton->hide();
    m_ui->retryButton->hide();

    ConnectionOptionsDialog dlg(this);
    if (dlg.exec() != ConnectionOptionsDialog::Accepted)
    {
        m_ui->statusLabel->setText(tr("Connection is not initialized"));
        m_ui->preloadImageLabel->hide();
        m_ui->retryButton->show();
        return;
    }

    m_ui->preloadImageLabel->show();
    m_ui->breakButton->show();
    m_ui->retryButton->hide();

    settings::Settings conf;
    conf.setAddress(dlg.address());
    conf.setPort(dlg.port());

    m_ui->statusLabel->setText(tr("Connecting to host %1:%2")
                               .arg(conf.address().toString())
                               .arg(conf.port()));

    m_transport.reset(new Transport(conf));
    m_transport->moveToThread(m_recvThread);

    m_outCtrl.reset(new OutputController(m_transport.get()));
    m_inCtrl.reset(new InputController(m_transport.get(),
                                       m_isDebugMode ? MessageDirection::Outcoming
                                                     : MessageDirection::Incoming));

    QObject::connect(m_transport.get(), &Transport::received,
                     this, &ControlPanelWidget::slotOnBytesReceive);
    QObject::connect(m_transport.get(), &Transport::sent,
                     this, &ControlPanelWidget::slotOnBytesSend);

    QObject::connect(m_inCtrl.get(), &InputController::messageReceived,
                     this, &ControlPanelWidget::slotReceiveMessage);

    QObject::connect(m_recvThread, &QThread::started,
                     m_transport.get(), &Transport::start);
    QObject::connect(m_transport.get(), &Transport::connected,
                     this, &ControlPanelWidget::slotOnConnect);
    QObject::connect(m_transport.get(), &Transport::disconnected,
                     this, &ControlPanelWidget::slotOnDisconnect);
    QObject::connect(m_transport.get(), &Transport::error,
                     this, &ControlPanelWidget::slotOnError);

    m_recvThread->start();
}

void ControlPanelWidget::slotBreakInitialization()
{
    // TODO
    Logger::instance().trace(tr("TODO: Break"));
}

void ControlPanelWidget::slotRetryInitialization()
{
    initialize();
}

void ControlPanelWidget::slotOnError()
{
    slotOnDisconnect();
}

void ControlPanelWidget::slotOnConnect()
{
    setWindowTitle(tr("[Connected] %1 - %2:%3")
                   .arg(::titleString())
                   .arg(m_transport->currentSettings().address().toString())
                   .arg(m_transport->currentSettings().port()));

    m_ui->stackedWidget->setCurrentWidget(m_ui->preloadPage);
    m_ui->statusLabel->setText(tr("Waiting device's configuration"));
    m_ui->preloadImageLabel->show();
    m_ui->breakButton->show();
    m_ui->retryButton->hide();

    if (m_isDebugMode)
    {
        makeDebugConfiguration(::debugControlsCount());
    }
}

void ControlPanelWidget::makeDebugConfiguration(int buttonsCount)
{
    Q_ASSERT(buttonsCount > 0);

    m_ui->deviceIdentityFrame->show();

    m_controlIds.reserve(buttonsCount);
    for (quint8 i = 0; i < buttonsCount; ++i)
    {
        m_controlIds.append(i);
    }

    createControls();
}

void ControlPanelWidget::makeConfiguration(const protocol::incoming::Message& message)
{
    using protocol::incoming::DeviceIdentityMessage;

    const DeviceIdentityMessage& dimsg = dynamic_cast<const DeviceIdentityMessage&>(message);

    m_controlIds.reserve(dimsg.buttonsNumbers().size());

    for (const quint8 each : dimsg.buttonsNumbers())
    {
        m_controlIds.append(each);
    }

    createControls();
}

void ControlPanelWidget::createControls()
{
    int row = 0;
    int column = 0;

    for (auto it = m_controlIds.cbegin(), end = m_controlIds.cend(); it != end; ++it)
    {
        createControl(*it, row, column);

        if (++column >= ::controlsGridColumnCount())
        {
            ++row;
            column = 0;
        }
    }
    m_ui->stackedWidget->setCurrentWidget(m_ui->workingPage);
}


void ControlPanelWidget::createControl(quint8 controlId, int row, int column)
{
    if (!m_controlWidgets.contains(controlId))
    {
        DisplayControlWidget* wgt = new DisplayControlWidget(m_isDebugMode);
        m_controlWidgets.insert(controlId, wgt);
        initConnectionsForControl(wgt);

        QGridLayout* lout = qobject_cast<QGridLayout*>(m_ui->workingPage->layout());
        if (lout != nullptr)
        {
            lout->addWidget(wgt, row, column);
        }
    }
}

void ControlPanelWidget::initConnectionsForControl(DisplayControlWidget* control)
{
    Q_CHECK_PTR(control);

    if (!m_isDebugMode)
    {
        QObject::connect(control, &DisplayControlWidget::activated,
                         this, &ControlPanelWidget::slotChangeActiveControl);
    }
    else
    {
        QObject::connect(control, &DisplayControlWidget::activated,
                         this, &ControlPanelWidget::slotChangeButtonsState);
    }
}

void ControlPanelWidget::slotChangeActiveControl(bool enabled)
{
    if (enabled)
    {
        if (m_activeControl != nullptr)
        {
            m_activeControl->setActive(false);
        }
        m_activeControl = qobject_cast<DisplayControlWidget*>(sender());
    }
    else
    {
        m_activeControl = nullptr;
    }

    if (enabled && m_activeControl != nullptr)
    {
        if (m_optionsWidget == nullptr)
        {
            m_optionsWidget = new DisplayOptionsWidget();
            emit subwindowCreated(m_optionsWidget);

            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::closed,
                             this, &ControlPanelWidget::slotOptionsClose);
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::imageFirstEnabled,
                             this, static_cast<void(ControlPanelWidget::*)(bool)>(&ControlPanelWidget::slotActiveControlImageFirstChange));
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::imageSecondEnabled,
                             this, static_cast<void(ControlPanelWidget::*)(bool)>(&ControlPanelWidget::slotActiveControlImageSecondChange));
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::blinkingEnabled,
                             this, &ControlPanelWidget::slotActiveControlBlinkingChange);
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::timeOnChanged,
                             this, &ControlPanelWidget::slotActiveControlTimeOnChange);
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::timeOffChanged,
                             this, &ControlPanelWidget::slotActiveControlTimeOffChange);
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::brightChanged,
                             this, &ControlPanelWidget::slotActiveControlBrightChange);
        }

        m_optionsWidget->setFirstImage(m_activeControl->firstImage());
        m_optionsWidget->setFirstImageEnabled(m_activeControl->isFirstImageEnabled());
        m_optionsWidget->setSecondImage(m_activeControl->secondImage());
        m_optionsWidget->setSecondImageEnabled(m_activeControl->isSecondImageEnabled());
        m_optionsWidget->setBlinkingEnabled(m_activeControl->isBlinkingEnabled());
        m_optionsWidget->setTimeOn(m_activeControl->timeOn());
        m_optionsWidget->setTimeOff(m_activeControl->timeOff());
        m_optionsWidget->setBrightLevel(m_activeControl->brightLevel());
    }

    if (enabled)
    {
        m_optionsWidget->show();
    }
    else
    {
        emit subwindowClosed(m_optionsWidget);
        m_optionsWidget = nullptr;
    }
}

void ControlPanelWidget::slotOptionsClose()
{
    if (m_activeControl != nullptr)
    {
        m_activeControl->setActive(false);
        m_activeControl = nullptr;
    }
}

void ControlPanelWidget::slotChangeButtonsState(bool enabled)
{
    Q_UNUSED(enabled);

    using protocol::ButtonState;
    using protocol::incoming::ButtonsStateMessage;

    QVector<ButtonState> states;
    states.reserve(m_controlIds.size());

    for (quint8 each : m_controlIds)
    {
        DisplayControlWidget* control = m_controlWidgets[each];
        states.append(control->isActive() ? ButtonState::On
                                          : ButtonState::Off);
    }
    ButtonsStateMessage* msg = new ButtonsStateMessage();
    msg->setButtonsStates(std::move(states));

    slotSendMessage(QSharedPointer<protocol::AbstractMessage>(msg));
}

void ControlPanelWidget::slotSendDeviceIdentity()
{
    using protocol::incoming::DeviceIdentityMessage;

    DeviceIdentityMessage* msg = new DeviceIdentityMessage();
    msg->setFirmwareVersion(static_cast<quint8>(::firmwareVersion()));
    msg->setButtonsNumbers(m_controlIds);

    slotSendMessage(QSharedPointer<protocol::AbstractMessage>(msg));
}

void ControlPanelWidget::slotOnDisconnect()
{
    m_recvThread->quit();
    m_recvThread->wait();

    if (m_optionsWidget != nullptr)
    {
        m_optionsWidget->hide();
    }

    if (m_isDebugMode)
    {
        m_ui->deviceIdentityFrame->hide();
    }

    removeAllContols();

    setWindowTitle(tr("[Disconneted] %1 - %2:%3")
                   .arg(::titleString())
                   .arg(m_transport->currentSettings().address().toString())
                   .arg(m_transport->currentSettings().port()));

    m_ui->stackedWidget->setCurrentWidget(m_ui->preloadPage);
    m_ui->statusLabel->setText(tr("Error connection: %1")
                               .arg(m_transport->errorString()));
    m_ui->preloadImageLabel->hide();
    m_ui->breakButton->hide();
    m_ui->retryButton->show();
}

void ControlPanelWidget::slotOnBytesReceive(const QByteArray& bytes)
{
    const QString str = tr("Received bytes size = %1: %2")
            .arg(bytes.size())
            .arg(QString::fromLatin1(bytes.toHex()));

    QString log = m_ui->logTextEdit->toPlainText();
    if (!log.isEmpty())
    {
        log += "\n";
    }
    log += str;
    m_ui->logTextEdit->setPlainText(log);
    m_ui->logTextEdit->verticalScrollBar()->setValue(m_ui->logTextEdit->verticalScrollBar()->maximum());
}

void ControlPanelWidget::slotOnBytesSend(const QByteArray& bytes)
{
    const QString str = tr("Sent bytes size = %1: %2")
            .arg(bytes.size())
            .arg(QString::fromLatin1(bytes.toHex()));

    QString log = m_ui->logTextEdit->toPlainText();
    if (!log.isEmpty())
    {
        log += "\n";
    }
    log += str;
    m_ui->logTextEdit->setPlainText(log);
    m_ui->logTextEdit->verticalScrollBar()->setValue(m_ui->logTextEdit->verticalScrollBar()->maximum());
}

void ControlPanelWidget::slotSendMessage(const QSharedPointer<protocol::AbstractMessage>& message)
{
    if (message != nullptr)
    {
        m_outCtrl->slotSend(*message);
    }
}

void ControlPanelWidget::slotReceiveMessage(const QSharedPointer<protocol::AbstractMessage>& message)
{
    Q_CHECK_PTR(message);

    using protocol::MessageDirection;

    switch (message->direction())
    {
    case MessageDirection::Incoming:
        if (!m_isDebugMode)
        {
            processMessage(dynamic_cast<const protocol::incoming::Message&>(*message));
        }
        break;
    case MessageDirection::Outcoming:
        if (m_isDebugMode)
        {
            processMessage(dynamic_cast<const protocol::outcoming::Message&>(*message));
        }
        break;
    default:
        Q_ASSERT_X(false, "ControlPanelWidget::slotReceiveMessage", "Unexpected MessageDirection");
        break;
    }
}

void ControlPanelWidget::processMessage(const protocol::incoming::Message& message)
{
    using namespace protocol::incoming;

    switch (message.type())
    {
    case MessageType::DeviceIdentity:
        removeAllContols();
        makeConfiguration(message);
        break;
    case MessageType::ButtonsState:
        applyButtonsStates(dynamic_cast<const ButtonsStateMessage&>(message).buttonsStates());
        break;
    default:
        Q_ASSERT_X(false, "ControlPanelWidget::processMessage", "Unexpected incoming MessageType");
        break;
    }
}

void ControlPanelWidget::applyButtonsStates(const QVector<protocol::ButtonState>& states)
{
    using protocol::ButtonState;

    QVectorIterator<ButtonState> stateIt(states);
    for (auto it = m_controlIds.cbegin(), end = m_controlIds.cend(); it != end; ++it)
    {
        m_controlWidgets[*it]->highlight(stateIt.next() == ButtonState::On);
    }
}

void ControlPanelWidget::processMessage(const protocol::outcoming::Message& message)
{
    using namespace protocol::outcoming;

    switch (message.type())
    {
    case MessageType::BlinkOptions:
        {
            const BlinkOptionsMessage& blomsg = dynamic_cast<const BlinkOptionsMessage&>(message);
            QVector<quint8>::const_iterator founded = std::find(m_controlIds.cbegin(), m_controlIds.cend(), blomsg.displayNumber());
            if (founded != m_controlIds.cend())
            {
                DisplayControlWidget* control = m_controlWidgets[*founded];
                control->setTimeOn(10 * blomsg.timeOn());
                control->setTimeOff(10 * blomsg.timeOff());
            }
        }
        break;
    case MessageType::BrightOptions:
        {
            const BrightOptionsMessage& bromsg = dynamic_cast<const BrightOptionsMessage&>(message);
            QVector<quint8>::const_iterator founded = std::find(m_controlIds.cbegin(), m_controlIds.cend(), bromsg.displayNumber());
            if (founded != m_controlIds.cend())
            {
                DisplayControlWidget* control = m_controlWidgets[*founded];
                control->setBrightLevel(bromsg.brightLevel());
            }
        }
        break;
    case MessageType::DisplayImages:
        {
        // TODO: image num -> image filename
        }
        break;
    case MessageType::DisplayOptions:
        {
            const DisplayOptionsMessage& domsg = dynamic_cast<const DisplayOptionsMessage&>(message);
            QVector<quint8>::const_iterator founded = std::find(m_controlIds.cbegin(), m_controlIds.cend(), domsg.displayNumber());
            if (founded != m_controlIds.cend())
            {
                DisplayControlWidget* control = m_controlWidgets[*founded];
                control->setBlinkingEnabled(domsg.blinkState() == protocol::BlinkState::On);
                switch (domsg.imageSelection())
                {
                case protocol::ImageSelection::Nothing:
                    control->resetFirstImage();
                    control->resetSecondImage();
                    break;
                case protocol::ImageSelection::First:
                    control->setFirstImage(control->firstImage());
                    control->resetSecondImage();
                    break;
                case protocol::ImageSelection::Second:
                    control->resetFirstImage();
                    control->setSecondImage(control->secondImage());
                    break;
                case protocol::ImageSelection::Both:
                    control->setFirstImage(control->firstImage());
                    control->setSecondImage(control->secondImage());
                    break;
                default:
                    break;
                }
            }
        }
        break;
    case MessageType::DeviceAddress:
    case MessageType::ImagesData:
        qDebug().noquote() << "TODO: process message type" << static_cast<quint8>(message.type());
        break;
    default:
        Q_ASSERT_X(false, "ControlPanelWidget::processMessage", "Unexpected outcoming MessageType");
        break;
    }
}

void ControlPanelWidget::slotActiveControlImageFirstChange(bool enabled)
{
    Q_CHECK_PTR(m_activeControl);

    if (enabled)
        m_activeControl->setFirstImage(m_activeControl->firstImage());
    else
        m_activeControl->resetFirstImage();
}

void ControlPanelWidget::slotActiveControlImageSecondChange(bool enabled)
{
    Q_CHECK_PTR(m_activeControl);

    if (enabled)
        m_activeControl->setSecondImage(m_activeControl->secondImage());
    else
        m_activeControl->resetSecondImage();
}

void ControlPanelWidget::slotActiveControlBlinkingChange(bool enabled)
{
    Q_CHECK_PTR(m_activeControl);

    m_activeControl->setBlinkingEnabled(enabled);
}

void ControlPanelWidget::slotActiveControlTimeOnChange(int msec)
{
    Q_CHECK_PTR(m_activeControl);

    m_activeControl->setTimeOn(msec);
}

void ControlPanelWidget::slotActiveControlTimeOffChange(int msec)
{
    Q_CHECK_PTR(m_activeControl);

    m_activeControl->setTimeOff(msec);
}

void ControlPanelWidget::slotActiveControlBrightChange(int level)
{
    Q_CHECK_PTR(m_activeControl);

    m_activeControl->setBrightLevel(level);
}
