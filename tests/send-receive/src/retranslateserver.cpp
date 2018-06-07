#include "retranslateserver.h"

#include <QTcpServer>
#include <QTcpSocket>

namespace test
{
namespace sendrecv
{

RetranslateServer::RetranslateServer(quint16 port, QObject* parent) :
    QObject(parent),
    m_server(new QTcpServer(this))
{
    QObject::connect(m_server, &QTcpServer::newConnection,
                     this, &RetranslateServer::slotOnNewConnection);

    m_server->listen(QHostAddress::Any, port);
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

void RetranslateServer::slotOnNewConnection()
{
    while (m_server->hasPendingConnections())
    {
        QTcpSocket* socket = m_server->nextPendingConnection();
        if (socket != nullptr)
        {
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

} // sendrecv
} // test
