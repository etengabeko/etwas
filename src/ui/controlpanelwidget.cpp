#include "controlpanelwidget.h"
#include "ui_controlpanel.h"

#include <QByteArray>
#include <QHostAddress>
#include <QScrollBar>
#include <QSharedPointer>

#include "ioservice/inputcontroller.h"
#include "ioservice/outputcontroller.h"
#include "ioservice/transport.h"
#include "logger/logger.h"
#include "protocol/protocol.h"
#include "settings/settings.h"

ControlPanelWidget::ControlPanelWidget(QWidget* parent) :
    QWidget(parent),
    m_ui(new Ui::ControlPanel())
{
    m_ui->setupUi(this);
}

ControlPanelWidget::~ControlPanelWidget()
{
    m_inCtrlIn.reset();
    m_inCtrlOut.reset();
    m_outCtrl.reset();
    m_transport.reset();

    delete m_ui;
    m_ui = nullptr;
}

void ControlPanelWidget::initialize(const QHostAddress& address, quint16 port)
{
    using namespace ioservice;
    using protocol::MessageDirection;

    settings::Settings conf;
    conf.setAddress(address);
    conf.setPort(port);

    m_transport.reset(new Transport(conf));
    m_inCtrlIn.reset(new InputController(m_transport.get(), MessageDirection::Incoming));
    m_inCtrlOut.reset(new InputController(m_transport.get(), MessageDirection::Outcoming));
    m_outCtrl.reset(new OutputController(m_transport.get()));

    QObject::connect(m_transport.get(), &Transport::received,
                     this, &ControlPanelWidget::slotReceiveBytes);
    QObject::connect(m_inCtrlIn.get(), &InputController::messageReceived,
                     this, &ControlPanelWidget::slotReceiveMessage);
    QObject::connect(m_inCtrlOut.get(), &InputController::messageReceived,
                     this, &ControlPanelWidget::slotReceiveMessage);

    auto res = m_transport->start();
    if (res.first)
    {
        Logger::instance().debug(tr("Transport started successful"));
    }
    else
    {
        Logger::instance().error(tr("Failed start transport: %1")
                      .arg(res.second));
    }
}

void ControlPanelWidget::slotReceiveBytes(const QByteArray& bytes)
{
    const QString str = tr("Received bytes [%1]: %2")
            .arg(bytes.size())
            .arg(QString::fromLatin1(bytes.toHex()));
    Logger::instance().debug(str);
}

void ControlPanelWidget::slotReceiveMessage(const QSharedPointer<protocol::AbstractMessage>& message)
{
    Q_CHECK_PTR(message);

    const QByteArray ba = message->serialize();
    const QString str = tr("Parsed message [%1]: %2")
            .arg(ba.size())
            .arg(QString::fromLatin1(ba.toHex()));
    Logger::instance().debug(str);

    QString log = m_ui->logTextEdit->toPlainText();
    if (!log.isEmpty())
    {
        log += "\n";
    }
    log += str;
    m_ui->logTextEdit->setPlainText(log);
    m_ui->logTextEdit->verticalScrollBar()->setValue(m_ui->logTextEdit->verticalScrollBar()->maximum());
}
