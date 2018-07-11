#ifndef SETTINGS_SETTINGS_H
#define SETTINGS_SETTINGS_H

#include <memory>

class QString;
class QStringList;
template <typename T> class QVector;

namespace settings
{
class DisplaySettings;

namespace details
{
class SettingsPrivate;
} // details

class Settings
{
public:
    explicit Settings(const QString& fileName);
    ~Settings() NOEXCEPT;

    void load();
    void save() const;

    const QVector<DisplaySettings>& displaysOptions() const NOEXCEPT;
    void setDisplaysOptions(const QVector<DisplaySettings>& options);
    void setDisplaysOptions(QVector<DisplaySettings>&& options) NOEXCEPT;

    const QStringList& imagesFileNames() const NOEXCEPT;
    void setImagesFileNames(const QStringList& fileNames);
    void setImagesFileNames(QStringList&& fileNames) NOEXCEPT;

private:
    std::unique_ptr<details::SettingsPrivate> m_pimpl;

};

} // settings

#endif // SETTINGS_SETTINGS_H
