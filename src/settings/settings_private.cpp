#include "settings_private.h"

#include <QSettings>

namespace
{

const QString imagesSectionKey() { return "images"; }
const QString imageFileNameKey() { return "filename"; }

const QString displaysSectionKey() { return "displays"; }
const QString displayNumberKey() { return "number"; }
const QString displayFirstImageNumberKey() { return "first_image"; }
const QString displaySecondImageNumberKey() { return "second_image"; }
const QString displayImagesSelectionKey() { return "selected_images"; }
const QString displayBlinkStateKey() { return "blinking"; }
const QString displayTimeOnKey() { return "time_on_msec"; }
const QString displayTimeOffKey() { return "time_off_msec"; }
const QString displayBrightLevelKey() { return "brightness"; }

template <typename T>
bool tryReadValue(T& dst, const QSettings& src, const QString& key)
{
    if (src.contains(key))
    {
        dst = src.value(key).value<T>();
        return true;
    }
    return false;
}

}

namespace settings
{
namespace details
{

SettingsPrivate::SettingsPrivate(const QString& fileName) :
    m_fileName(fileName)
{
    Q_ASSERT_X(!fileName.isEmpty(), "SettingsPrivate::SettingsPrivate()", "Empty settings filename");
}

SettingsPrivate::~SettingsPrivate() NOEXCEPT
{

}

void SettingsPrivate::load()
{
    QSettings config(m_fileName, QSettings::IniFormat);
    m_imagesFileNames = readImagesFileNames(config);
    m_displaysOptions = readDisplaysOptions(config);
}

QStringList SettingsPrivate::readImagesFileNames(QSettings& settings) const
{
    QStringList result;

    const int size = settings.beginReadArray(::imagesSectionKey());
    result.reserve(size);
    const QString key = ::imageFileNameKey();
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        result.append(std::move(settings.value(key).toString()));
    }
    settings.endArray();

    return result;
}

QVector<DisplaySettings> SettingsPrivate::readDisplaysOptions(QSettings& settings) const
{
    QVector<DisplaySettings> result;

    const int size = settings.beginReadArray(::displaysSectionKey());
    result.reserve(size);
    for (int i = 0; i < size; ++i)
    {
        result.append(std::move(readOneDisplayOptions(settings, i)));
    }
    settings.endArray();

    return result;
}

DisplaySettings SettingsPrivate::readOneDisplayOptions(QSettings& settings, int index) const
{
    DisplaySettings result;

    settings.setArrayIndex(index);
    int tmp = 0;
    QString str;

    if (::tryReadValue<int>(tmp, settings, ::displayNumberKey()))
    {
        result.setDisplayNumber(static_cast<quint8>(tmp));
    }
    if (::tryReadValue<int>(tmp, settings, ::displayFirstImageNumberKey()))
    {
        result.setFirstImageNumber(static_cast<quint8>(tmp));
    }
    if (::tryReadValue<int>(tmp, settings, ::displaySecondImageNumberKey()))
    {
        result.setSecondImageNumber(static_cast<quint8>(tmp));
    }
    if (::tryReadValue<QString>(str, settings, ::displayImagesSelectionKey()))
    {
        bool ok = false;
        const auto selection = DisplaySettings::imageSelectionFromString(str, &ok);
        if (ok)
        {
            result.setImageSelection(selection);
        }
    }
    if (::tryReadValue<QString>(str, settings, ::displayBlinkStateKey()))
    {
        bool ok = false;
        const auto blinking = DisplaySettings::blinkStateFromString(str, &ok);
        if (ok)
        {
            result.setBlinkState(blinking);
        }
    }
    if (::tryReadValue<int>(tmp, settings, ::displayTimeOnKey()))
    {
        result.setTimeOn(static_cast<quint16>(tmp));
    }
    if (::tryReadValue<int>(tmp, settings, ::displayTimeOffKey()))
    {
        result.setTimeOff(static_cast<quint16>(tmp));
    }
    if (::tryReadValue<int>(tmp, settings, ::displayBrightLevelKey()))
    {
        result.setBrightLevel(static_cast<quint8>(tmp));
    }

    return result;
}

bool SettingsPrivate::save() const
{
    QSettings config(m_fileName, QSettings::IniFormat);
    writeImagesFileNames(config);
    writeDisplaysOptions(config);
    config.sync();

    return (config.status() == QSettings::Status::NoError);
}

void SettingsPrivate::writeImagesFileNames(QSettings& settings) const
{
    const int size = m_imagesFileNames.size();
    settings.beginWriteArray(::imagesSectionKey(), size);
    const QString key = ::imageFileNameKey();
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue(key, m_imagesFileNames.at(i));
    }
    settings.endArray();
}

void SettingsPrivate::writeDisplaysOptions(QSettings& settings) const
{
    const int size = m_displaysOptions.size();
    settings.beginWriteArray(::displaysSectionKey(), size);
    for (int i = 0; i < size; ++i)
    {
        writeOneDisplayOptions(settings, i);
    }
    settings.endArray();
}

void SettingsPrivate::writeOneDisplayOptions(QSettings& settings, int index) const
{
    const DisplaySettings& current = m_displaysOptions.at(index);

    if (current.isSetDisplayNumber())
    {
        settings.setArrayIndex(index);
        settings.setValue(::displayNumberKey(), current.displayNumber());

        if (current.isSetFirstImageNumber())
        {
            settings.setValue(::displayFirstImageNumberKey(), current.firstImageNumber());
        }
        if (current.isSetSecondImageNumber())
        {
            settings.setValue(::displaySecondImageNumberKey(), current.secondImageNumber());
        }
        if (current.isSetTimeOn())
        {
            settings.setValue(::displayTimeOnKey(), current.timeOn());
        }
        if (current.isSetTimeOff())
        {
            settings.setValue(::displayTimeOffKey(), current.timeOff());
        }
        if (current.isSetBrightLevel())
        {
            settings.setValue(::displayBrightLevelKey(), current.brightLevel());
        }
        settings.setValue(::displayImagesSelectionKey(), current.imageSelectionToString());
        settings.setValue(::displayBlinkStateKey(), current.blinkStateToString());
    }
}

const QVector<DisplaySettings>& SettingsPrivate::displaysOptions() const NOEXCEPT
{
    return m_displaysOptions;
}

void SettingsPrivate::setDisplaysOptions(const QVector<DisplaySettings>& options)
{
    m_displaysOptions = options;
}

void SettingsPrivate::setDisplaysOptions(QVector<DisplaySettings>&& options) NOEXCEPT
{
    std::swap(m_displaysOptions, options);
}

const QStringList& SettingsPrivate::imagesFileNames() const NOEXCEPT
{
    return m_imagesFileNames;
}

void SettingsPrivate::setImagesFileNames(const QStringList& fileNames)
{
    m_imagesFileNames = fileNames;
}

void SettingsPrivate::setImagesFileNames(QStringList&& fileNames) NOEXCEPT
{
    std::swap(m_imagesFileNames, fileNames);
}

} // details
} // settings
