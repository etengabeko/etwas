#include "transport.h"
#include "transport_private.h"

#include <QByteArray>
#include <QString>

#include "settings/settings.h"

namespace ioservice
{

Transport::Transport(const settings::Settings& settings,
                     QObject* parent) :
    QObject(parent),
    m_pimpl(new details::TransportPrivate(settings.address(),
                                          settings.port(),
                                          this))
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

const settings::Settings& Transport::currentSettings() const
{
    static settings::Settings conf;
    conf.setAddress(m_pimpl->address());
    conf.setPort(m_pimpl->port());

    return conf;;
}

const QString Transport::errorString() const
{
    return m_pimpl->errorString();
}

void Transport::start()
{
    m_pimpl->start();
}

void Transport::stop()
{
    m_pimpl->stop();
}

void Transport::slotSend(const QByteArray& data)
{
    m_pimpl->send(data);
}

void Transport::slotSend(QByteArray&& data)
{
    m_pimpl->send(std::forward<QByteArray>(data));
}

} // ioservice
