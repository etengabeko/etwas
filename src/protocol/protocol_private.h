#ifndef PROTOCOL_PROTOCOL_PRIVATE_H
#define PROTOCOL_PROTOCOL_PRIVATE_H

#include <QtGlobal>
#include <QPair>
#include <QRgb>
#include <QString>
#include <QVector>

#include "types.h"

namespace protocol
{
namespace details
{

class PingMessagePrivate
{
public:

private:

};

class DeviceIdentityMessagePrivate
{
public:
    quint8 firmwareVersion() const NOEXCEPT;
    void setFirmwareVersion(quint8 version) NOEXCEPT;

    const QVector<quint8> buttonsNumbers() const;
    void setButtonsNumbers(const QVector<quint8>& numbers);
    void setButtonsNumbers(QVector<quint8>&& numbers) NOEXCEPT;

private:
    quint8 m_firmwareVersion = 0;
    QVector<quint8> m_buttonsNumbers;

};

class ButtonsStateMessagePrivate
{
public:
    const QVector<ButtonState> buttonsStates() const;
    void setButtonsStates(const QVector<ButtonState>& states);
    void setButtonsStates(QVector<ButtonState>&& states) NOEXCEPT;

    quint8 bitByteShift(quint8 bitNumber) const NOEXCEPT;

    static quint8 maxButtonsStatesCount() NOEXCEPT;
    static void setMaxButtonsStatesCount(quint8 count) NOEXCEPT;

private:
    QVector<ButtonState> m_states;
    static quint8 m_maxButtonsCount;

};

class BeginLogMessagePrivate
{
public:
    quint32 count() const NOEXCEPT;
    void setCount(quint32 num) NOEXCEPT;

private:
    quint32 m_count = 0;
};

class NextLogMessagePrivate
{
public:
    const QByteArray& data() const NOEXCEPT;
    void setData(const QByteArray& content);

private:
    QByteArray m_data;

};

class EndLogMessagePrivate
{
public:

private:

};

class DeviceAddressMessagePrivate
{
public:
    const QString address() const;
    void setAddress(const QString& addr);

    quint16 port() const NOEXCEPT;
    void setPort(quint16 num) NOEXCEPT;

private:
    QString m_address;
    quint16 m_port = 0;

};

class DisplayImagesMessagePrivate
{
public:
    quint8 displayNumber() const NOEXCEPT;
    void setDisplayNumber(quint8 num) NOEXCEPT;

    quint8 firstImageNumber() const NOEXCEPT;
    void setFirstImageNumber(quint8 num) NOEXCEPT;

    quint8 secondImageNumber() const NOEXCEPT;
    void setSecondImageNumber(quint8 num) NOEXCEPT;

private:
    quint8 m_displayNum = 0;

    quint8 m_firstImageNum = 0;
    quint8 m_secondImageNum = 0;

};

class DisplayOptionsMessagePrivate
{
public:
    quint8 displayNumber() const NOEXCEPT;
    void setDisplayNumber(quint8 num) NOEXCEPT;

    ImageSelection imageSelection() const NOEXCEPT;
    void setImageSelection(ImageSelection selection) NOEXCEPT;

    BlinkState blinkState() const NOEXCEPT;
    void setBlinkState(BlinkState blink) NOEXCEPT;

private:
    quint8 m_displayNum = 0;

    ImageSelection m_selection = ImageSelection::Nothing;
    BlinkState m_blinking = BlinkState::Off;

};

class BlinkOptionsMessagePrivate
{
public:
    quint8 displayNumber() const NOEXCEPT;
    void setDisplayNumber(quint8 num) NOEXCEPT;

    quint8 timeOn() const NOEXCEPT;
    void setTimeOn(quint8 msec10) NOEXCEPT;

    quint8 timeOff() const NOEXCEPT;
    void setTimeOff(quint8 msec10) NOEXCEPT;

private:
    quint8 m_displayNum = 0;

    quint16 m_timeOnMsec = 0;
    quint16 m_timeOffMsec = 0;
};

class BrightOptionsMessagePrivate
{
public:
    quint8 displayNumber() const NOEXCEPT;
    void setDisplayNumber(quint8 num) NOEXCEPT;

    quint8 brightLevel() const NOEXCEPT;
    void setBrightLevel(quint8 bright) NOEXCEPT;

private:
    quint8 m_displayNum = 0;

    quint8 m_bright = static_cast<quint8>(BrightLevel::Min);

};

class ImageDataMessagePrivate
{
public:
    quint8 imageNumber() const NOEXCEPT;
    void setImageNumber(quint8 num) NOEXCEPT;

    const QVector<QRgb> imageColors() const;
    void setImageColors(const QVector<QRgb>& colors);
    void setImageColors(QVector<QRgb>&& colors) NOEXCEPT;

    QPair<quint8, quint8> rgbTo16bit(const QRgb& color) const;
    QRgb rgbFrom16bit(const QPair<quint8, quint8>& bits) const;

private:
    quint8 m_imageNum = 0;

    QVector<QRgb> m_imageColors;

};

class SendLogMessagePrivate
{
public:

private:

};

class ClearLogMessagePrivate
{
public:

private:

};

class CurrentTimeMessagePrivate
{
public:
    quint32 currentTime() const NOEXCEPT;
    void setCurrentTime(quint32 epoch) NOEXCEPT;

private:
    quint32 m_currentTime = 0;

};

} // details
} // protocol

#endif // PROTOCOL_PROTOCOL_PRIVATE_H
