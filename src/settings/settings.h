#ifndef SETTINGS_SETTINGS_H
#define SETTINGS_SETTINGS_H

#include <memory>

class QString;

namespace settings
{
namespace details
{
class SettingsPrivate;
} // details

class Settings
{
public:
    explicit Settings(const QString& fileName);
    ~Settings() NOEXCEPT;
    // TODO

private:
    std::unique_ptr<details::SettingsPrivate> m_pimpl;

};

} // settings

#endif // SETTINGS_SETTINGS_H
