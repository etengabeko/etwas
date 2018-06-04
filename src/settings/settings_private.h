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
    ~SettingsPrivate() noexcept = default;
    // TODO

    const QHostAddress& address() const noexcept;
    void setAddress(const QHostAddress& addr);
    void setAddress(QHostAddress&& addr) noexcept;

    quint16 port() const noexcept;
    void setPort(quint16 port) noexcept;

private:
    QHostAddress m_address;
    quint16 m_port;

};

} // details
} // settings

#endif // SETTINGS_SETTINGS_PRIVATE_H
