#include "displaysettings.h"

#include <QString>
#include <QStringList>
#include <QMap>

using protocol::BlinkState;
using protocol::BrightLevel;
using protocol::ImageSelection;

const int settings::DisplaySettings::kInvalidValue = -1;

namespace
{

const QMap<ImageSelection, QString>& imageSelectionsMap()
{
    static const QMap<ImageSelection, QString> selections =
    {
        { ImageSelection::Nothing, "none"   },
        { ImageSelection::First,   "first"  },
        { ImageSelection::Second,  "second" },
        { ImageSelection::Both,    "both"   }
    };

    return selections;
}

const QMap<BlinkState, QString>& blinkStatesMap()
{
    static const QMap<BlinkState, QString> states =
    {
        { BlinkState::On,  "true"  },
        { BlinkState::Off, "false" }
    };

    return states;
}

}

namespace settings
{

DisplaySettings::~DisplaySettings() NOEXCEPT
{

}

bool DisplaySettings::isSetDisplayNumber() const NOEXCEPT
{
    return m_displayNumber > kInvalidValue;
}

quint8 DisplaySettings::displayNumber() const NOEXCEPT
{
    return static_cast<quint8>(m_displayNumber);
}

void DisplaySettings::setDisplayNumber(quint8 num) NOEXCEPT
{
    m_displayNumber = static_cast<int>(num);
}

void DisplaySettings::resetDisplayNumber() NOEXCEPT
{
    m_displayNumber = kInvalidValue;
}

bool DisplaySettings::isSetFirstImageNumber() const NOEXCEPT
{
    return m_firstImageNumber > kInvalidValue;
}

quint8 DisplaySettings::firstImageNumber() const NOEXCEPT
{
    return static_cast<quint8>(m_firstImageNumber);
}

void DisplaySettings::setFirstImageNumber(quint8 num) NOEXCEPT
{
    m_firstImageNumber = static_cast<int>(num);
}

void DisplaySettings::resetFirstImageNumber() NOEXCEPT
{
    m_firstImageNumber = kInvalidValue;
}

bool DisplaySettings::isSetSecondImageNumber() const NOEXCEPT
{
    return m_secondImageNumber > kInvalidValue;
}

quint8 DisplaySettings::secondImageNumber() const NOEXCEPT
{
    return static_cast<quint8>(m_secondImageNumber);
}

void DisplaySettings::setSecondImageNumber(quint8 num) NOEXCEPT
{
    m_secondImageNumber = static_cast<int>(num);
}

void DisplaySettings::resetSecondImageNumber() NOEXCEPT
{
    m_secondImageNumber = kInvalidValue;
}

bool DisplaySettings::isSetTimeOn() const NOEXCEPT
{
    return m_timeOnMsec > kInvalidValue;
}

quint16 DisplaySettings::timeOn() const NOEXCEPT
{
    return static_cast<quint16>(m_timeOnMsec);
}

void DisplaySettings::setTimeOn(quint16 msec) NOEXCEPT
{
    m_timeOnMsec = static_cast<int>(msec);
}

void DisplaySettings::resetTimeOn() NOEXCEPT
{
    m_timeOnMsec = kInvalidValue;
}

bool DisplaySettings::isSetTimeOff() const NOEXCEPT
{
    return m_timeOffMsec > kInvalidValue;
}

quint16 DisplaySettings::timeOff() const NOEXCEPT
{
    return static_cast<quint16>(m_timeOffMsec);
}

void DisplaySettings::setTimeOff(quint16 msec) NOEXCEPT
{
    m_timeOffMsec = static_cast<int>(msec);
}

void DisplaySettings::resetTimeOff() NOEXCEPT
{
    m_timeOffMsec = kInvalidValue;
}

bool DisplaySettings::isSetBrightLevel() const NOEXCEPT
{
    return m_brightLevel > kInvalidValue;
}

quint8 DisplaySettings::brightLevel() const NOEXCEPT
{
    return static_cast<quint8>(m_brightLevel);
}

void DisplaySettings::setBrightLevel(quint8 level) NOEXCEPT
{
    m_brightLevel = static_cast<int>(std::min(std::max(level, static_cast<quint8>(BrightLevel::Min)),
                                              static_cast<quint8>(BrightLevel::Max)));
}

void DisplaySettings::resetBrightLevel() NOEXCEPT
{
    m_brightLevel = kInvalidValue;
}

ImageSelection DisplaySettings::imageSelection() const NOEXCEPT
{
    return m_imageSelection;
}

void DisplaySettings::setImageSelection(ImageSelection selection) NOEXCEPT
{
    m_imageSelection = selection;
}

const QString DisplaySettings::imageSelectionToString() const
{
    return imageSelectionToString(imageSelection());
}

BlinkState DisplaySettings::blinkState() const NOEXCEPT
{
    return m_blinkState;
}

void DisplaySettings::setBlinkState(BlinkState state) NOEXCEPT
{
    m_blinkState = state;
}

const QString DisplaySettings::blinkStateToString() const
{
    return blinkStateToString(blinkState());
}

const QString DisplaySettings::imageSelectionToString(ImageSelection selection)
{
    auto founded = ::imageSelectionsMap().find(selection);
    return (founded != ::imageSelectionsMap().cend() ? founded.value()
                                                     : imageSelectionToString(ImageSelection::Nothing));
}

ImageSelection DisplaySettings::imageSelectionFromString(const QString& str, bool* ok)
{
    if (ok != nullptr)
    {
        const QStringList values = ::imageSelectionsMap().values();
        auto founded = std::find_if(values.cbegin(),
                                    values.cend(),
                                    [&str](const QString& each) { return each == str.toLower(); });
        *ok = (founded != values.cend());
    }

    return ::imageSelectionsMap().key(str.toLower(), ImageSelection::Nothing);
}

const QString DisplaySettings::blinkStateToString(BlinkState state)
{
    auto founded = ::blinkStatesMap().find(state);
    return (founded != ::blinkStatesMap().cend() ? founded.value()
                                                 : blinkStateToString(BlinkState::Off));
}

BlinkState DisplaySettings::blinkStateFromString(const QString& str, bool* ok)
{
    if (ok != nullptr)
    {
        const QStringList values = ::blinkStatesMap().values();
        auto founded = std::find_if(values.cbegin(),
                                    values.cend(),
                                    [&str](const QString& each) { return each == str.toLower(); });
        *ok = (founded != values.cend());
    }

    return ::blinkStatesMap().key(str.toLower(), BlinkState::Off);
}

} // settings
