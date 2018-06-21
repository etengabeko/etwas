#include "transport.h"
#include "transport_private.h"

#include <functional>

#include <QByteArray>

#include "settings/settings.h"

namespace ioservice
{

Transport::Transport(const settings::Settings& settings,
                     QObject* parent) :
    QObject(parent),
    m_pimpl(new details::TransportPrivate(settings.address(),
                                          settings.port(),
                                          std::bind(&Transport::onReceived,
                                                    this,
                                                    std::placeholders::_1)))
{

}

Transport::~Transport() NOEXCEPT
{
    m_pimpl.reset();
}

QPair<bool, QString> Transport::start()
{
    return m_pimpl->start();
}

void Transport::onReceived(const QByteArray& data)
{
    emit received(data);
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
