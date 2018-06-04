#include "settings_private.h"

namespace settings
{
namespace details
{

const QHostAddress& SettingsPrivate::address() const noexcept
{
    return m_address;
}

void SettingsPrivate::setAddress(const QHostAddress& addr)
{
    m_address = addr;
}

void SettingsPrivate::setAddress(QHostAddress&& addr) noexcept
{
    m_address.swap(addr);
}

quint16 SettingsPrivate::port() const noexcept
{
    return m_port;
}

void SettingsPrivate::setPort(quint16 port) noexcept
{
    m_port = port;
}

} // details
} // settings
