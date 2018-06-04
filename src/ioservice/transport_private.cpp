#include "transport_private.h"

#include <QByteArray>
#include <QString>
#include <QTcpSocket>

namespace ioservice
{
namespace details
{

TransportPrivate::TransportPrivate(const QHostAddress& address,
                                   quint16 port,
                                   std::function<void(const QByteArray&)> onReceive,
                                   QObject* parent) :
    QObject(parent),
    m_address(address),
    m_port(port),
    m_onReceive(onReceive)
{
    Q_ASSERT(m_onReceive);
}

QPair<bool, QString> TransportPrivate::start()
{
    if (m_socket == nullptr)
    {
        m_socket = new QTcpSocket(this);
        QObject::connect(m_socket, &QTcpSocket::readyRead,
                         this, &TransportPrivate::slotReceive);
    }
    else if (m_socket->state() == QTcpSocket::ConnectedState)
    {
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected();
    }

    m_socket->connectToHost(m_address, m_port);
    m_socket->waitForConnected();

    return (m_socket->state() == QTcpSocket::ConnectedState ? qMakePair(true, QString())
                                                            : qMakePair(false, m_socket->errorString()));
}

void TransportPrivate::slotReceive()
{
    Q_CHECK_PTR(m_socket);

    m_onReceive(m_socket->readAll());
}

void TransportPrivate::send(const QByteArray& data)
{
    if (m_socket == nullptr)
    {
        return;
    }
    else if (m_socket->state() != QTcpSocket::ConnectedState)
    {
        return;
    }


    m_socket->write(data);
    m_socket->waitForBytesWritten();
}

void TransportPrivate::send(QByteArray&& data)
{
    if (m_socket == nullptr)
    {
        return;
    }
    else if (m_socket->state() != QTcpSocket::ConnectedState)
    {
        return;
    }


    m_socket->write(data);
    m_socket->waitForBytesWritten();
}

} // details
} // ioservice
