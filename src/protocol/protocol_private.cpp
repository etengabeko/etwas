#include "protocol_private.h"

namespace protocol
{
namespace details
{

quint8 DeviceIdentityMessagePrivate::firmwareVersion() const noexcept
{
    return m_firmwareVersion;
}

void DeviceIdentityMessagePrivate::setFirmwareVersion(quint8 version) noexcept
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

void DeviceIdentityMessagePrivate::setButtonsNumbers(QVector<quint8>&& numbers) noexcept
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

void ButtonsStateMessagePrivate::setButtonsStates(QVector<ButtonState>&& states) noexcept
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

quint16 DeviceAddressMessagePrivate::port() const noexcept
{
    return m_port;
}

void DeviceAddressMessagePrivate::setPort(quint16 num) noexcept
{
    m_port = num;
}

quint8 DisplayImagesMessagePrivate::displayNumber() const noexcept
{
    return m_displayNum;
}

void DisplayImagesMessagePrivate::setDisplayNumber(quint8 num) noexcept
{
    m_displayNum = num;
}

quint8 DisplayImagesMessagePrivate::firstImageNumber() const noexcept
{
    return m_firstImageNum;
}

void DisplayImagesMessagePrivate::setFirstImageNumber(quint8 num) noexcept
{
    m_firstImageNum = num;
}

quint8 DisplayImagesMessagePrivate::secondImageNumber() const noexcept
{
    return m_secondImageNum;
}

void DisplayImagesMessagePrivate::setSecondImageNumber(quint8 num) noexcept
{
    m_secondImageNum = num;
}

quint8 DisplayOptionsMessagePrivate::displayNumber() const noexcept
{
    return m_displayNum;
}

void DisplayOptionsMessagePrivate::setDisplayNumber(quint8 num) noexcept
{
    m_displayNum = num;
}

ImageSelection DisplayOptionsMessagePrivate::imageSelection() const noexcept
{
    return m_selection;
}

void DisplayOptionsMessagePrivate::setImageSelection(ImageSelection selection) noexcept
{
    m_selection = selection;
}

BlinkState DisplayOptionsMessagePrivate::blinkState() const noexcept
{
    return m_blinking;
}

void DisplayOptionsMessagePrivate::setBlinkState(BlinkState blink) noexcept
{
    m_blinking = blink;
}

quint8 BlinkOptionsMessagePrivate::displayNumber() const noexcept
{
    return m_displayNum;
}

void BlinkOptionsMessagePrivate::setDisplayNumber(quint8 num) noexcept
{
    m_displayNum = num;
}

quint8 BlinkOptionsMessagePrivate::timeOn() const noexcept
{
    return (m_timeOnMsec / 10);
}

void BlinkOptionsMessagePrivate::setTimeOn(quint8 msec10) noexcept
{
    m_timeOnMsec = (msec10 * 10);
}

quint8 BlinkOptionsMessagePrivate::timeOff() const noexcept
{
    return (m_timeOffMsec / 10);
}

void BlinkOptionsMessagePrivate::setTimeOff(quint8 msec10) noexcept
{
    m_timeOffMsec = (msec10 * 10);
}

quint8 BrightOptionsMessagePrivate::displayNumber() const noexcept
{
    return m_displayNum;
}

void BrightOptionsMessagePrivate::setDisplayNumber(quint8 num) noexcept
{
    m_displayNum = num;
}

quint8 BrightOptionsMessagePrivate::brightLevel() const noexcept
{
    return m_bright;
}

void BrightOptionsMessagePrivate::setBrightLevel(quint8 bright) noexcept
{
    m_bright = std::min(bright, static_cast<quint8>(BrightLevel::Max));
}

quint8 ImagesDataMessagePrivate::imageNumber() const noexcept
{
    return m_imageNum;
}

void ImagesDataMessagePrivate::setImageNumber(quint8 num) noexcept
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

void ImagesDataMessagePrivate::setImageColors(QVector<QRgb>&& colors) noexcept
{
    m_imageColors.swap(colors);
}


} // details
} // protocol
