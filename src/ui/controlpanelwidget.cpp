#include "controlpanelwidget.h"
#include "ui_controlpanel.h"

#include <algorithm>

#include <QAction>
#include <QByteArray>
#include <QCloseEvent>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QMenu>
#include <QMenuBar>
#include <QMovie>
#include <QScrollBar>
#include <QSharedPointer>
#include <QStatusBar>
#include <QTemporaryFile>
#include <QThread>
#include <QTimer>

#include "ioservice/inputcontroller.h"
#include "ioservice/outputcontroller.h"
#include "ioservice/transport.h"
#include "logger/logger.h"
#include "protocol/protocol.h"
#include "protocol/types.h"
#include "settings/displaysettings.h"
#include "settings/settings.h"
#include "storage/imagestorage.h"
#include "ui/connectionoptionsdialog.h"
#include "ui/displaycontrolwidget.h"
#include "ui/displayoptionswidget.h"
#include "ui/imagestoragewidget.h"

namespace
{

QString defaultHostAddress() { return "192.168.0.10"; }
quint16 defaultPort() { return 7; }
quint8 defaultBrightLevel() { return 15; }
quint16 defaultTimeout() { return 1000; }

int defaultPingTimeoutMsecs() { return 1000; }

int firmwareVersion() { return 0x02; }
const QSize& buttonImageSize()
{
    static const QSize size(64, 48);
    return size;
}

quint8 commonControlsCount() { return 16; }
quint8 debugControlsCount()  { return 19; }

QString titleString() { return QApplication::translate("ControlPanelWidget", "Device"); }

Logger::Level customLogLevel() { return Logger::Level::Debug; }
const QString customLogFileName()
{
    return QFileInfo(QDir::temp(),
                     QString("%1.log").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")))
            .absoluteFilePath();
}

}

ControlPanelWidget::ControlPanelWidget(bool isDebugMode,
                                       QWidget* parent) :
    SubWindow(parent),
    m_ui(new Ui::ControlPanel()),
    m_isDebugMode(isDebugMode),
    m_recvThread(new QThread()),
    m_currentAddress(::defaultHostAddress()),
    m_currentPort(::defaultPort()),
    m_imgStorage(new storage::ImageStorage()),
    m_statusBar(new QStatusBar())
{
    m_ui->setupUi(this);

    layout()->addWidget(m_statusBar);
    showPingCounterMessage();

    m_ui->deviceAddressButton->hide();
    m_ui->deviceIdentityButton->hide();
    m_ui->logFileNameLineEdit->hide();
    m_ui->logFileNameLabel->hide();
    m_ui->columnCountLabel->hide();
    m_ui->columnCountSpinBox->hide();

    QMovie* preloadMovie = new QMovie(this);
    preloadMovie->setFileName(":/preload.gif");
    m_ui->preloadImageLabel->setMovie(preloadMovie);
    preloadMovie->start();

    QObject::connect(m_ui->incomingClearButton, &QPushButton::clicked,
                     m_ui->incomingLogTextEdit, &QPlainTextEdit::clear);
    QObject::connect(m_ui->outcomingClearButton, &QPushButton::clicked,
                     m_ui->outcomingLogTextEdit, &QPlainTextEdit::clear);
    QObject::connect(m_ui->breakButton, &QPushButton::clicked,
                     this, &ControlPanelWidget::slotBreakInitialization);
    QObject::connect(m_ui->retryButton, &QPushButton::clicked,
                     this, &ControlPanelWidget::slotRetryInitialization);
    QObject::connect(m_ui->deviceIdentityButton, &QPushButton::clicked,
                     this, &ControlPanelWidget::slotSendDeviceIdentity);
    QObject::connect(m_ui->deviceAddressButton, &QPushButton::clicked,
                     this, &ControlPanelWidget::slotChangeDeviceAddress);
    QObject::connect(m_ui->columnCountSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &ControlPanelWidget::slotChangeControlsColumnsCount);

    if (!m_isDebugMode)
    {
        QMenuBar* menubar = new QMenuBar(this);
        layout()->setMenuBar(menubar);

        QMenu* fileMenu = menubar->addMenu(tr("File"));
        QAction* disconnectAction = fileMenu->addAction(tr("Disconnect"));
        QObject::connect(disconnectAction, &QAction::triggered,
                         this, &ControlPanelWidget::slotCloseConnection);

        QMenu* editMenu = menubar->addMenu(tr("Edit"));

        QAction* loadAction = editMenu->addAction(tr("Load configuration"));
        QObject::connect(loadAction, &QAction::triggered,
                         this, &ControlPanelWidget::slotLoadConfiguration);
        QAction* saveAction = editMenu->addAction(tr("Save configuration"));
        QObject::connect(saveAction, &QAction::triggered,
                         this, &ControlPanelWidget::slotSaveConfiguration);

        QMenu* optionsMenu = menubar->addMenu(tr("Options"));

        QAction* optionsAction = optionsMenu->addAction(tr("Connection options"));
        QObject::connect(optionsAction, &QAction::triggered,
                         this, &ControlPanelWidget::slotChangeDeviceAddress);
        QAction* imagesAction = optionsMenu->addAction(tr("Load images"));
        QObject::connect(imagesAction, &QAction::triggered,
                         this, &ControlPanelWidget::slotLoadImages);
    }
    else
    {
        m_pingTimer = new QTimer(this);
        QObject::connect(m_pingTimer, &QTimer::timeout,
                         this, &ControlPanelWidget::slotSendPing);
        m_pingTimer->setInterval(::defaultPingTimeoutMsecs());
    }
}

ControlPanelWidget::~ControlPanelWidget()
{
    delete m_pingTimer;
    m_pingTimer = nullptr;

    emit subwindowClosed(m_optionsWidget);
    emit subwindowClosed(m_imagesWidget);

    m_recvThread->quit();
    m_transport = nullptr;
    m_recvThread->wait();

    delete m_recvThread;
    m_recvThread = nullptr;

    m_inCtrl.reset();
    m_outCtrl.reset();

    removeAllContols();

    delete m_statusBar;
    m_statusBar = nullptr;

    delete m_ui;
    m_ui = nullptr;
}

void ControlPanelWidget::closeEvent(QCloseEvent* event)
{
    stopTransport();
    SubWindow::closeEvent(event);
}

void ControlPanelWidget::stopTransport()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (m_transport != nullptr)
    {
        const int kMinDelayMsec = 1;
        QTimer::singleShot(kMinDelayMsec, m_transport, &ioservice::Transport::stop);
        QThread::msleep(10* kMinDelayMsec);
    }

    QApplication::processEvents();

    QApplication::restoreOverrideCursor();
}

void ControlPanelWidget::removeAllContols()
{
    QHashIterator<int, DisplayControlWidget*> it(m_controlWidgets);
    while (it.hasNext())
    {
        DisplayControlWidget* each = it.next().value();
        delete each;
    }
    delete m_ui->commonGroupBox->layout();
    delete m_ui->additionalGroupBox->layout();

    m_controlWidgets.clear();
    m_controlIds.clear();
    m_activeControl = nullptr;
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
    if (!m_currentAddress.isEmpty())
    {
        dlg.setAddress(QHostAddress(m_currentAddress));
    }
    if (m_currentPort > 0)
    {
        dlg.setPort(m_currentPort);
    }
    QString logFileName = (!m_ui->logFileNameLineEdit->text().isEmpty() ? m_ui->logFileNameLineEdit->text()
                                                                        : ::customLogFileName());
    dlg.setLogFileName(logFileName);

    if (dlg.exec() != ConnectionOptionsDialog::Accepted)
    {
        m_ui->statusLabel->setText(tr("Connection is not initialized"));
        m_ui->preloadImageLabel->hide();
        m_ui->retryButton->show();
        m_ui->logFileNameLineEdit->hide();
        m_ui->logFileNameLabel->hide();
        return;
    }

    m_ui->preloadImageLabel->show();
    m_ui->breakButton->show();
    m_ui->retryButton->hide();

    m_currentAddress = dlg.address().toString();
    m_currentPort = dlg.port();

    logFileName = dlg.logFileName();
    m_ui->logFileNameLineEdit->setText(logFileName);
    if (!logFileName.isEmpty())
    {
        m_logger.reset(new Logger(::customLogLevel(),
                                  Logger::Device::File,
                                  logFileName));
        m_ui->logFileNameLineEdit->show();
        m_ui->logFileNameLabel->show();
    }
    else
    {
        m_logger.reset(new Logger(::customLogLevel()));
        m_ui->logFileNameLineEdit->hide();
        m_ui->logFileNameLabel->hide();
    }

    m_transport = new Transport(dlg.address(), dlg.port());
    m_transport->moveToThread(m_recvThread);

    m_outCtrl.reset(new OutputController(m_transport));
    m_inCtrl.reset(new InputController(m_transport,
                                       m_isDebugMode ? MessageDirection::Outcoming
                                                     : MessageDirection::Incoming));

    QObject::connect(m_transport, &Transport::received,
                     this, &ControlPanelWidget::slotOnBytesReceive);
    QObject::connect(m_transport, &Transport::sent,
                     this, &ControlPanelWidget::slotOnBytesSend);

    QObject::connect(m_inCtrl.get(), &InputController::messageReceived,
                     this, &ControlPanelWidget::slotReceiveMessage);

    QObject::connect(m_recvThread, &QThread::started,
                     m_transport, &Transport::start);
    QObject::connect(m_recvThread, &QThread::finished,
                     m_transport, &Transport::deleteLater);

    QObject::connect(m_transport, &Transport::connected,
                     this, &ControlPanelWidget::slotOnConnect);
    QObject::connect(m_transport, &Transport::disconnected,
                     this, &ControlPanelWidget::slotOnDisconnect);
    QObject::connect(m_transport, &Transport::error,
                     this, &ControlPanelWidget::slotOnError);

    const QString strAddress = QString("%1:%2")
                               .arg(m_currentAddress)
                               .arg(m_currentPort);
    m_ui->statusLabel->setText(tr("Connecting to host %1").arg(strAddress));
    m_logger->info(tr("Try connect to %1").arg(strAddress));

    m_recvThread->start();
}

void ControlPanelWidget::slotBreakInitialization()
{
    stopTransport();

    m_ui->statusLabel->setText(tr("Connection was interrupted"));
    m_ui->preloadImageLabel->hide();
    m_ui->breakButton->hide();
    m_ui->retryButton->show();
}

void ControlPanelWidget::slotChangeDeviceAddress()
{
    using protocol::outcoming::DeviceAddressMessage;

    ConnectionOptionsDialog dlg;
    if (!m_currentAddress.isEmpty())
    {
        dlg.setAddress(QHostAddress(m_currentAddress));
    }
    if (m_currentPort > 0)
    {
        dlg.setPort(m_currentPort);
    }
    dlg.hideLogOptions();

    if (dlg.exec() == ConnectionOptionsDialog::Accepted)
    {
        DeviceAddressMessage* message = new DeviceAddressMessage();
        message->setAddress(dlg.address().toString());
        message->setPort(dlg.port());

        slotSendMessage(QSharedPointer<protocol::AbstractMessage>(message));
    }
}

void ControlPanelWidget::slotRetryInitialization()
{
    initialize();
}

void ControlPanelWidget::slotOnError()
{
    if (m_logger != nullptr)
    {
        const QString errorString = m_transport->errorString();
        m_logger->error(tr("%1:%2 : %3")
                        .arg(m_currentAddress)
                        .arg(m_currentPort)
                        .arg(errorString));
        m_ui->statusLabel->setText(tr("Error connection: %1").arg(errorString));
    }

    setNotConnectedState();
}

void ControlPanelWidget::slotOnConnect()
{
    const QString strAddress = QString("%1:%2")
                               .arg(m_currentAddress)
                               .arg(m_currentPort);
    m_logger->info(tr("Connected to %1").arg(strAddress));
    setWindowTitle(tr("[Connected] %1 - %2").arg(::titleString()).arg(strAddress));

    m_ui->stackedWidget->setCurrentWidget(m_ui->preloadPage);
    m_ui->statusLabel->setText(tr("Waiting device's configuration"));
    m_ui->preloadImageLabel->show();
    m_ui->breakButton->show();
    m_ui->retryButton->hide();

    if (m_isDebugMode)
    {
        makeDebugConfiguration(::debugControlsCount());
    }
    else
    {
        m_ui->deviceAddressButton->show();
    }
}

void ControlPanelWidget::makeDebugConfiguration(int buttonsCount)
{
    Q_ASSERT(buttonsCount > 0);

    m_ui->deviceIdentityButton->show();

    m_controlIds.reserve(buttonsCount);
    for (quint8 i = 0; i < buttonsCount; ++i)
    {
        m_controlIds.append(i);
    }

    createControls();

    if (m_pingCounter > 0)
    {
        m_pingCounter = 0;
        showPingCounterMessage();
    }

    m_pingTimer->start();
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

void ControlPanelWidget::makeConfiguration(const QVector<settings::DisplaySettings>& displays)
{
    using protocol::BlinkState;
    using protocol::ImageSelection;
    using settings::DisplaySettings;

    m_controlIds.reserve(displays.size());

    for (const DisplaySettings& each : displays)
    {
        m_controlIds.append(each.displayNumber());
    }

    createControls();

    for (const DisplaySettings& each : displays)
    {
        DisplayControlWidget* control = m_controlWidgets[each.displayNumber()];
        if (control != nullptr)
        {
            if (each.isSetFirstImageNumber())
            {
                control->setFirstImage(each.firstImageNumber());
            }
            if (each.isSetSecondImageNumber())
            {
                control->setSecondImage(each.secondImageNumber());
            }
            if (each.isSetTimeOn())
            {
                control->setTimeOn(each.timeOn());
            }
            if (each.isSetTimeOff())
            {
                control->setTimeOff(each.timeOff());
            }
            if (each.isSetBrightLevel())
            {
                control->setBrightLevel(each.brightLevel());
            }

            control->setBlinkingEnabled(each.blinkState() == BlinkState::On);

            switch (each.imageSelection())
            {
            case ImageSelection::First:
                control->resetSecondImage();
                break;
            case ImageSelection::Second:
                control->resetFirstImage();
                break;
            case ImageSelection::Nothing:
                control->resetFirstImage();
                control->resetSecondImage();
                break;
            case ImageSelection::Both:
            default:
                qt_noop();
                break;
            }
        }
    }
}

void ControlPanelWidget::createControls()
{
    for (auto it = m_controlIds.cbegin(), end = m_controlIds.cend(); it != end; ++it)
    {
        createControl(*it);
    }

    slotChangeControlsColumnsCount(m_ui->columnCountSpinBox->value());

    m_ui->stackedWidget->setCurrentWidget(m_ui->workingPage);
}


void ControlPanelWidget::createControl(quint8 controlId)
{
    if (!m_controlWidgets.contains(controlId))
    {
        DisplayControlWidget* wgt = new DisplayControlWidget(m_imgStorage.get(),
                                                             m_isDebugMode);
        m_controlWidgets.insert(controlId, wgt);
        initConnectionsForControl(wgt);

        wgt->setBrightLevel(::defaultBrightLevel());
        wgt->setFirstImage(controlId);
        wgt->setFirstImageEnable(true);
        wgt->setSecondImage(controlId);
        wgt->setSecondImageEnable(false);
        wgt->setTimeOn(::defaultTimeout());
        wgt->setTimeOff(::defaultTimeout());
        wgt->setBlinkingEnabled(false);
    }
}

void ControlPanelWidget::initConnectionsForControl(DisplayControlWidget* control)
{
    Q_CHECK_PTR(control);

    QObject::connect(m_imgStorage.get(), &storage::ImageStorage::imagesChanged,
                     control, &DisplayControlWidget::reloadImages);

    if (m_isDebugMode)
    {
        QObject::connect(control, &DisplayControlWidget::activated,
                         this, &ControlPanelWidget::slotChangeButtonsState);
    }
    else
    {
        QObject::connect(control, &DisplayControlWidget::activated,
                         this, &ControlPanelWidget::slotChangeActiveControl);
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

    if (enabled && m_activeControl != nullptr)
    {
        if (m_optionsWidget == nullptr)
        {
            m_optionsWidget = new DisplayOptionsWidget(m_imgStorage.get());
            emit subwindowCreated(m_optionsWidget);

            QObject::connect(m_imgStorage.get(), &storage::ImageStorage::imagesChanged,
                             m_optionsWidget, &DisplayOptionsWidget::reloadImages);
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::closed,
                             this, &ControlPanelWidget::slotOptionsClose);
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::imageFirstEnabled,
                             this, static_cast<void(ControlPanelWidget::*)(bool)>(&ControlPanelWidget::slotActiveControlImageFirstChange));
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::imageSecondEnabled,
                             this, static_cast<void(ControlPanelWidget::*)(bool)>(&ControlPanelWidget::slotActiveControlImageSecondChange));
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::imageFirstChanged,
                             this, static_cast<void(ControlPanelWidget::*)(int)>(&ControlPanelWidget::slotActiveControlImageFirstChange));
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::imageSecondChanged,
                             this, static_cast<void(ControlPanelWidget::*)(int)>(&ControlPanelWidget::slotActiveControlImageSecondChange));
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::blinkingEnabled,
                             this, &ControlPanelWidget::slotActiveControlBlinkingChange);
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::timeOnChanged,
                             this, &ControlPanelWidget::slotActiveControlTimeOnChange);
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::timeOffChanged,
                             this, &ControlPanelWidget::slotActiveControlTimeOffChange);
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::brightChanged,
                             this, &ControlPanelWidget::slotActiveControlBrightChange);
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::imageFirstSelected,
                             this, &ControlPanelWidget::slotActiveControlImageFirstSelect);
            QObject::connect(m_optionsWidget, &DisplayOptionsWidget::imageSecondSelected,
                             this, &ControlPanelWidget::slotActiveControlImageSecondSelect);
        }

        m_optionsWidget->setFirstImage(m_activeControl->firstImageIndex());
        m_optionsWidget->setFirstImageEnabled(m_activeControl->isFirstImageEnabled());
        m_optionsWidget->setSecondImage(m_activeControl->secondImageIndex());
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
        m_activeControl = nullptr;
    }
}

void ControlPanelWidget::slotApplySelectedImage(quint8 imageIndex)
{
    if (m_optionsWidget != nullptr)
    {
        switch (m_lastSelected)
        {
        case SelectedImage::First:
            m_optionsWidget->setFirstImage(imageIndex);
            break;
        case SelectedImage::Second:
            m_optionsWidget->setSecondImage(imageIndex);
            break;
        case SelectedImage::Unset:
        default:
            break;
        }
    }

    if (!m_isDebugMode)
    {
       createDisplayImagesMessage();
    }

    m_imagesWidget->close();
}

void ControlPanelWidget::slotOptionsClose()
{
    if (m_activeControl != nullptr)
    {
        m_activeControl->setActive(false);
        m_activeControl = nullptr;
        emit subwindowClosed(m_optionsWidget);
        m_optionsWidget = nullptr;
    }
}

void ControlPanelWidget::slotImagesClose()
{
    emit subwindowClosed(m_imagesWidget);
    m_imagesWidget = nullptr;
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
    ButtonsStateMessage* message = new ButtonsStateMessage();
    message->setButtonsStates(std::move(states));

    slotSendMessage(QSharedPointer<protocol::AbstractMessage>(message));
}

void ControlPanelWidget::slotSendPing()
{
    using protocol::incoming::PingMessage;

    slotSendMessage(QSharedPointer<protocol::AbstractMessage>(new PingMessage()));

    ++m_pingCounter;
    showPingCounterMessage();
}

void ControlPanelWidget::slotSendDeviceIdentity()
{
    using protocol::incoming::DeviceIdentityMessage;

    DeviceIdentityMessage* message = new DeviceIdentityMessage();
    message->setFirmwareVersion(static_cast<quint8>(::firmwareVersion()));
    message->setButtonsNumbers(m_controlIds);

    slotSendMessage(QSharedPointer<protocol::AbstractMessage>(message));
}

void ControlPanelWidget::slotSendImagesData()
{
    using protocol::outcoming::ImageDataMessage;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QVector<QSharedPointer<protocol::AbstractMessage>> forSend;
    forSend.reserve(m_imgStorage->availableImages().size());

    QMapIterator<quint8, QString> it(m_imgStorage->availableImages());
    while (it.hasNext())
    {
        const quint8 imageNumber = it.peekNext().key();
        const QString& imageFileName = it.next().value();

        ImageDataMessage* message = new ImageDataMessage();
        message->setImageNumber(imageNumber);
        message->setImageColors(storage::ImageStorage::pixmapToColors(QPixmap(imageFileName)));

        forSend.append(QSharedPointer<protocol::AbstractMessage>(message));
    }

    for (const QSharedPointer<protocol::AbstractMessage>& each : forSend)
    {
        slotSendMessage(each);
    }

    QApplication::restoreOverrideCursor();
}

void ControlPanelWidget::slotOnDisconnect()
{
    const QString strAddress = QString("%1:%2")
                               .arg(m_currentAddress)
                               .arg(m_currentPort);
    m_logger->info(tr("Disconnected from %1").arg(strAddress));
    setWindowTitle(tr("[Disconnected] %1 - %2").arg(::titleString()).arg(strAddress));

    if (   m_transport != nullptr
        && m_transport->errorString().isEmpty())
    {
        m_ui->statusLabel->setText(tr("Connection was cancelled"));
    }

    setNotConnectedState();
}

void ControlPanelWidget::setNotConnectedState()
{
    m_recvThread->quit();
    m_transport = nullptr;
    m_recvThread->wait();

    if (m_optionsWidget != nullptr)
    {
        emit subwindowClosed(m_optionsWidget);
        m_optionsWidget = nullptr;
    }

    if (m_isDebugMode)
    {
        m_pingTimer->stop();

        m_ui->deviceIdentityButton->hide();
    }
    else
    {
        m_ui->deviceAddressButton->hide();
    }

    removeAllContols();

    m_ui->stackedWidget->setCurrentWidget(m_ui->preloadPage);
    m_ui->preloadImageLabel->hide();
    m_ui->breakButton->hide();
    m_ui->retryButton->show();
}

void ControlPanelWidget::showPingCounterMessage()
{
    m_statusBar->showMessage(tr("%1 Null-messages count: %2")
                             .arg(m_isDebugMode ? tr("Sent") : tr("Received"))
                             .arg(m_pingCounter));
}

void ControlPanelWidget::slotOnBytesReceive(const QByteArray& bytes)
{
    const QString str = createReceivedBytesLogMessage(bytes);
    m_logger->trace(str);

    if (m_logger->level() >= Logger::Level::Trace)
    {
        logMessage(str, protocol::MessageDirection::Incoming);
    }
}

QString ControlPanelWidget::createReceivedBytesLogMessage(const QByteArray& bytes) const
{
    return tr("Received bytes (size %1) from %2:%3 : %4")
           .arg(bytes.size())
           .arg(m_currentAddress)
           .arg(m_currentPort)
           .arg(QString::fromLatin1(bytes.toHex()).toUpper());
}

void ControlPanelWidget::slotOnBytesSend(const QByteArray& bytes)
{
    const QString str = createSentBytesLogMessage(bytes);
    m_logger->trace(str);

    if (m_logger->level() >= Logger::Level::Trace)
    {
        logMessage(str, protocol::MessageDirection::Outcoming);
    }
}

QString ControlPanelWidget::createSentBytesLogMessage(const QByteArray& bytes) const
{
    return tr("Sent bytes (size %1) to %2:%3 : %4")
           .arg(bytes.size())
           .arg(m_currentAddress)
           .arg(m_currentPort)
           .arg(QString::fromLatin1(bytes.toHex()).toUpper());
}

void ControlPanelWidget::logMessage(const QString& message, protocol::MessageDirection direction)
{
    using protocol::MessageDirection;

    QPlainTextEdit* target = nullptr;
    switch (direction)
    {
    case MessageDirection::Incoming:
        target = m_ui->incomingLogTextEdit;
        break;
    case MessageDirection::Outcoming:
        target = m_ui->outcomingLogTextEdit;
        break;
    default:
        return;
    }

    QString log = target->toPlainText();
    if (!log.isEmpty())
    {
        log += "\n";
    }
    log += message;
    target->setPlainText(log);
    target->verticalScrollBar()->setValue(target->verticalScrollBar()->maximum());
}

void ControlPanelWidget::slotSendMessage(const QSharedPointer<protocol::AbstractMessage>& message)
{
    using namespace protocol;

    if (   m_outCtrl != nullptr
        && message != nullptr)
    {
        m_outCtrl->slotSend(*message);

        const bool isIncomingPing =    message->direction() == MessageDirection::Incoming
                                    && dynamic_cast<const incoming::Message&>(*message).type() == incoming::MessageType::Ping;
        const bool isOutcomingPing =    message->direction() == MessageDirection::Outcoming
                                     && dynamic_cast<const outcoming::Message&>(*message).type() == outcoming::MessageType::Ping;
        if (!isIncomingPing && !isOutcomingPing)
        {
            QByteArray ba;
            {
                QDataStream out(&ba, QIODevice::WriteOnly);
                out.setByteOrder(QDataStream::LittleEndian);
                out << *message;
            }
            const QString str = createSentBytesLogMessage(ba);
            m_logger->info(str);
            logMessage(str, MessageDirection::Outcoming);
        }
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
    case MessageType::Ping:
        ++m_pingCounter;
        showPingCounterMessage();
        break;
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

    if (message.type() != MessageType::Ping)
    {
        QByteArray ba;
        {
            QDataStream out(&ba, QIODevice::WriteOnly);
            out.setByteOrder(QDataStream::LittleEndian);
            out << message;
        }
        const QString str = createReceivedBytesLogMessage(ba);
        m_logger->info(str);
        logMessage(str, protocol::MessageDirection::Incoming);
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
    case MessageType::Ping:
        ++m_pingCounter;
        showPingCounterMessage();
        break;
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
            const DisplayImagesMessage& dimsg = dynamic_cast<const DisplayImagesMessage&>(message);
            QVector<quint8>::const_iterator founded = std::find(m_controlIds.cbegin(), m_controlIds.cend(), dimsg.displayNumber());
            if (founded != m_controlIds.cend())
            {
                DisplayControlWidget* control = m_controlWidgets[*founded];
                control->setFirstImage(dimsg.firstImageNumber());
                control->setSecondImage(dimsg.secondImageNumber());
            }
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
                    control->setFirstImageEnable(false);
                    control->setSecondImage(false);
                    break;
                case protocol::ImageSelection::First:
                    control->setFirstImageEnable(true);
                    control->setSecondImageEnable(false);
                    break;
                case protocol::ImageSelection::Second:
                    control->setFirstImageEnable(false);
                    control->setSecondImageEnable(true);
                    break;
                case protocol::ImageSelection::Both:
                    control->setFirstImageEnable(true);
                    control->setSecondImageEnable(true);
                    break;
                default:
                    break;
                }
            }
        }
        break;
    case MessageType::DeviceAddress:
        {
            const DeviceAddressMessage& damsg = dynamic_cast<const DeviceAddressMessage&>(message);
            qInfo().noquote() << tr("Receive request to change device address to %1:%2").arg(damsg.address()).arg(damsg.port());
        }
        break;
    case MessageType::ImageData:
        {
            const ImageDataMessage& idmsg = dynamic_cast<const ImageDataMessage&>(message);
            QString errorString;
            QTemporaryFile* tmp = new QTemporaryFile(this);
            if (tmp->open())
            {
                tmp->close();
                const QPixmap image = storage::ImageStorage::pixmapFromColors(idmsg.imageColors(), ::buttonImageSize());
                if (image.save(tmp->fileName(), "BMP"))
                {
                    m_imgStorage->addImage(idmsg.imageNumber(), tmp->fileName());
                }
                else
                {
                    errorString = tr("Pixmap saving failed");
                }
            }
            else
            {
                errorString = tmp->errorString();
            }

            if (!errorString.isEmpty())
            {
                qWarning().noquote() << tr("Can not save image: %1").arg(errorString);
                tmp->deleteLater();
            }
        }
        break;
    default:
        Q_ASSERT_X(false, "ControlPanelWidget::processMessage", "Unexpected outcoming MessageType");
        break;
    }

    if (message.type() != MessageType::Ping)
    {
        QByteArray ba;
        {
            QDataStream out(&ba, QIODevice::WriteOnly);
            out.setByteOrder(QDataStream::LittleEndian);
            out << message;
        }
        const QString str = createReceivedBytesLogMessage(ba);
        m_logger->info(str);
        logMessage(str, protocol::MessageDirection::Incoming);
    }
}

void ControlPanelWidget::slotActiveControlImageFirstChange(bool enabled)
{
    Q_CHECK_PTR(m_activeControl);

    if (m_activeControl->isFirstImageEnabled() != enabled)
    {
        m_activeControl->setFirstImageEnable(enabled);
        createDisplayOptionsMessage();
    }
}

void ControlPanelWidget::slotActiveControlImageSecondChange(bool enabled)
{
    Q_CHECK_PTR(m_activeControl);

    if (m_activeControl->isSecondImageEnabled() != enabled)
    {
        m_activeControl->setSecondImageEnable(enabled);
        createDisplayOptionsMessage();
    }
}

void ControlPanelWidget::slotActiveControlImageFirstChange(int imageIndex)
{
    Q_CHECK_PTR(m_activeControl);

    if (m_activeControl->firstImageIndex() != imageIndex)
    {
        DisplayOptionsWidget* options = qobject_cast<DisplayOptionsWidget*>(sender());
        if (imageIndex != storage::ImageStorage::kInvalidIndex)
        {
            m_activeControl->setFirstImage(static_cast<quint8>(imageIndex));
            if (options != nullptr)
            {
                m_activeControl->setFirstImageEnable(options->isFirstImageEnable());
            }
        }
        else
        {
            m_activeControl->resetFirstImage();
        }
        createDisplayOptionsMessage();
    }
}

void ControlPanelWidget::slotActiveControlImageSecondChange(int imageIndex)
{
    Q_CHECK_PTR(m_activeControl);

    if (m_activeControl->secondImageIndex() != imageIndex)
    {
        DisplayOptionsWidget* options = qobject_cast<DisplayOptionsWidget*>(sender());
        if (imageIndex != storage::ImageStorage::kInvalidIndex)
        {
            m_activeControl->setSecondImage(static_cast<quint8>(imageIndex));
            if (options != nullptr)
            {
                m_activeControl->setSecondImageEnable(options->isSecondImageEnable());
            }
        }
        else
        {
            m_activeControl->resetSecondImage();
        }
        createDisplayOptionsMessage();
    }
}

void ControlPanelWidget::slotLoadImages()
{
    m_lastSelected = SelectedImage::Unset;
    showImagesWidget();
}

void ControlPanelWidget::slotActiveControlImageFirstSelect()
{
    m_lastSelected = SelectedImage::First;
    showImagesWidget();
}

void ControlPanelWidget::slotActiveControlImageSecondSelect()
{
    m_lastSelected = SelectedImage::Second;
    showImagesWidget();
}

void ControlPanelWidget::showImagesWidget()
{
    if (m_imagesWidget == nullptr)
    {
        m_imagesWidget = new ImageStorageWidget(m_imgStorage.get());
        emit subwindowCreated(m_imagesWidget);

        QObject::connect(m_imagesWidget, &ImageStorageWidget::closed,
                         this, &ControlPanelWidget::slotImagesClose);
        QObject::connect(m_imagesWidget, &ImageStorageWidget::imageSelected,
                         this, &ControlPanelWidget::slotApplySelectedImage);
        QObject::connect(m_imagesWidget, &ImageStorageWidget::uploadNeeded,
                         this, &ControlPanelWidget::slotSendImagesData);

        m_imagesWidget->show();
    }
    else
    {
        emit subwindowRaised(m_imagesWidget);
    }
}

void ControlPanelWidget::slotActiveControlBlinkingChange(bool enabled)
{
    Q_CHECK_PTR(m_activeControl);

    if (m_activeControl->isBlinkingEnabled() != enabled)
    {
        m_activeControl->setBlinkingEnabled(enabled);
        createDisplayOptionsMessage();
    }
}

void ControlPanelWidget::slotActiveControlTimeOnChange(int msec)
{
    Q_CHECK_PTR(m_activeControl);

    if (m_activeControl->timeOn() != msec)
    {
        m_activeControl->setTimeOn(msec);
        createBlinkOptionsMessage();
    }
}

void ControlPanelWidget::slotActiveControlTimeOffChange(int msec)
{
    Q_CHECK_PTR(m_activeControl);

    if (m_activeControl->timeOff() != msec)
    {
        m_activeControl->setTimeOff(msec);
        createBlinkOptionsMessage();
    }
}

void ControlPanelWidget::slotActiveControlBrightChange(int level)
{
    Q_CHECK_PTR(m_activeControl);

    if (m_activeControl->brightLevel() != level)
    {
        m_activeControl->setBrightLevel(level);
        createBrightOptionsMessage();
    }
}

quint8 ControlPanelWidget::findActiveControlId(bool* ok) const
{
    Q_CHECK_PTR(ok);

    *ok = false;

    if (m_activeControl != nullptr)
    {
        const int kInvalid = -1;
        int res = m_controlWidgets.key(m_activeControl, kInvalid);
        if (res != kInvalid)
        {
            *ok = true;
            return static_cast<quint8>(res);
        }
    }

    return 0;
}

void ControlPanelWidget::createDisplayOptionsMessage()
{
    using protocol::BlinkState;
    using protocol::ImageSelection;
    using protocol::outcoming::DisplayOptionsMessage;

    bool ok = false;
    const quint8 controlId = findActiveControlId(&ok);
    if (ok)
    {
        DisplayOptionsMessage* message = new DisplayOptionsMessage();
        message->setDisplayNumber(controlId);
        message->setBlinkState(m_activeControl->isBlinkingEnabled() ? BlinkState::On
                                                                    : BlinkState::Off);
        if (   m_activeControl->isFirstImageEnabled()
            && m_activeControl->isSecondImageEnabled())
        {
            message->setImageSelection(ImageSelection::Both);
        }
        else if (m_activeControl->isFirstImageEnabled())
        {
            message->setImageSelection(ImageSelection::First);
        }
        else if (m_activeControl->isSecondImageEnabled())
        {
            message->setImageSelection(ImageSelection::Second);
        }
        else
        {
            message->setImageSelection(ImageSelection::Nothing);
        }

        slotSendMessage(QSharedPointer<protocol::AbstractMessage>(message));
    }
}

void ControlPanelWidget::createDisplayImagesMessage()
{
    using protocol::outcoming::DisplayImagesMessage;

    bool ok = false;
    const quint8 controlId = findActiveControlId(&ok);
    if (ok)
    {
        DisplayImagesMessage* message = new DisplayImagesMessage();
        message->setDisplayNumber(controlId);
        if (m_activeControl->firstImageIndex() != storage::ImageStorage::kInvalidIndex)
        {
            message->setFirstImageNumber(static_cast<quint8>(m_activeControl->firstImageIndex()));
        }
        if (m_activeControl->secondImageIndex() != storage::ImageStorage::kInvalidIndex)
        {
            message->setSecondImageNumber(static_cast<quint8>(m_activeControl->secondImageIndex()));
        }

        slotSendMessage(QSharedPointer<protocol::AbstractMessage>(message));
    }
}

void ControlPanelWidget::createBlinkOptionsMessage()
{
    using protocol::outcoming::BlinkOptionsMessage;

    bool ok = false;
    const quint8 controlId = findActiveControlId(&ok);
    if (ok)
    {
        BlinkOptionsMessage* message = new BlinkOptionsMessage();
        message->setDisplayNumber(controlId);
        message->setTimeOn(static_cast<quint8>(m_activeControl->timeOn() / 10));
        message->setTimeOff(static_cast<quint8>(m_activeControl->timeOff() / 10));

        slotSendMessage(QSharedPointer<protocol::AbstractMessage>(message));
    }
}

void ControlPanelWidget::createBrightOptionsMessage()
{
    using protocol::outcoming::BrightOptionsMessage;

    bool ok = false;
    const quint8 controlId = findActiveControlId(&ok);
    if (ok)
    {
        BrightOptionsMessage* message = new BrightOptionsMessage();
        message->setDisplayNumber(controlId);
        message->setBrightLevel(static_cast<quint8>(m_activeControl->brightLevel()));

        slotSendMessage(QSharedPointer<protocol::AbstractMessage>(message));
    }
}

void ControlPanelWidget::slotChangeControlsColumnsCount(int count)
{
    QGridLayout* oldCommonLayout = qobject_cast<QGridLayout*>(m_ui->commonGroupBox->layout());
    QGridLayout* oldAdditionalLayout = qobject_cast<QGridLayout*>(m_ui->additionalGroupBox->layout());

    if (   oldCommonLayout == nullptr
        || oldCommonLayout->columnCount()-1 != count)
    {
        QGridLayout* newCommonLayout = new QGridLayout();
        QGridLayout* newAdditionalLayout = new QGridLayout();

        int row = 0;
        int col = 0;

        int rowAdd = 0;

        quint8 completedCount = 0;

        for (const quint8 id : m_controlIds)
        {
            DisplayControlWidget* each = m_controlWidgets[static_cast<int>(id)];
            if (each != nullptr)
            {
                if (completedCount++ < ::commonControlsCount())
                {
                    newCommonLayout->addWidget(each, row, col);
                    if (++col >= count)
                    {
                        newCommonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), row, col);
                        col = 0;
                        ++row;
                    }
                }
                else
                {
                    newAdditionalLayout->addWidget(each, rowAdd++, 0);
                }
            }
        }
        newCommonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), (col > 0 ? row+1 : row), 0);
        newAdditionalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), rowAdd, 0);

        delete oldCommonLayout;
        delete oldAdditionalLayout;

        m_ui->commonGroupBox->setLayout(newCommonLayout);
        m_ui->additionalGroupBox->setLayout(newAdditionalLayout);

        m_ui->additionalGroupBox->setVisible(completedCount > ::commonControlsCount());
    }
}

void ControlPanelWidget::slotCloseConnection()
{
    stopTransport();
}

void ControlPanelWidget::slotLoadConfiguration()
{
    using protocol::BlinkState;
    using protocol::ImageSelection;
    using settings::DisplaySettings;
    using settings::Settings;
    using storage::ImageStorage;

    const QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                          tr("Select file to load configuration"),
                                                          QCoreApplication::applicationDirPath(),
                                                          tr("INI Files (*.ini);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        Settings current(fileName);
        current.load();

        for (quint8 index : m_imgStorage->availableImages().keys())
        {
            m_imgStorage->removeImage(index);
        }
        quint8 index = 0;
        for (const QString& each : current.imagesFileNames())
        {
            m_imgStorage->addImage(index++, each);
        }

        removeAllContols();
        makeConfiguration(current.displaysOptions());
    }
}

void ControlPanelWidget::slotSaveConfiguration()
{
    using protocol::BlinkState;
    using protocol::ImageSelection;
    using settings::DisplaySettings;
    using settings::Settings;
    using storage::ImageStorage;

    const QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                          tr("Select file to save configuration"),
                                                          QCoreApplication::applicationDirPath(),
                                                          tr("INI Files (*.ini);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        Settings current(fileName);
        current.setImagesFileNames(m_imgStorage->availableImages().values());

        QVector<DisplaySettings> displays;
        displays.reserve(m_controlIds.size());

        for (const quint8 index : m_controlIds)
        {
            const DisplayControlWidget* control = m_controlWidgets[index];
            if (control != nullptr)
            {
                DisplaySettings each;
                each.setDisplayNumber(index);

                if (control->firstImageIndex() > ImageStorage::kInvalidIndex)
                {
                    each.setFirstImageNumber(static_cast<quint8>(control->firstImageIndex()));
                }
                if (control->secondImageIndex() > ImageStorage::kInvalidIndex)
                {
                    each.setSecondImageNumber(static_cast<quint8>(control->secondImageIndex()));
                }

                if (control->isFirstImageEnabled() && control->isSecondImageEnabled())
                {
                    each.setImageSelection(ImageSelection::Both);
                }
                else if (control->isFirstImageEnabled())
                {
                    each.setImageSelection(ImageSelection::First);
                }
                else if (control->isSecondImageEnabled())
                {
                    each.setImageSelection(ImageSelection::Second);
                }
                else
                {
                    each.setImageSelection(ImageSelection::Nothing);
                }

                each.setBlinkState(control->isBlinkingEnabled() ? BlinkState::On : BlinkState::Off);
                each.setBrightLevel(static_cast<quint8>(control->brightLevel()));
                each.setTimeOn(static_cast<quint16>(control->timeOn()));
                each.setTimeOff(static_cast<quint16>(control->timeOff()));

                displays.append(each);
            }
        }
        current.setDisplaysOptions(displays);

        current.save();
    }
}
