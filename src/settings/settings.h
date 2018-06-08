#ifndef SETTINGS_SETTINGS_H
#define SETTINGS_SETTINGS_H

#include <memory>

#include <QHostAddress>

namespace settings
{
namespace details
{
class SettingsPrivate;
} // details

class Settings
{
public:
    explicit Settings();
    ~Settings() NOEXCEPT;
    // TODO

    const QHostAddress& address() const NOEXCEPT;
    void setAddress(const QHostAddress& addr);
    void setAddress(QHostAddress&& addr) NOEXCEPT;

    quint16 port() const NOEXCEPT;
    void setPort(quint16 portnum) NOEXCEPT;

private:
    std::unique_ptr<details::SettingsPrivate> m_pimpl;

};

} // settings

#endif // SETTINGS_SETTINGS_H
