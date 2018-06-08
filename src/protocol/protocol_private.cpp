#include "protocol_private.h"

namespace protocol
{
namespace details
{

quint8 DeviceIdentityMessagePrivate::firmwareVersion() const NOEXCEPT
{
    return m_firmwareVersion;
}

void DeviceIdentityMessagePrivate::setFirmwareVersion(quint8 version) NOEXCEPT
{
    m_firmwareVersion = version;
}

const QVector<quint8> DeviceIdentityMessagePrivate::buttonsNumbers() const
{
    return m_buttonsNumbers;
}

void DeviceIdentityMessagePrivate::setButtonsNumbers(const QVector<quint8>& numbers)
{
    m_buttonsNumbers = numbers;
}

void DeviceIdentityMessagePrivate::setButtonsNumbers(QVector<quint8>&& numbers) NOEXCEPT
{
    m_buttonsNumbers.swap(numbers);
}

const QVector<ButtonState> ButtonsStateMessagePrivate::buttonsStates() const
{
    return m_states;
}

void ButtonsStateMessagePrivate::setButtonsStates(const QVector<ButtonState>& states)
{
    m_states = states;
}

void ButtonsStateMessagePrivate::setButtonsStates(QVector<ButtonState>&& states) NOEXCEPT
{
    m_states.swap(states);
}

const QString DeviceAddressMessagePrivate::address() const
{
    return m_address;
}

void DeviceAddressMessagePrivate::setAddress(const QString& addr)
{
    m_address = addr;
}

quint16 DeviceAddressMessagePrivate::port() const NOEXCEPT
{
    return m_port;
}

void DeviceAddressMessagePrivate::setPort(quint16 num) NOEXCEPT
{
    m_port = num;
}

quint8 DisplayImagesMessagePrivate::displayNumber() const NOEXCEPT
{
    return m_displayNum;
}

void DisplayImagesMessagePrivate::setDisplayNumber(quint8 num) NOEXCEPT
{
    m_displayNum = num;
}

quint8 DisplayImagesMessagePrivate::firstImageNumber() const NOEXCEPT
{
    return m_firstImageNum;
}

void DisplayImagesMessagePrivate::setFirstImageNumber(quint8 num) NOEXCEPT
{
    m_firstImageNum = num;
}

quint8 DisplayImagesMessagePrivate::secondImageNumber() const NOEXCEPT
{
    return m_secondImageNum;
}

void DisplayImagesMessagePrivate::setSecondImageNumber(quint8 num) NOEXCEPT
{
    m_secondImageNum = num;
}

quint8 DisplayOptionsMessagePrivate::displayNumber() const NOEXCEPT
{
    return m_displayNum;
}

void DisplayOptionsMessagePrivate::setDisplayNumber(quint8 num) NOEXCEPT
{
    m_displayNum = num;
}

ImageSelection DisplayOptionsMessagePrivate::imageSelection() const NOEXCEPT
{
    return m_selection;
}

void DisplayOptionsMessagePrivate::setImageSelection(ImageSelection selection) NOEXCEPT
{
    m_selection = selection;
}

BlinkState DisplayOptionsMessagePrivate::blinkState() const NOEXCEPT
{
    return m_blinking;
}

void DisplayOptionsMessagePrivate::setBlinkState(BlinkState blink) NOEXCEPT
{
    m_blinking = blink;
}

quint8 BlinkOptionsMessagePrivate::displayNumber() const NOEXCEPT
{
    return m_displayNum;
}

void BlinkOptionsMessagePrivate::setDisplayNumber(quint8 num) NOEXCEPT
{
    m_displayNum = num;
}

quint8 BlinkOptionsMessagePrivate::timeOn() const NOEXCEPT
{
    return (m_timeOnMsec / 10);
}

void BlinkOptionsMessagePrivate::setTimeOn(quint8 msec10) NOEXCEPT
{
    m_timeOnMsec = (msec10 * 10);
}

quint8 BlinkOptionsMessagePrivate::timeOff() const NOEXCEPT
{
    return (m_timeOffMsec / 10);
}

void BlinkOptionsMessagePrivate::setTimeOff(quint8 msec10) NOEXCEPT
{
    m_timeOffMsec = (msec10 * 10);
}

quint8 BrightOptionsMessagePrivate::displayNumber() const NOEXCEPT
{
    return m_displayNum;
}

void BrightOptionsMessagePrivate::setDisplayNumber(quint8 num) NOEXCEPT
{
    m_displayNum = num;
}

quint8 BrightOptionsMessagePrivate::brightLevel() const NOEXCEPT
{
    return m_bright;
}

void BrightOptionsMessagePrivate::setBrightLevel(quint8 bright) NOEXCEPT
{
    m_bright = std::min(bright, static_cast<quint8>(BrightLevel::Max));
}

quint8 ImagesDataMessagePrivate::imageNumber() const NOEXCEPT
{
    return m_imageNum;
}

void ImagesDataMessagePrivate::setImageNumber(quint8 num) NOEXCEPT
{
    m_imageNum = num;
}

const QVector<QRgb> ImagesDataMessagePrivate::imageColors() const
{
    return m_imageColors;
}

void ImagesDataMessagePrivate::setImageColors(const QVector<QRgb>& colors)
{
    m_imageColors = colors;
}

void ImagesDataMessagePrivate::setImageColors(QVector<QRgb>&& colors) NOEXCEPT
{
    m_imageColors.swap(colors);
}


} // details
} // protocol
