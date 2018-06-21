#ifndef SETTINGS_SETTINGS_PRIVATE_H
#define SETTINGS_SETTINGS_PRIVATE_H

#include <QHostAddress>

namespace settings
{
namespace details
{
class SettingsPrivate
{
public:
    explicit SettingsPrivate() = default;
    ~SettingsPrivate() NOEXCEPT;
    // TODO

    const QHostAddress& address() const NOEXCEPT;
    void setAddress(const QHostAddress& addr);
    void setAddress(QHostAddress&& addr) NOEXCEPT;

    quint16 port() const NOEXCEPT;
    void setPort(quint16 port) NOEXCEPT;

private:
    QHostAddress m_address;
    quint16 m_port;

};

} // details
} // settings

#endif // SETTINGS_SETTINGS_PRIVATE_H
