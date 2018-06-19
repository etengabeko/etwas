#include "controlpanelwidget.h"
#include "ui_controlpanel.h"

#include <QByteArray>
#include <QCloseEvent>
#include <QHostAddress>
#include <QScrollBar>
#include <QSharedPointer>

#include "createmessageswidget.h"

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

    CreateMessagesWidget* wgt = new CreateMessagesWidget(this);
    QLayout* lout = m_ui->controlGroupBox->layout();
    QLayoutItem* item = lout->takeAt(lout->count()-1);
    lout->addWidget(wgt);
    lout->addItem(item);

    QObject::connect(wgt, &CreateMessagesWidget::messageCreated,
                     this, &ControlPanelWidget::slotSendMessage);
    QObject::connect(m_ui->incomingRadioButton, &QRadioButton::toggled,
                     this, &ControlPanelWidget::slotChangeReceiveMessagesType);
    QObject::connect(m_ui->outcomingRadioButton, &QRadioButton::toggled,
                     this, &ControlPanelWidget::slotChangeReceiveMessagesType);
    QObject::connect(m_ui->clearButton, &QPushButton::clicked,
                     m_ui->logTextEdit, &QPlainTextEdit::clear);
}

ControlPanelWidget::~ControlPanelWidget()
{
    m_inCtrl.reset();
    m_outCtrl.reset();
    m_transport.reset();

    delete m_ui;
    m_ui = nullptr;
}

void ControlPanelWidget::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    emit closed();
}

bool ControlPanelWidget::initialize(const QHostAddress& address, quint16 port)
{
    using namespace ioservice;
    using protocol::MessageDirection;

    settings::Settings conf;
    conf.setAddress(address);
    conf.setPort(port);

    m_transport.reset(new Transport(conf));
    m_outCtrl.reset(new OutputController(m_transport.get()));

    QObject::connect(m_transport.get(), &Transport::received,
                     this, &ControlPanelWidget::slotReceiveBytes);

    m_ui->outcomingRadioButton->setChecked(true);

    auto res = m_transport->start();
    if (res.first)
    {
        setWindowTitle(QString("%1 - %2:%3")
                       .arg(windowTitle())
                       .arg(address.toString())
                       .arg(port));
        Logger::instance().debug(tr("Transport started successful"));
    }
    else
    {
        emit error(res.second);
    }
    return res.first;
}

void ControlPanelWidget::slotChangeReceiveMessagesType(bool checked)
{
    using ioservice::InputController;

    if (checked)
    {
        if (sender() == m_ui->incomingRadioButton)
        {
            m_inCtrl.reset(new InputController(m_transport.get(), protocol::MessageDirection::Incoming));
        }
        else if (sender() == m_ui->outcomingRadioButton)
        {
            m_inCtrl.reset(new InputController(m_transport.get(), protocol::MessageDirection::Outcoming));
        }

        if (m_inCtrl != nullptr)
        {
            QObject::connect(m_inCtrl.get(), &InputController::messageReceived,
                             this, &ControlPanelWidget::slotReceiveMessage);
        }
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

void ControlPanelWidget::slotSendMessage(const QSharedPointer<protocol::AbstractMessage>& message)
{
    Q_CHECK_PTR(message);

    const QByteArray ba = message->serialize();
    const QString str = tr("Sent message [%1]: %2")
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

    m_outCtrl->slotSend(*message);
}
