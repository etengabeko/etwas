#include "transport.h"
#include "transport_private.h"

#include <QByteArray>
#include <QHostAddress>
#include <QString>

#include "asyncqueue.h"

namespace ioservice
{

Transport::Transport(const QHostAddress& address,
                     quint16 port,
                     QObject* parent) :
    QObject(parent),
    m_pimpl(new details::TransportPrivate(address, port, this))
{
    QObject::connect(m_pimpl, &details::TransportPrivate::connected,
                     this, &Transport::connected);
    QObject::connect(m_pimpl, &details::TransportPrivate::disconnected,
                     this, &Transport::disconnected);
    QObject::connect(m_pimpl, &details::TransportPrivate::error,
                     this, &Transport::error);
    QObject::connect(m_pimpl, &details::TransportPrivate::received,
                     this, &Transport::received);
    QObject::connect(m_pimpl, &details::TransportPrivate::sent,
                     this, &Transport::sent);
}

Transport::~Transport() NOEXCEPT
{
    delete m_pimpl;
    m_pimpl = nullptr;
}

const QHostAddress& Transport::currentAddress() const NOEXCEPT
{
    return m_pimpl->address();
}

quint16 Transport::currentPort() const NOEXCEPT
{
    return m_pimpl->port();
}

const QString Transport::errorString() const
{
    return m_pimpl->errorString();
}

void Transport::setDataQueue(AsyncQueue<QByteArray>* queue) NOEXCEPT
{
    m_pimpl->setDataQueue(queue);
}

void Transport::start()
{
    m_pimpl->start();
}

void Transport::stop()
{
    m_pimpl->stop();
}

void Transport::send(const QByteArray& data)
{
    m_pimpl->send(data);
}

void Transport::send(QByteArray&& data)
{
    m_pimpl->send(std::forward<QByteArray>(data));
}

void Transport::sendQueuedData()
{
    m_pimpl->sendQueuedData();
}

} // ioservice
