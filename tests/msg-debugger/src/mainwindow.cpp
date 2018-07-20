#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QByteArray>
#include <QHostAddress>
#include <QLabel>
#include <QPixmap>
#include <QScrollBar>
#include <QSharedPointer>
#include <QString>
#include <QTableWidget>

#include "ioservice/inputcontroller.h"
#include "ioservice/transport.h"
#include "protocol/protocol.h"
#include "storage/imagestorage.h"

namespace
{

const QSize& buttonImageSize()
{
    static const QSize size(64, 48);
    return size;
}

enum Column
{
    Number = 0,
    Image
};

}

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::MainWindow())
{
    m_ui->setupUi(this);
    m_ui->disconnectButton->hide();

    QObject::connect(m_ui->connectButton, &QPushButton::clicked,
                     this, &MainWindow::slotStart);
    QObject::connect(m_ui->disconnectButton, &QPushButton::clicked,
                     this, &MainWindow::slotStop);
    QObject::connect(m_ui->clearImagesButton, &QPushButton::clicked,
                     this, &MainWindow::slotClearImages);
    QObject::connect(m_ui->clearLogButton, &QPushButton::clicked,
                     m_ui->logTextEdit, &QPlainTextEdit::clear);
}

MainWindow::~MainWindow()
{
    m_ctrlIn.reset();
    m_ctrlOut.reset();
    m_transport.reset();

    delete m_ui;
    m_ui = nullptr;
}

void MainWindow::slotStart()
{
    using namespace ioservice;
    using protocol::MessageDirection;

    const QString address = m_ui->addressLineEdit->text();
    m_transport.reset(new Transport((address.toLower() == "localhost" ? QHostAddress(QHostAddress::LocalHost)
                                                                      : QHostAddress(address)),
                                    static_cast<quint16>(m_ui->portSpinBox->value())));
    m_ctrlIn.reset(new InputController(m_transport.data(), MessageDirection::Incoming));
    m_ctrlOut.reset(new InputController(m_transport.data(), MessageDirection::Outcoming));

    QObject::connect(m_transport.data(), &Transport::connected,
                     this, &MainWindow::slotConnect);
    QObject::connect(m_transport.data(), &Transport::disconnected,
                     this, &MainWindow::slotDisconnect);
    QObject::connect(m_ctrlIn.data(), &InputController::messageReceived,
                     this, &MainWindow::slotParseIncoming);
    QObject::connect(m_ctrlOut.data(), &InputController::messageReceived,
                     this, &MainWindow::slotParseOutcoming);

    m_transport->start();
}

void MainWindow::slotStop()
{
    m_transport->stop();
}

void MainWindow::slotClearImages()
{
    m_ui->imagesTableWidget->clearContents();
    m_ui->imagesTableWidget->setRowCount(0);
}

void MainWindow::slotConnect()
{
    m_ui->addressLineEdit->setEnabled(false);
    m_ui->portSpinBox->setEnabled(false);
    m_ui->connectButton->hide();
    m_ui->disconnectButton->show();

    m_ui->imagesTableWidget->clearContents();

    addLogMessage(tr("Connected to %1:%2")
                  .arg(m_ui->addressLineEdit->text())
                  .arg(m_ui->portSpinBox->value()));
}

void MainWindow::slotDisconnect()
{
    m_ui->addressLineEdit->setEnabled(true);
    m_ui->portSpinBox->setEnabled(true);
    m_ui->connectButton->show();
    m_ui->disconnectButton->hide();

    addLogMessage(tr("Disconnected from %1:%2")
                  .arg(m_ui->addressLineEdit->text())
                  .arg(m_ui->portSpinBox->value()));
}

void MainWindow::addLogMessage(const QString& str)
{
    QString log = m_ui->logTextEdit->toPlainText();
    if (!log.isEmpty())
    {
        log += "\n";
    }
    log += str;

    m_ui->logTextEdit->setPlainText(log);
    m_ui->logTextEdit->verticalScrollBar()->setValue(m_ui->logTextEdit->verticalScrollBar()->maximum());
}

void MainWindow::slotReceiveBytes(const QByteArray& data)
{
    addLogMessage(tr("Received bytes (size %1):\n%2")
                  .arg(data.size())
                  .arg(QString::fromLatin1(data.toHex()).toUpper()));
}

void MainWindow::slotParseIncoming(const QSharedPointer<protocol::AbstractMessage>& msg)
{
    using namespace protocol::incoming;

    const Message& parsed = dynamic_cast<const Message&>(*msg);

    addLogMessage(tr("Parsed message type = %1: %2")
                  .arg(typeToString(parsed.type()))
                  .arg(messageToString(parsed)));
}

void MainWindow::slotParseOutcoming(const QSharedPointer<protocol::AbstractMessage>& msg)
{
    using namespace protocol::outcoming;

    const Message& parsed = dynamic_cast<const Message&>(*msg);

    addLogMessage(tr("Parsed message type = %1: %2")
                  .arg(typeToString(parsed.type()))
                  .arg(messageToString(parsed)));

    if (parsed.type() == MessageType::ImageData)
    {
        addImageFromMessage(parsed);
    }
}

QString MainWindow::messageToString(const protocol::incoming::Message& /*msg*/) const
{
    // TODO
    return QString::null;
}

QString MainWindow::messageToString(const protocol::outcoming::Message& /*msg*/) const
{
    // TODO
    return QString::null;
}

void MainWindow::addImageFromMessage(const protocol::outcoming::Message& msg)
{
    using namespace protocol::outcoming;

    const ImageDataMessage& imgMessage = dynamic_cast<const ImageDataMessage&>(msg);

    const int row = m_ui->imagesTableWidget->rowCount();
    m_ui->imagesTableWidget->setRowCount(row+1);

    const QPixmap pxm = storage::ImageStorage::pixmapFromColors(imgMessage.imageColors(), ::buttonImageSize());

    QTableWidgetItem* item = new QTableWidgetItem(QString::number(imgMessage.imageNumber()));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_ui->imagesTableWidget->setItem(row, Column::Number, item);

    QLabel* label = new QLabel(this);
    label->setPixmap(pxm);
    m_ui->imagesTableWidget->setCellWidget(row, Column::Image, label);
    m_ui->imagesTableWidget->setRowHeight(row, pxm.height());

    m_ui->imagesTableWidget->resizeColumnsToContents();
}
