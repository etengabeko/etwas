#include "settings.h"
#include "settings_private.h"

#include <QString>
#include <QStringList>
#include <QVector>

#include "settings/displaysettings.h"

namespace settings
{

Settings::Settings(const QString& fileName) :
    m_pimpl(new details::SettingsPrivate(fileName))
{

}

Settings::~Settings() NOEXCEPT
{
    m_pimpl.reset();
}

void Settings::load()
{
    m_pimpl->load();
}

bool Settings::save() const
{
    return m_pimpl->save();
}

const QVector<DisplaySettings>& Settings::displaysOptions() const NOEXCEPT
{
    return m_pimpl->displaysOptions();
}

void Settings::setDisplaysOptions(const QVector<DisplaySettings>& options)
{
    m_pimpl->setDisplaysOptions(options);
}

void Settings::setDisplaysOptions(QVector<DisplaySettings>&& options) NOEXCEPT
{
    m_pimpl->setDisplaysOptions(std::forward<QVector<DisplaySettings>>(options));
}

const QStringList& Settings::imagesFileNames() const NOEXCEPT
{
    return m_pimpl->imagesFileNames();
}

void Settings::setImagesFileNames(const QStringList& fileNames)
{
    m_pimpl->setImagesFileNames(fileNames);
}

void Settings::setImagesFileNames(QStringList&& fileNames) NOEXCEPT
{
    m_pimpl->setImagesFileNames(std::forward<QStringList>(fileNames));
}

} // settings
