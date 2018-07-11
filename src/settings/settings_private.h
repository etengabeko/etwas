#ifndef SETTINGS_SETTINGS_PRIVATE_H
#define SETTINGS_SETTINGS_PRIVATE_H

#include <QString>

namespace settings
{
namespace details
{
class SettingsPrivate
{
public:
    explicit SettingsPrivate(const QString& fileName);
    ~SettingsPrivate() NOEXCEPT;
    // TODO

private:
    const QString m_fileName;

};

} // details
} // settings

#endif // SETTINGS_SETTINGS_PRIVATE_H
