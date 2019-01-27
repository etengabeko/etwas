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
        { MessageType::Ping, QCoreApplication::translate("protocol", "Ping = %1")
                    .arg(::typeToHex(MessageType::Ping)) },
        { MessageType::DeviceIdentity, QCoreApplication::translate("protocol", "DeviceIdentity = %1")
                    .arg(::typeToHex(MessageType::DeviceIdentity)) },
        { MessageType::ButtonsState,   QCoreApplication::translate("protocol", "ButtonsState = %1")
                    .arg(::typeToHex(MessageType::ButtonsState)) },
        { MessageType::BeginLog,       QCoreApplication::translate("protocol", "BeginLog = %1")
                    .arg(::typeToHex(MessageType::BeginLog)) },
        { MessageType::NextLog,        QCoreApplication::translate("protocol", "NextLog = %1")
                    .arg(::typeToHex(MessageType::NextLog)) },
        { MessageType::EndLog,         QCoreApplication::translate("protocol", "EndLog = %1")
                    .arg(::typeToHex(MessageType::EndLog)) }
    };

    auto founded = types.find(type);
    return (founded != types.cend() ? founded.value()
                                    : QCoreApplication::translate("protocol", "Unknown incoming type = %1").arg(::typeToHex(type)));
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
            in.setByteOrder(QDataStream::LittleEndian);
            in >> type;
        }

        switch (static_cast<MessageType>(type))
        {
        case MessageType::Ping:
            result.reset(new PingMessage());
            break;
        case MessageType::DeviceIdentity:
            result.reset(new DeviceIdentityMessage());
            break;
        case MessageType::ButtonsState:
            result.reset(new ButtonsStateMessage());
            break;
        case MessageType::BeginLog:
            result.reset(new BeginLogMessage());
            break;
        case MessageType::NextLog:
            result.reset(new NextLogMessage());
            break;
        case MessageType::EndLog:
            result.reset(new EndLogMessage());
            break;
        case MessageType::Unknown:
        default:
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

PingMessage::PingMessage() :
    Message(MessageType::Ping),
    m_pimpl(new details::PingMessagePrivate())
{

}

PingMessage::~PingMessage() NOEXCEPT
{
    m_pimpl.reset();
}

PingMessage::PingMessage(const PingMessage& other) :
    Message(other),
    m_pimpl(new details::PingMessagePrivate(*other.m_pimpl))
{

}

PingMessage& PingMessage::operator =(const PingMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

PingMessage::PingMessage(PingMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

PingMessage& PingMessage::operator =(PingMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray PingMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type);
    return result;
}

bool PingMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
    quint8 tmp = 0;
    in >> tmp;

    return (tmp == static_cast<quint8>(type()));
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
    out.setByteOrder(QDataStream::LittleEndian);
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
    in.setByteOrder(QDataStream::LittleEndian);
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

quint8 ButtonsStateMessage::maxButtonsStatesCount() NOEXCEPT
{
    return details::ButtonsStateMessagePrivate::maxButtonsStatesCount();
}

void ButtonsStateMessage::setMaxButtonsStatesCount(quint8 count) NOEXCEPT
{
    details::ButtonsStateMessagePrivate::setMaxButtonsStatesCount(count);
}

const QByteArray ButtonsStateMessage::serialize() const
{
    QByteArray states(m_pimpl->maxButtonsStatesCount(), '\0');
    for (uint i = 0, sz = buttonsStates().size(); i < sz; ++i)
    {
        quint8 eachState = static_cast<quint8>(buttonsStates().at(i));
        quint8 currentByte = states[i/8];
        states[i/8] = currentByte | (eachState << m_pimpl->bitByteShift(i));
    }

    QByteArray result;
    {
        QDataStream out(&result, QIODevice::WriteOnly);
        out.setByteOrder(QDataStream::LittleEndian);
        out << static_cast<quint8>(m_type);
        for (uint i = 0, sz = states.size(); i < sz; ++i)
        {
            quint8 eachByte = states[i];
            out << eachByte;
        }
    }

    return result;
}

bool ButtonsStateMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (   tmp == static_cast<quint8>(type())
               && src.size() >= static_cast<int>(m_pimpl->maxButtonsStatesCount() + sizeof(tmp)));
    if (ok)
    {
        QVector<quint8> bytes(m_pimpl->maxButtonsStatesCount(), 0);
        for (uint i = 0, sz = bytes.size(); i < sz; ++i)
        {
            in >> tmp;
            bytes[i] = tmp;
        }

        QVector<ButtonState> states(8 * m_pimpl->maxButtonsStatesCount(), ButtonState::Off);
        for (int i = 0, sz = states.size(); i < sz; ++i)
        {
            const quint8 mask = 1 << m_pimpl->bitByteShift(i);
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

void ButtonsStateMessage::setButtonsStates(const QVector<ButtonState>& states)
{
    m_pimpl->setButtonsStates(states);
}

void ButtonsStateMessage::setButtonsStates(QVector<ButtonState>&& states) NOEXCEPT
{
    m_pimpl->setButtonsStates(std::forward<QVector<ButtonState>>(states));
}

BeginLogMessage::BeginLogMessage() :
    Message(MessageType::BeginLog),
    m_pimpl(new details::BeginLogMessagePrivate())
{

}

BeginLogMessage::~BeginLogMessage() NOEXCEPT
{
    m_pimpl.reset();
}

BeginLogMessage::BeginLogMessage(const BeginLogMessage& other) :
    Message(other),
    m_pimpl(new details::BeginLogMessagePrivate(*other.m_pimpl))
{

}

BeginLogMessage& BeginLogMessage::operator =(const BeginLogMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

BeginLogMessage::BeginLogMessage(BeginLogMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

BeginLogMessage& BeginLogMessage::operator =(BeginLogMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray BeginLogMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint32>(count());
    return result;
}

bool BeginLogMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (tmp == static_cast<quint8>(type()));
    if (ok)
    {
        quint32 num = 0;
        in >> num;
        setCount(num);
    }
    return ok;
}

quint32 BeginLogMessage::count() const NOEXCEPT
{
    return m_pimpl->count();
}

void BeginLogMessage::setCount(quint32 num) NOEXCEPT
{
    m_pimpl->setCount(num);
}

NextLogMessage::NextLogMessage() :
    Message(MessageType::NextLog),
    m_pimpl(new details::NextLogMessagePrivate())
{

}

NextLogMessage::~NextLogMessage() NOEXCEPT
{
    m_pimpl.reset();
}

NextLogMessage::NextLogMessage(const NextLogMessage& other) :
    Message(other),
    m_pimpl(new details::NextLogMessagePrivate(*other.m_pimpl))
{

}

NextLogMessage& NextLogMessage::operator =(const NextLogMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

NextLogMessage::NextLogMessage(NextLogMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

NextLogMessage& NextLogMessage::operator =(NextLogMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray NextLogMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type);
    out.writeRawData(data().constData(), data().size());
    return result;
}

bool NextLogMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (tmp == static_cast<quint8>(type()));
    if (ok)
    {
        QByteArray content(src.size() - sizeof(tmp), '\0');
        in.readRawData(content.data(), content.size());
        setData(content);
    }
    return ok;
}

const QByteArray& NextLogMessage::data() const NOEXCEPT
{
    return m_pimpl->data();
}

void NextLogMessage::setData(const QByteArray& content)
{
    m_pimpl->setData(content);
}

EndLogMessage::EndLogMessage() :
    Message(MessageType::EndLog),
    m_pimpl(new details::EndLogMessagePrivate())
{

}

EndLogMessage::~EndLogMessage() NOEXCEPT
{
    m_pimpl.reset();
}

EndLogMessage::EndLogMessage(const EndLogMessage& other) :
    Message(other),
    m_pimpl(new details::EndLogMessagePrivate(*other.m_pimpl))
{

}

EndLogMessage& EndLogMessage::operator =(const EndLogMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

EndLogMessage::EndLogMessage(EndLogMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

EndLogMessage& EndLogMessage::operator =(EndLogMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray EndLogMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type);
    return result;
}

bool EndLogMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
    quint8 tmp = 0;
    in >> tmp;

    return (tmp == static_cast<quint8>(type()));
}

} // incoming

namespace outcoming
{
const QString typeToString(MessageType type)
{
    static const QMap<MessageType, QString> types
    {
        { MessageType::Ping, QCoreApplication::translate("protocol", "Ping = %1")
                    .arg(::typeToHex(MessageType::Ping)) },
        { MessageType::DeviceAddress,  QCoreApplication::translate("protocol", "DeviceAddress = %1")
                    .arg(::typeToHex(MessageType::DeviceAddress)) },
        { MessageType::DisplayImages,  QCoreApplication::translate("protocol", "DisplayImages = %1")
                    .arg(::typeToHex(MessageType::DisplayImages)) },
        { MessageType::DisplayOptions, QCoreApplication::translate("protocol", "DisplayOptions = %1")
                    .arg(::typeToHex(MessageType::DisplayOptions)) },
        { MessageType::BlinkOptions,   QCoreApplication::translate("protocol", "BlinkOptions = %1")
                    .arg(::typeToHex(MessageType::BlinkOptions)) },
        { MessageType::BrightOptions,  QCoreApplication::translate("protocol", "BrightOptions = %1")
                    .arg(::typeToHex(MessageType::BrightOptions)) },
        { MessageType::ImageData,      QCoreApplication::translate("protocol", "ImagesData = %1")
                    .arg(::typeToHex(MessageType::ImageData)) },
        { MessageType::SendLog,        QCoreApplication::translate("protocol", "SendLog = %1")
                    .arg(::typeToHex(MessageType::SendLog)) },
        { MessageType::ClearLog,       QCoreApplication::translate("protocol", "ClearLog = %1")
                    .arg(::typeToHex(MessageType::ClearLog)) },
        { MessageType::CurrentTime,    QCoreApplication::translate("protocol", "CurrentTime = %1")
                    .arg(::typeToHex(MessageType::CurrentTime)) }
    };

    auto founded = types.find(type);
    return (founded != types.cend() ? founded.value()
                                    : QCoreApplication::translate("protocol", "Unknown outcoming type = %1").arg(::typeToHex(type)));
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
            in.setByteOrder(QDataStream::LittleEndian);
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
        case MessageType::ImageData:
            result.reset(new ImageDataMessage());
            break;
        case MessageType::SendLog:
            result.reset(new SendLogMessage());
            break;
        case MessageType::ClearLog:
            result.reset(new ClearLogMessage());
            break;
        case MessageType::CurrentTime:
            result.reset(new CurrentTimeMessage());
            break;
        case MessageType::Unknown:
        default:
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

PingMessage::PingMessage() :
    Message(MessageType::Ping),
    m_pimpl(new details::PingMessagePrivate())
{

}

PingMessage::~PingMessage() NOEXCEPT
{
    m_pimpl.reset();
}

PingMessage::PingMessage(const PingMessage& other) :
    Message(other),
    m_pimpl(new details::PingMessagePrivate(*other.m_pimpl))
{

}

PingMessage& PingMessage::operator =(const PingMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

PingMessage::PingMessage(PingMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

PingMessage& PingMessage::operator =(PingMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray PingMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type);
    return result;
}

bool PingMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
    quint8 tmp = 0;
    in >> tmp;

    return (tmp == static_cast<quint8>(type()));
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
        out.setByteOrder(QDataStream::LittleEndian);
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
    in.setByteOrder(QDataStream::LittleEndian);
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
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint8>(displayNumber())
        << static_cast<quint8>(firstImageNumber())
        << static_cast<quint8>(secondImageNumber());

    return result;
}

bool DisplayImagesMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
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
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint8>(displayNumber())
        << static_cast<quint8>(imageSelection())
        << static_cast<quint8>(blinkState());

    return result;
}

bool DisplayOptionsMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (tmp == static_cast<quint8>(type()));
    if (ok)
    {
        in >> tmp;
        setDisplayNumber(tmp);

        in >> tmp;
        ok = (tmp <= static_cast<quint8>(ImageSelection::Both)) && ok;
        if (ok)
        {
            setImageSelection(static_cast<ImageSelection>(tmp));
        }

        in >> tmp;
        ok = (tmp <= static_cast<quint8>(BlinkState::On)) && ok;
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
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint8>(displayNumber())
        << static_cast<quint8>(timeOn())
        << static_cast<quint8>(timeOff());

    return result;
}

bool BlinkOptionsMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
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
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint8>(displayNumber())
        << static_cast<quint8>(brightLevel());

    return result;
}

bool BrightOptionsMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (tmp == static_cast<quint8>(type()));
    if (ok)
    {
        in >> tmp;
        setDisplayNumber(tmp);

        in >> tmp;
        ok = (tmp <= static_cast<quint8>(BrightLevel::Max)) && ok;
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

ImageDataMessage::ImageDataMessage() :
    Message(MessageType::ImageData),
    m_pimpl(new details::ImageDataMessagePrivate())
{

}

ImageDataMessage::~ImageDataMessage() NOEXCEPT
{
    m_pimpl.reset();
}

ImageDataMessage::ImageDataMessage(const ImageDataMessage& other) :
    Message(other),
    m_pimpl(new details::ImageDataMessagePrivate(*other.m_pimpl))
{

}

ImageDataMessage& ImageDataMessage::operator =(const ImageDataMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

ImageDataMessage::ImageDataMessage(ImageDataMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

ImageDataMessage& ImageDataMessage::operator =(ImageDataMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray ImageDataMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint8>(imageNumber());
    for (const QRgb& each : imageColors())
    {
        const QPair<quint8, quint8> color16bit = m_pimpl->rgbTo16bit(each);
        out << color16bit.first << color16bit.second;
    }
    return result;
}

bool ImageDataMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (tmp == static_cast<quint8>(type()));
    if (ok)
    {
        in >> tmp;
        setImageNumber(tmp);
        QVector<QRgb> colors;
        colors.reserve(src.size()/2 - 1);
        QPair<quint8, quint8> color16bit { 0, 0 };
        while (!in.atEnd())
        {
            in >> color16bit.first >> color16bit.second;
            colors.append(m_pimpl->rgbFrom16bit(color16bit));
        }
        setImageColors(std::move(colors));
    }
    return ok;
}

quint8 ImageDataMessage::imageNumber() const NOEXCEPT
{
    return m_pimpl->imageNumber();
}

void ImageDataMessage::setImageNumber(quint8 num) NOEXCEPT
{
    m_pimpl->setImageNumber(num);
}

const QVector<QRgb> ImageDataMessage::imageColors() const
{
    return m_pimpl->imageColors();
}

void ImageDataMessage::setImageColors(const QVector<QRgb>& colors)
{
    m_pimpl->setImageColors(colors);
}

void ImageDataMessage::setImageColors(QVector<QRgb>&& colors) NOEXCEPT
{
    m_pimpl->setImageColors(std::forward<QVector<QRgb>>(colors));
}

const QByteArray ImageDataMessage::serializeWithoutData() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint8>(imageNumber());
    return result;
}

SendLogMessage::SendLogMessage() :
    Message(MessageType::SendLog),
    m_pimpl(new details::SendLogMessagePrivate())
{

}

SendLogMessage::~SendLogMessage() NOEXCEPT
{
    m_pimpl.reset();
}

SendLogMessage::SendLogMessage(const SendLogMessage& other) :
    Message(other),
    m_pimpl(new details::SendLogMessagePrivate(*other.m_pimpl))
{

}

SendLogMessage& SendLogMessage::operator =(const SendLogMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

SendLogMessage::SendLogMessage(SendLogMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

SendLogMessage& SendLogMessage::operator =(SendLogMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray SendLogMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type);
    return result;
}

bool SendLogMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
    quint8 tmp = 0;
    in >> tmp;

    return (tmp == static_cast<quint8>(type()));
}

ClearLogMessage::ClearLogMessage() :
    Message(MessageType::ClearLog),
    m_pimpl(new details::ClearLogMessagePrivate())
{

}

ClearLogMessage::~ClearLogMessage() NOEXCEPT
{
    m_pimpl.reset();
}

ClearLogMessage::ClearLogMessage(const ClearLogMessage& other) :
    Message(other),
    m_pimpl(new details::ClearLogMessagePrivate(*other.m_pimpl))
{

}

ClearLogMessage& ClearLogMessage::operator =(const ClearLogMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

ClearLogMessage::ClearLogMessage(ClearLogMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

ClearLogMessage& ClearLogMessage::operator =(ClearLogMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray ClearLogMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type);
    return result;
}

bool ClearLogMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
    quint8 tmp = 0;
    in >> tmp;

    return (tmp == static_cast<quint8>(type()));
}

CurrentTimeMessage::CurrentTimeMessage() :
    Message(MessageType::CurrentTime),
    m_pimpl(new details::CurrentTimeMessagePrivate())
{

}

CurrentTimeMessage::~CurrentTimeMessage() NOEXCEPT
{
    m_pimpl.reset();
}

CurrentTimeMessage::CurrentTimeMessage(const CurrentTimeMessage& other) :
    Message(other),
    m_pimpl(new details::CurrentTimeMessagePrivate(*other.m_pimpl))
{

}

CurrentTimeMessage& CurrentTimeMessage::operator =(const CurrentTimeMessage& other)
{
    *m_pimpl = *other.m_pimpl;
    return *this;
}

CurrentTimeMessage::CurrentTimeMessage(CurrentTimeMessage&& other) NOEXCEPT :
    Message(other),
    m_pimpl(std::move(other.m_pimpl))
{

}

CurrentTimeMessage& CurrentTimeMessage::operator =(CurrentTimeMessage&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

const QByteArray CurrentTimeMessage::serialize() const
{
    QByteArray result;

    QDataStream out(&result, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint8>(m_type)
        << static_cast<quint32>(currentTime());
    return result;
}

bool CurrentTimeMessage::parse(const QByteArray& src)
{
    QDataStream in(src);
    in.setByteOrder(QDataStream::LittleEndian);
    quint8 tmp = 0;
    in >> tmp;

    bool ok = (tmp == static_cast<quint8>(type()));
    if (ok)
    {
        quint32 epoch = 0;
        in >> epoch;
        setCurrentTime(epoch);
    }
    return ok;
}

quint32 CurrentTimeMessage::currentTime() const NOEXCEPT
{
    return m_pimpl->currentTime();
}

void CurrentTimeMessage::setCurrentTime(quint32 epoch) NOEXCEPT
{
    m_pimpl->setCurrentTime(epoch);
}

} // outcoming
} // protocol
