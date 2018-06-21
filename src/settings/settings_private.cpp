#include "settings_private.h"

namespace settings
{
namespace details
{

SettingsPrivate::~SettingsPrivate() NOEXCEPT
{

}

const QHostAddress& SettingsPrivate::address() const NOEXCEPT
{
    return m_address;
}

void SettingsPrivate::setAddress(const QHostAddress& addr)
{
    m_address = addr;
}

void SettingsPrivate::setAddress(QHostAddress&& addr) NOEXCEPT
{
    m_address.swap(addr);
}

quint16 SettingsPrivate::port() const NOEXCEPT
{
    return m_port;
}

void SettingsPrivate::setPort(quint16 port) NOEXCEPT
{
    m_port = port;
}

} // details
} // settings
