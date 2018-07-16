#ifndef SETTINGS_SETTINGS_PRIVATE_H
#define SETTINGS_SETTINGS_PRIVATE_H

#include <QString>
#include <QStringList>
#include <QVector>

#include "settings/displaysettings.h"

class QSettings;

namespace settings
{
namespace details
{
class SettingsPrivate
{
public:
    explicit SettingsPrivate(const QString& fileName);
    ~SettingsPrivate() NOEXCEPT;

    void load();
    bool save() const;

    const QVector<DisplaySettings>& displaysOptions() const NOEXCEPT;
    void setDisplaysOptions(const QVector<DisplaySettings>& options);
    void setDisplaysOptions(QVector<DisplaySettings>&& options) NOEXCEPT;

    const QStringList& imagesFileNames() const NOEXCEPT;
    void setImagesFileNames(const QStringList& fileNames);
    void setImagesFileNames(QStringList&& fileNames) NOEXCEPT;

private:
    QStringList readImagesFileNames(QSettings& settings) const;
    void writeImagesFileNames(QSettings& settings) const;

    QVector<DisplaySettings> readDisplaysOptions(QSettings& settings) const;
    void writeDisplaysOptions(QSettings& settings) const;

    DisplaySettings readOneDisplayOptions(QSettings& settings, int index) const;
    void writeOneDisplayOptions(QSettings& settings, int index) const;

private:
    const QString m_fileName;

    QStringList m_imagesFileNames;
    QVector<DisplaySettings> m_displaysOptions;

};

} // details
} // settings

#endif // SETTINGS_SETTINGS_PRIVATE_H
