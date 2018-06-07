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
    ~Settings() noexcept;
    // TODO

    const QHostAddress& address() const noexcept;
    void setAddress(const QHostAddress& addr);
    void setAddress(QHostAddress&& addr) noexcept;

    quint16 port() const noexcept;
    void setPort(quint16 portnum) noexcept;

private:
    std::unique_ptr<details::SettingsPrivate> m_pimpl;

};

} // settings

#endif // SETTINGS_SETTINGS_H
