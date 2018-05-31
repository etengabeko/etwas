#ifndef PROTOCOL_PROTOCOL_PRIVATE_H
#define PROTOCOL_PROTOCOL_PRIVATE_H

#include <QtGlobal>
#include <QRgb>
#include <QString>
#include <QVector>

#include "types.h"

namespace protocol
{
namespace details
{

class DeviceIdentityMessagePrivate
{
public:
    quint8 firmwareVersion() const noexcept;
    void setFirmwareVersion(quint8 version) noexcept;

    const QVector<quint8> buttonsNumbers() const;
    void setButtonsNumbers(const QVector<quint8>& numbers);
    void setButtonsNumbers(QVector<quint8>&& numbers) noexcept;

private:
    quint8 m_firmwareVersion = 0;
    QVector<quint8> m_buttonsNumbers;

};

class ButtonsStateMessagePrivate
{
public:
    const QVector<ButtonState> buttonsStates() const;
    void setButtonsStates(const QVector<ButtonState>& states);
    void setButtonsStates(QVector<ButtonState>&& states) noexcept;

private:
    QVector<ButtonState> m_states;

};

class DeviceAddressMessagePrivate
{
public:
    const QString address() const;
    void setAddress(const QString& addr);

    quint16 port() const noexcept;
    void setPort(quint16 num) noexcept;

private:
    QString m_address;
    quint16 m_port = 0;

};

class DisplayImagesMessagePrivate
{
public:
    quint8 displayNumber() const noexcept;
    void setDisplayNumber(quint8 num) noexcept;

    quint8 firstImageNumber() const noexcept;
    void setFirstImageNumber(quint8 num) noexcept;

    quint8 secondImageNumber() const noexcept;
    void setSecondImageNumber(quint8 num) noexcept;

private:
    quint8 m_displayNum = 0;

    quint8 m_firstImageNum = 0;
    quint8 m_secondImageNum = 0;

};

class DisplayOptionsMessagePrivate
{
public:
    quint8 displayNumber() const noexcept;
    void setDisplayNumber(quint8 num) noexcept;

    ImageSelection imageSelection() const noexcept;
    void setImageSelection(ImageSelection selection) noexcept;

    BlinkState blinkState() const noexcept;
    void setBlinkState(BlinkState blink) noexcept;

private:
    quint8 m_displayNum = 0;

    ImageSelection m_selection = ImageSelection::Nothing;
    BlinkState m_blinking = BlinkState::Off;

};

class BlinkOptionsMessagePrivate
{
public:
    quint8 displayNumber() const noexcept;
    void setDisplayNumber(quint8 num) noexcept;

    quint8 timeOn() const noexcept;
    void setTimeOn(quint8 msec10) noexcept;

    quint8 timeOff() const noexcept;
    void setTimeOff(quint8 msec10) noexcept;

private:
    quint8 m_displayNum = 0;

    quint16 m_timeOnMsec = 0;
    quint16 m_timeOffMsec = 0;
};

class BrightOptionsMessagePrivate
{
public:
    quint8 displayNumber() const noexcept;
    void setDisplayNumber(quint8 num) noexcept;

    quint8 brightLevel() const noexcept;
    void setBrightLevel(quint8 bright) noexcept;

private:
    quint8 m_displayNum = 0;

    quint16 m_bright = static_cast<quint8>(BrightLevel::Min);

};

class ImagesDataMessagePrivate
{
public:
    quint8 imageNumber() const noexcept;
    void setImageNumber(quint8 num) noexcept;

    const QVector<QRgb> imageColors() const;
    void setImageColors(const QVector<QRgb>& colors);
    void setImageColors(QVector<QRgb>&& colors) noexcept;

private:
    quint8 m_imageNum = 0;

    QVector<QRgb> m_imageColors;

};

} // details
} // protocol

#endif // PROTOCOL_PROTOCOL_PRIVATE_H
