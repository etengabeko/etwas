#ifndef SETTINGS_DISPLAY_SETTINGS_H
#define SETTINGS_DISPLAY_SETTINGS_H

#include <QtGlobal>

#include "protocol/types.h"

namespace settings
{

class DisplaySettings
{
public:
    DisplaySettings() = default;
    ~DisplaySettings() NOEXCEPT;

    bool isSetDisplayNumber() const NOEXCEPT;
    quint8 displayNumber() const NOEXCEPT;
    void setDisplayNumber(quint8 num) NOEXCEPT;
    void resetDisplayNumber() NOEXCEPT;

    bool isSetFirstImageNumber() const NOEXCEPT;
    quint8 firstImageNumber() const NOEXCEPT;
    void setFirstImageNumber(quint8 num) NOEXCEPT;
    void resetFirstImageNumber() NOEXCEPT;

    bool isSetSecondImageNumber() const NOEXCEPT;
    quint8 secondImageNumber() const NOEXCEPT;
    void setSecondImageNumber(quint8 num) NOEXCEPT;
    void resetSecondImageNumber() NOEXCEPT;

    bool isSetTimeOn() const NOEXCEPT;
    quint16 timeOn() const NOEXCEPT;
    void setTimeOn(quint16 msec) NOEXCEPT;
    void resetTimeOn() NOEXCEPT;

    bool isSetTimeOff() const NOEXCEPT;
    quint16 timeOff() const NOEXCEPT;
    void setTimeOff(quint16 msec) NOEXCEPT;
    void resetTimeOff() NOEXCEPT;

    bool isSetBrightLevel() const NOEXCEPT;
    quint8 brightLevel() const NOEXCEPT;
    void setBrightLevel(quint8 level) NOEXCEPT;
    void resetBrightLevel() NOEXCEPT;

    protocol::ImageSelection imageSelection() const NOEXCEPT;
    void setImageSelection(protocol::ImageSelection selection) NOEXCEPT;
    const QString imageSelectionToString() const;

    protocol::BlinkState blinkState() const NOEXCEPT;
    void setBlinkState(protocol::BlinkState state) NOEXCEPT;
    const QString blinkStateToString() const;

    static const QString imageSelectionToString(protocol::ImageSelection selection);
    static protocol::ImageSelection imageSelectionFromString(const QString& str, bool* ok = nullptr);
    static const QString blinkStateToString(protocol::BlinkState state);
    static protocol::BlinkState blinkStateFromString(const QString& str, bool* ok = nullptr);

    bool operator== (const DisplaySettings& rhs) const NOEXCEPT;

private:
    static const int kInvalidValue;

    int m_displayNumber = kInvalidValue;

    int m_firstImageNumber = kInvalidValue;
    int m_secondImageNumber = kInvalidValue;

    protocol::ImageSelection m_imageSelection = protocol::ImageSelection::Nothing;
    protocol::BlinkState m_blinkState = protocol::BlinkState::Off;

    int m_timeOnMsec = kInvalidValue;
    int m_timeOffMsec = kInvalidValue;

    int m_brightLevel = kInvalidValue;

};

} // settings

#endif // SETTINGS_DISPLAY_SETTINGS_H
