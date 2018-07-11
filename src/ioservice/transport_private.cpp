#include "transport_private.h"

#include <QByteArray>
#include <QTcpSocket>

namespace ioservice
{
namespace details
{

TransportPrivate::TransportPrivate(const QHostAddress& address,
                                   quint16 port,
                                   QObject* parent) :
    QObject(parent),
    m_address(address),
    m_port(port)
{

}

TransportPrivate::~TransportPrivate() NOEXCEPT
{
    if (m_socket != nullptr)
    {
        m_socket->close();
        m_socket->deleteLater();
    }
}

const QHostAddress& TransportPrivate::address() const NOEXCEPT
{
    return m_address;
}

quint16 TransportPrivate::port() const NOEXCEPT
{
    return m_port;
}

const QString TransportPrivate::errorString() const
{
    return (m_socket == nullptr ? tr("Socket is not initialized")
                                : (m_socket->error() == QTcpSocket::UnknownSocketError ? QString::null
                                                                                       : m_socket->errorString()));
}

void TransportPrivate::start()
{
    if (m_socket == nullptr)
    {
        m_socket = new QTcpSocket(this);
        QObject::connect(m_socket, &QTcpSocket::connected,
                         this, &TransportPrivate::connected);
        QObject::connect(m_socket, &QTcpSocket::disconnected,
                         this, &TransportPrivate::disconnected);
        QObject::connect(m_socket, static_cast<void (QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error),
                         [this](QTcpSocket::SocketError) { emit error(); });

        QObject::connect(m_socket, &QTcpSocket::readyRead,
                         this, &TransportPrivate::slotOnReceive);
    }
    else if (m_socket->state() == QTcpSocket::ConnectedState)
    {
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected();
    }

    m_socket->connectToHost(m_address, m_port);
}

void TransportPrivate::stop()
{
    if (m_socket != nullptr)
    {
        m_socket->close();
    }
}

void TransportPrivate::slotOnReceive()
{
    Q_CHECK_PTR(m_socket);

    emit received(m_socket->readAll());
}

void TransportPrivate::send(const QByteArray& data)
{
    if (m_socket == nullptr)
    {
        return;
    }
    else if (m_socket->state() != QTcpSocket::ConnectedState)
    {
        emit error();
        return;
    }

    m_socket->write(data);
    m_socket->waitForBytesWritten();
    emit sent(data);
}

void TransportPrivate::send(QByteArray&& data)
{
    send(const_cast<const QByteArray&>(data));
}

} // details
} // ioservice
