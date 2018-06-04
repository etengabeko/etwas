#include "settings.h"
#include "settings_private.h"

namespace settings
{

Settings::Settings() :
    m_pimpl(new details::SettingsPrivate())
{

}

Settings::~Settings() noexcept
{
    m_pimpl.reset();
}

const QHostAddress& Settings::address() const noexcept
{
    return m_pimpl->address();
}

void Settings::setAddress(const QHostAddress& addr)
{
    m_pimpl->setAddress(addr);
}

void Settings::setAddress(QHostAddress&& addr) noexcept
{
    m_pimpl->setAddress(std::forward<QHostAddress>(addr));
}

quint16 Settings::port() const noexcept
{
    return m_pimpl->port();
}

void Settings::setPort(quint16 portnum) noexcept
{
    m_pimpl->setPort(portnum);
}

} // settings
