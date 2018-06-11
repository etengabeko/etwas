#include "protocol.h"

#include <QCoreApplication>
#include <QDataStream>
#include <QMap>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QVector>

#include "protocol_private.h"
#include "types.h"

namespace
{

template <typename T>
const QString typeToHex(T type)
{
    return QString("0x%1")
           .arg(QString::number(static_cast<quint8>(type), 16).toUpper());
}

int maxButtonsStatesCount() { return 32; }
int addressStringSize() { return 15; }

}

namespace protocol
{

AbstractMessage::AbstractMessage(MessageDirection direction) NOEXCEPT
    : m_direction(direction)
{

}

AbstractMessage::~AbstractMessage() NOEXCEPT
{

}

std::unique_ptr<AbstractMessage> AbstractMessage::deserialize(MessageDirection direction,
                                                              const QByteArray& content)
{
    std::unique_ptr<AbstractMessage> result;

    switch (direction)
    {
    case MessageDirection::Incoming:
        result.reset(incoming::Message::deserialize(content).release());
        break;
    case MessageDirection::Outcoming:
        result.reset(outcoming::Message::deserialize(content).release());
        break;
    case MessageDirection::Unknown:
    default:
        Q_ASSERT_X(false, "AbstractMessage::deserialize", "Unknown message direction");
        break;
    }
    return result;
}

MessageDirection AbstractMessage::direction() const NOEXCEPT
{
    return m_direction;
}

QDataStream& operator <<(QDataStream& dst, const AbstractMessage& src)
{
    const QByteArray content = src.serialize();
    dst << static_cast<quint16>(content.size());
    dst.writeRawData(content.constData(), content.size());

    return dst;
}

QDataStream& operator >>(QDataStream& src, AbstractMessage& dst)
{
    quint16 size = 0;
    src >> size;
    if (size > 0)
    {
        QByteArray content(size, '\0');
        src.readRawData(content.data(), size);
        dst.parse(content);
    }
    return src;
}

namespace incoming
{

const QString typeToString(MessageType type)
{
    static const QMap<MessageType, QString> types
    {
        { MessageType::DeviceIdentity, qApp->tr("DeviceIdentity = %1")
                    .arg(::typeToHex(MessageType::DeviceIdentity)) },
        { MessageType::ButtonsState,   qApp->tr("ButtonsState = %1")
                    .arg(::typeToHex(MessageType::ButtonsState)) }
    };

    auto founded = types.find(type);
    return (founded != types.cend() ? founded.value()
                                    : qApp->tr("Unknown incoming type = %1").arg(::typeToHex(type)));
}

Message::Message(MessageType type) NOEXCEPT :
    AbstractMessage(MessageDirection::Incoming),
    m_type(type)
{

}

Message::~Message() NOEXCEPT
{

}

Message::Message(const Message& other) NOEXCEPT :
    AbstractMessage(other),
    m_type(other.m_type)
{

}

std::unique_ptr<Message> Message::deserialize(const QByteArray& content)
{
    std::unique_ptr<Message> result;

    if (!content.isEmpty())
    {
        quint8 type = 0;
        {
            QDataStream in(content);
            in.setByteOrder(QDataStream::BigEndian);
            in >> type;
        }

        switch (static_cast<MessageType>(type))
        {
        case MessageType::DeviceIdentity:
            result.reset(new DeviceIdentityMessage());
            break;
        case MessageType::ButtonsState:
            result.reset(new ButtonsStateMessage());
            break;
        case MessageType::Unknown:
        default:
            Q_ASSERT_X(false, "incoming::Message::deserialize", "Unknown message type");
            break;
        }
    }

    if (result != nullptr)
    {
        result->parse(content);
    }
    return result;
}

MessageType Message::type() const NOEXCEPT
{
    return m_type;
}

DeviceIdentityMessage::DeviceIdentityMessage() :
    Message(MessageType::DeviceIdentity),
    m_pimpl(new details::DeviceIdentityMessagePrivate())
{

}

DeviceIdentityMessage::~DeviceIdentityMessage() NOEXCEPT
{
    m_pimpl.reset();
}

DeviceIdentityMessage::DeviceIdentityMessage(const DeviceIdentityMessage& other) :
    Message(other),
    m_pimpl(new details::DeviceIdentityMessagePrivate(*other.m_pimpl))
{

}

DeviceIdentityMessage& DeviceIdentityMessage::operator =(const DeviceIdentityMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

DeviceIdentityMessage::DeviceIdentityMessage(DeviceIdentityMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

DeviceIdentityMessage& DeviceIdentityMessage::operator =(DeviceIdentityMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray DeviceIdentityMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint8>(firmwareVersion())
        << static_cast<quint16>(buttonsNumbers().size());
    for (const auto& each : buttonsNumbers())
    {
        out << static_cast<quint8>(each);
    }
    return result;
}

bool DeviceIdentityMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::BigEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (tmp == static_cast<quint8>(type()));
    if (ok)
    {
        in >> tmp;
        setFirmwareVersion(tmp);
        quint16 buttonsCount = 0;
        in >> buttonsCount;
        if (buttonsCount > 0)
        {
            QVector<quint8> buttons(buttonsCount, 0);
            for (size_t i = 0; i < buttonsCount; ++i)
            {
                in >> buttons[static_cast<int>(i)];
            }
            setButtonsNumbers(std::move(buttons));
        }
    }
    return ok;
}

quint8 DeviceIdentityMessage::firmwareVersion() const NOEXCEPT
{
    return m_pimpl->firmwareVersion();
}

void DeviceIdentityMessage::setFirmwareVersion(quint8 version) NOEXCEPT
{
    m_pimpl->setFirmwareVersion(version);
}

const QVector<quint8> DeviceIdentityMessage::buttonsNumbers() const
{
    return m_pimpl->buttonsNumbers();
}

void DeviceIdentityMessage::setButtonsNumbers(const QVector<quint8>& numbers)
{
    m_pimpl->setButtonsNumbers(numbers);
}

void DeviceIdentityMessage::setButtonsNumbers(QVector<quint8>&& numbers)
{
    m_pimpl->setButtonsNumbers(std::forward<QVector<quint8>>(numbers));
}

ButtonsStateMessage::ButtonsStateMessage() :
    Message(MessageType::ButtonsState),
    m_pimpl(new details::ButtonsStateMessagePrivate())
{

}

ButtonsStateMessage::~ButtonsStateMessage() NOEXCEPT
{
    m_pimpl.reset();
}

ButtonsStateMessage::ButtonsStateMessage(const ButtonsStateMessage& other) :
    Message(other),
    m_pimpl(new details::ButtonsStateMessagePrivate(*other.m_pimpl))
{

}

ButtonsStateMessage& ButtonsStateMessage::operator =(const ButtonsStateMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

ButtonsStateMessage::ButtonsStateMessage(ButtonsStateMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

ButtonsStateMessage& ButtonsStateMessage::operator =(ButtonsStateMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray ButtonsStateMessage::serialize() const
{
    QByteArray states(::maxButtonsStatesCount(), '\0');
    for (uint i = 0, sz = buttonsStates().size(); i < sz; ++i)
    {
        quint8 eachState = static_cast<quint8>(buttonsStates().at(i));
        quint8 currentByte = states[i/8];
        states[i/8] = currentByte | (eachState << (i%8));
    }

    QByteArray result;
    {
        QDataStream out(&result, QIODevice::WriteOnly);
        out.setByteOrder(QDataStream::BigEndian);
        out << static_cast<quint8>(m_type);
    }
    result += states;

    return result;
}

bool ButtonsStateMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::BigEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (   tmp == static_cast<quint8>(type())
               && src.size() >= static_cast<int>(::maxButtonsStatesCount() + sizeof(tmp)));
    if (ok)
    {
        QVector<quint8> bytes(::maxButtonsStatesCount(),'\0');
        {
            QByteArray ba(::maxButtonsStatesCount(), '\0');
            in.readRawData(ba.data(), bytes.size());
            std::copy(ba.begin(), ba.end(), bytes.begin());
        }

        QVector<ButtonState> states(8 * ::maxButtonsStatesCount(), ButtonState::Off);
        for (int i = 0, sz = 8 * ::maxButtonsStatesCount(); i < sz; ++i)
        {
            const quint8 mask = 1 << (i%8);
            quint8 currentByte = bytes.at(i/8);
            states[i] = (currentByte & mask) == 0 ? ButtonState::Off
                                                  : ButtonState::On;
        }

        setButtonsStates(std::move(states));
    }
    return ok;
}

const QVector<ButtonState> ButtonsStateMessage::buttonsStates() const
{
    return m_pimpl->buttonsStates();
}

void ButtonsStateMessage::setButtonsStates(QVector<ButtonState>&& states)
{
    m_pimpl->setButtonsStates(std::forward<QVector<ButtonState>>(states));
}

} // incoming

namespace outcoming
{
const QString typeToString(MessageType type)
{
    static const QMap<MessageType, QString> types
    {
        { MessageType::DeviceAddress,  qApp->tr("DeviceAddress = %1")
                    .arg(::typeToHex(MessageType::DeviceAddress)) },
        { MessageType::DisplayImages,  qApp->tr("DisplayImages = %1")
                    .arg(::typeToHex(MessageType::DisplayImages)) },
        { MessageType::DisplayOptions, qApp->tr("DisplayOptions = %1")
                    .arg(::typeToHex(MessageType::DisplayOptions)) },
        { MessageType::BlinkOptions,   qApp->tr("BlinkOptions = %1")
                    .arg(::typeToHex(MessageType::BlinkOptions)) },
        { MessageType::BrightOptions,  qApp->tr("BrightOptions = %1")
                    .arg(::typeToHex(MessageType::BrightOptions)) },
        { MessageType::ImagesData,     qApp->tr("ImagesData = %1")
                    .arg(::typeToHex(MessageType::ImagesData)) }
    };

    auto founded = types.find(type);
    return (founded != types.cend() ? founded.value()
                                    : qApp->tr("Unknown outcoming type = %1").arg(::typeToHex(type)));
}

Message::Message(MessageType type) NOEXCEPT :
    AbstractMessage(MessageDirection::Outcoming),
    m_type(type)
{

}

Message::~Message() NOEXCEPT
{

}

Message::Message(const Message& other) NOEXCEPT :
    AbstractMessage(other),
    m_type(other.m_type)
{

}

std::unique_ptr<Message> Message::deserialize(const QByteArray& content)
{
    std::unique_ptr<Message> result;

    if (!content.isEmpty())
    {
        quint8 type = 0;
        {
            QDataStream in(content);
            in.setByteOrder(QDataStream::BigEndian);
            in >> type;
        }

        switch (static_cast<MessageType>(type))
        {
        case MessageType::DeviceAddress:
            result.reset(new DeviceAddressMessage());
            break;
        case MessageType::DisplayImages:
            result.reset(new DisplayImagesMessage());
            break;
        case MessageType::DisplayOptions:
            result.reset(new DisplayOptionsMessage());
            break;
        case MessageType::BlinkOptions:
            result.reset(new BlinkOptionsMessage());
            break;
        case MessageType::BrightOptions:
            result.reset(new BrightOptionsMessage());
            break;
        case MessageType::ImagesData:
            result.reset(new ImagesDataMessage());
            break;
        case MessageType::Unknown:
        default:
            Q_ASSERT_X(false, "outcoming::Message::deserialize", "Unknown message type");
            break;
        }
    }

    if (result != nullptr)
    {
        result->parse(content);
    }
    return result;
}

MessageType Message::type() const NOEXCEPT
{
    return m_type;
}

DeviceAddressMessage::DeviceAddressMessage() :
    Message(MessageType::DeviceAddress),
    m_pimpl(new details::DeviceAddressMessagePrivate())
{

}

DeviceAddressMessage::~DeviceAddressMessage() NOEXCEPT
{
    m_pimpl.reset();
}

DeviceAddressMessage::DeviceAddressMessage(const DeviceAddressMessage& other) :
    Message(other),
    m_pimpl(new details::DeviceAddressMessagePrivate(*other.m_pimpl))
{

}

DeviceAddressMessage& DeviceAddressMessage::operator =(const DeviceAddressMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

DeviceAddressMessage::DeviceAddressMessage(DeviceAddressMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

DeviceAddressMessage& DeviceAddressMessage::operator =(DeviceAddressMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray DeviceAddressMessage::serialize() const
{
    static const QRegularExpression re("(?:[\\d]{1,3}\\.){3}[\\d]{1,3}");

    QByteArray result;
    {
        QDataStream out(&result, QIODevice::WriteOnly);
        out.setByteOrder(QDataStream::BigEndian);
        out << static_cast<quint8>(m_type);
        if (re.match(address()).hasMatch())
        {
            for (const QString& each : address().split('.'))
            {
                out << static_cast<quint8>(each.toUShort());
            }
        }
        else
        {
            for (size_t i = 0; i < 4; ++i)
            {
                out << static_cast<quint8>(0);
            }
        }
        out << static_cast<quint16>(port());
    }
    return result;
}

bool DeviceAddressMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::BigEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (   tmp == static_cast<quint8>(type())
               && static_cast<size_t>(src.size()) >= (4 * sizeof(quint8) + sizeof(quint16)));
    if (ok)
    {
        QString strAddress;
        strAddress.reserve(::addressStringSize());
        for (size_t i = 0; i < 4; ++i)
        {
            in >> tmp;
            if (i > 0)
            {
                strAddress += '.';
            }
            strAddress += QString::number(tmp);
        }
        quint16 portNum = 0;
        in >> portNum;

        setAddress(strAddress);
        setPort(portNum);
    }
    return ok;
}

const QString DeviceAddressMessage::address() const
{
    return m_pimpl->address();
}

void DeviceAddressMessage::setAddress(const QString& addr)
{
    m_pimpl->setAddress(addr);
}

quint16 DeviceAddressMessage::port() const NOEXCEPT
{
    return m_pimpl->port();
}

void DeviceAddressMessage::setPort(quint16 num) NOEXCEPT
{
    m_pimpl->setPort(num);
}

DisplayImagesMessage::DisplayImagesMessage() :
    Message(MessageType::DisplayImages),
    m_pimpl(new details::DisplayImagesMessagePrivate())
{

}

DisplayImagesMessage::~DisplayImagesMessage() NOEXCEPT
{
    m_pimpl.reset();
}

DisplayImagesMessage::DisplayImagesMessage(const DisplayImagesMessage& other) :
    Message(other),
    m_pimpl(new details::DisplayImagesMessagePrivate(*other.m_pimpl))
{

}

DisplayImagesMessage& DisplayImagesMessage::operator =(const DisplayImagesMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

DisplayImagesMessage::DisplayImagesMessage(DisplayImagesMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

DisplayImagesMessage& DisplayImagesMessage::operator =(DisplayImagesMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray DisplayImagesMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint8>(displayNumber())
        << static_cast<quint16>(firstImageNumber())
        << static_cast<quint16>(secondImageNumber());

    return result;
}

bool DisplayImagesMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::BigEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (tmp == static_cast<quint8>(type()));
    if (ok)
    {
        in >> tmp;
        setDisplayNumber(tmp);

        in >> tmp;
        setFirstImageNumber(tmp);

        in >> tmp;
        setSecondImageNumber(tmp);
    }
    return ok;
}

quint8 DisplayImagesMessage::displayNumber() const NOEXCEPT
{
    return m_pimpl->displayNumber();
}

void DisplayImagesMessage::setDisplayNumber(quint8 num) NOEXCEPT
{
    m_pimpl->setDisplayNumber(num);
}

quint8 DisplayImagesMessage::firstImageNumber() const NOEXCEPT
{
    return m_pimpl->firstImageNumber();
}

void DisplayImagesMessage::setFirstImageNumber(quint8 num) NOEXCEPT
{
    m_pimpl->setFirstImageNumber(num);
}

quint8 DisplayImagesMessage::secondImageNumber() const NOEXCEPT
{
    return m_pimpl->secondImageNumber();
}

void DisplayImagesMessage::setSecondImageNumber(quint8 num) NOEXCEPT
{
    m_pimpl->setSecondImageNumber(num);
}

DisplayOptionsMessage::DisplayOptionsMessage() :
    Message(MessageType::DisplayOptions),
    m_pimpl(new details::DisplayOptionsMessagePrivate())
{

}

DisplayOptionsMessage::~DisplayOptionsMessage() NOEXCEPT
{
    m_pimpl.reset();
}

DisplayOptionsMessage::DisplayOptionsMessage(const DisplayOptionsMessage& other) :
    Message(other),
    m_pimpl(new details::DisplayOptionsMessagePrivate(*other.m_pimpl))
{

}

DisplayOptionsMessage& DisplayOptionsMessage::operator =(const DisplayOptionsMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

DisplayOptionsMessage::DisplayOptionsMessage(DisplayOptionsMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

DisplayOptionsMessage& DisplayOptionsMessage::operator =(DisplayOptionsMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray DisplayOptionsMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint8>(displayNumber())
        << static_cast<quint8>(imageSelection())
        << static_cast<quint8>(blinkState());

    return result;
}

bool DisplayOptionsMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::BigEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (tmp == static_cast<quint8>(type()));
    if (ok)
    {
        in >> tmp;
        setDisplayNumber(tmp);

        in >> tmp;
        ok = (tmp < static_cast<quint8>(ImageSelection::End)) && ok;
        if (ok)
        {
            setImageSelection(static_cast<ImageSelection>(tmp));
        }

        in >> tmp;
        ok = (tmp < static_cast<quint8>(BlinkState::End)) && ok;
        if (ok)
        {
            setBlinkState(static_cast<BlinkState>(tmp));
        }
    }
    return ok;
}

quint8 DisplayOptionsMessage::displayNumber() const NOEXCEPT
{
    return m_pimpl->displayNumber();
}

void DisplayOptionsMessage::setDisplayNumber(quint8 num) NOEXCEPT
{
    m_pimpl->setDisplayNumber(num);
}

ImageSelection DisplayOptionsMessage::imageSelection() const NOEXCEPT
{
    return m_pimpl->imageSelection();
}

void DisplayOptionsMessage::setImageSelection(ImageSelection selection) NOEXCEPT
{
    m_pimpl->setImageSelection(selection);
}

BlinkState DisplayOptionsMessage::blinkState() const NOEXCEPT
{
    return m_pimpl->blinkState();
}

void DisplayOptionsMessage::setBlinkState(BlinkState blink) NOEXCEPT
{
    m_pimpl->setBlinkState(blink);
}

BlinkOptionsMessage::BlinkOptionsMessage() :
    Message(MessageType::BlinkOptions),
    m_pimpl(new details::BlinkOptionsMessagePrivate())
{

}

BlinkOptionsMessage::~BlinkOptionsMessage() NOEXCEPT
{
    m_pimpl.reset();
}

BlinkOptionsMessage::BlinkOptionsMessage(const BlinkOptionsMessage& other) :
    Message(other),
    m_pimpl(new details::BlinkOptionsMessagePrivate(*other.m_pimpl))
{

}

BlinkOptionsMessage& BlinkOptionsMessage::operator =(const BlinkOptionsMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

BlinkOptionsMessage::BlinkOptionsMessage(BlinkOptionsMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

BlinkOptionsMessage& BlinkOptionsMessage::operator =(BlinkOptionsMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray BlinkOptionsMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint8>(displayNumber())
        << static_cast<quint8>(timeOn())
        << static_cast<quint8>(timeOff());

    return result;
}

bool BlinkOptionsMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::BigEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (tmp == static_cast<quint8>(type()));
    if (ok)
    {
        in >> tmp;
        setDisplayNumber(tmp);

        in >> tmp;
        setTimeOn(tmp);

        in >> tmp;
        setTimeOff(tmp);
    }
    return ok;
}

quint8 BlinkOptionsMessage::displayNumber() const NOEXCEPT
{
    return m_pimpl->displayNumber();
}

void BlinkOptionsMessage::setDisplayNumber(quint8 num) NOEXCEPT
{
    m_pimpl->setDisplayNumber(num);
}

quint8 BlinkOptionsMessage::timeOn() const NOEXCEPT
{
    return m_pimpl->timeOn();
}

void BlinkOptionsMessage::setTimeOn(quint8 msec10) NOEXCEPT
{
    m_pimpl->setTimeOn(msec10);
}

quint8 BlinkOptionsMessage::timeOff() const NOEXCEPT
{
    return m_pimpl->timeOff();
}

void BlinkOptionsMessage::setTimeOff(quint8 msec10) NOEXCEPT
{
    m_pimpl->setTimeOff(msec10);
}

BrightOptionsMessage::BrightOptionsMessage() :
    Message(MessageType::BrightOptions),
    m_pimpl(new details::BrightOptionsMessagePrivate())
{

}

BrightOptionsMessage::~BrightOptionsMessage() NOEXCEPT
{
    m_pimpl.reset();
}

BrightOptionsMessage::BrightOptionsMessage(const BrightOptionsMessage& other) :
    Message(other),
    m_pimpl(new details::BrightOptionsMessagePrivate(*other.m_pimpl))
{

}

BrightOptionsMessage& BrightOptionsMessage::operator =(const BrightOptionsMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

BrightOptionsMessage::BrightOptionsMessage(BrightOptionsMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

BrightOptionsMessage& BrightOptionsMessage::operator =(BrightOptionsMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray BrightOptionsMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint8>(displayNumber())
        << static_cast<quint8>(brightLevel());

    return result;
}

bool BrightOptionsMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::BigEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (tmp == static_cast<quint8>(type()));
    if (ok)
    {
        in >> tmp;
        setDisplayNumber(tmp);

        in >> tmp;
        ok = (tmp < static_cast<quint8>(BrightLevel::End)) && ok;
        if (ok)
        {
            setBrightLevel(tmp);
        }
    }
    return ok;
}

quint8 BrightOptionsMessage::displayNumber() const NOEXCEPT
{
    return m_pimpl->displayNumber();
}

void BrightOptionsMessage::setDisplayNumber(quint8 num) NOEXCEPT
{
    m_pimpl->setDisplayNumber(num);
}

quint8 BrightOptionsMessage::brightLevel() const NOEXCEPT
{
    return m_pimpl->brightLevel();
}

void BrightOptionsMessage::setBrightLevel(quint8 bright) NOEXCEPT
{
    m_pimpl->setBrightLevel(bright);
}

ImagesDataMessage::ImagesDataMessage() :
    Message(MessageType::ImagesData),
    m_pimpl(new details::ImagesDataMessagePrivate())
{

}

ImagesDataMessage::~ImagesDataMessage() NOEXCEPT
{
    m_pimpl.reset();
}

ImagesDataMessage::ImagesDataMessage(const ImagesDataMessage& other) :
    Message(other),
    m_pimpl(new details::ImagesDataMessagePrivate(*other.m_pimpl))
{

}

ImagesDataMessage& ImagesDataMessage::operator =(const ImagesDataMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

ImagesDataMessage::ImagesDataMessage(ImagesDataMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

ImagesDataMessage& ImagesDataMessage::operator =(ImagesDataMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray ImagesDataMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint8>(imageNumber());
    // TODO

    return result;
}

bool ImagesDataMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::BigEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (tmp == static_cast<quint8>(type()));
    if (ok)
    {
        in >> tmp;
        setImageNumber(tmp);
        // TODO
    }
    return ok;
}

quint8 ImagesDataMessage::imageNumber() const NOEXCEPT
{
    return m_pimpl->imageNumber();
}

void ImagesDataMessage::setImageNumber(quint8 num) NOEXCEPT
{
    m_pimpl->setImageNumber(num);
}

const QVector<QRgb> ImagesDataMessage::imageColors() const
{
    return m_pimpl->imageColors();
}

void ImagesDataMessage::setImageColors(QVector<QRgb>&& colors)
{
    m_pimpl->setImageColors(std::forward<QVector<QRgb>>(colors));
}

} // outcoming
} // protocol
