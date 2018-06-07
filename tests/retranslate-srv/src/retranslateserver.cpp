#include "retranslateserver.h"

#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

namespace test
{

RetranslateServer::RetranslateServer(QObject* parent) :
    QObject(parent),
    m_server(new QTcpServer(this))
{
    QObject::connect(m_server, &QTcpServer::newConnection,
                     this, &RetranslateServer::slotOnNewConnection);
}

RetranslateServer::~RetranslateServer() noexcept
{
    m_server->close();
    delete m_server;
    m_server = nullptr;

    for (QTcpSocket* each : m_clients)
    {
        each->disconnectFromHost();
        each->waitForDisconnected();
        each->close();
        each->deleteLater();
    }
    m_clients.clear();
}

bool RetranslateServer::start(quint16 port)
{
    bool ok = m_server->listen(QHostAddress::Any, port);
    if (ok)
        qInfo().noquote() << tr("Start listening port %1")
                             .arg(port);
    else
        qWarning().noquote() << tr("Failure start listening port %1: %2")
                                .arg(port)
                                .arg(m_server->errorString());
    return ok;
}

void RetranslateServer::slotOnNewConnection()
{
    while (m_server->hasPendingConnections())
    {
        QTcpSocket* socket = m_server->nextPendingConnection();
        if (socket != nullptr)
        {
            qInfo().noquote() << tr("New connection: %1:%2")
                                 .arg(socket->peerAddress().toString())
                                 .arg(socket->peerPort());
            QObject::connect(socket, &QTcpSocket::readyRead,
                             this, &RetranslateServer::slotReadAndRetranslate);
            QObject::connect(socket, &QTcpSocket::disconnected,
                             this, &RetranslateServer::slotOnClientDisconnect);
            m_clients.append(socket);
        }
    }
}

void RetranslateServer::slotOnClientDisconnect()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket != nullptr)
    {
        qInfo().noquote() << tr("Disconnect: %1:%2")
                             .arg(socket->peerAddress().toString())
                             .arg(socket->peerPort());
        int index = m_clients.indexOf(socket);
        if (index >= 0)
        {
            m_clients.removeAt(index);
        }

        socket->disconnectFromHost();
        socket->waitForDisconnected();
        socket->close();
        socket->deleteLater();
    }
}

void RetranslateServer::slotReadAndRetranslate()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket != nullptr)
    {
        const QByteArray data = socket->readAll();
        qDebug().noquote() << tr("Received from %1:%2:\n%3")
                              .arg(socket->peerAddress().toString())
                              .arg(socket->peerPort())
                              .arg(QString::fromLatin1(data.toHex()));
        for (QTcpSocket* each : m_clients)
        {
            if (each != socket)
            {
                each->write(data);
                each->waitForBytesWritten();
            }
        }
    }
}

} // test
