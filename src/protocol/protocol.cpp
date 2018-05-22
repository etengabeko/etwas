#include "protocol.h"

#include <QCoreApplication>
#include <QDataStream>
#include <QMap>
#include <QString>

namespace
{

quint16 kDeviceIdentitySize() { return 259;  }
quint16 kButtonsStateSize()   { return 33;   }

quint16 kDeviceAddressSize()  { return 7;    }
quint16 kDisplayImagesSize()  { return 4;    }
quint16 kDisplayOptionsSize() { return 4;    }
quint16 kBlinkOptionsSize()   { return 4;    }
quint16 kBrightOptionsSize()  { return 3;    }
quint16 kImagesDataSize()     { return 6146; }

template <typename T>
const QString typeToHex(T type)
{
    return QString("0x%1")
           .arg(QString::number(static_cast<quint8>(type), 16).toUpper());
}

template <typename Based, typename Derived>
bool deserializeMessage(std::unique_ptr<Based>& dst, const QByteArray& src)
{
    if (src.size() < Derived::size())
    {
        return false;
    }

    Derived* derived = new Derived();
    dst.reset(derived);

    QDataStream in(src);
    in >> *derived;

    return true;
}

}

namespace protocol
{
AbstractMessage::AbstractMessage(MessageDirection direction)
    : m_direction(direction)
{

}

AbstractMessage::~AbstractMessage()
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

MessageDirection AbstractMessage::direction() const
{
    return m_direction;
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

Message::Message(MessageType type) :
    AbstractMessage(MessageDirection::Incoming),
    m_type(type)
{

}

Message::~Message()
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
            in >> type;
        }

        switch (static_cast<MessageType>(type))
        {
        case MessageType::DeviceIdentity:
            if (!::deserializeMessage<Message, DeviceIdentityMessage>(result, content))
            {
                // TODO
            }
            break;
        case MessageType::ButtonsState:
            if (!::deserializeMessage<Message, ButtonsStateMessage>(result, content))
            {
                // TODO
            }
            break;
        case MessageType::Unknown:
        default:
            Q_ASSERT_X(false, "incoming::Message::deserialize", "Unknown message type");
            break;
        }
    }

    return result;
}

MessageType Message::type() const
{
    return m_type;
}

DeviceIdentityMessage::DeviceIdentityMessage() :
    Message(MessageType::DeviceIdentity)
{

}

DeviceIdentityMessage::~DeviceIdentityMessage()
{

}

const QByteArray DeviceIdentityMessage::serialize() const
{
    // TODO
    return QByteArray(size(), '\0');
}

quint16 DeviceIdentityMessage::size()
{
    return ::kDeviceIdentitySize();
}

ButtonsStateMessage::ButtonsStateMessage() :
    Message(MessageType::ButtonsState)
{

}

ButtonsStateMessage::~ButtonsStateMessage()
{

}

const QByteArray ButtonsStateMessage::serialize() const
{
    // TODO
    return QByteArray(size(), '\0');
}

quint16 ButtonsStateMessage::size()
{
    return ::kButtonsStateSize();
}

QDataStream& operator <<(QDataStream& dst, const DeviceIdentityMessage& src)
{
    dst << DeviceIdentityMessage::size()
        << src.serialize();
    return dst;
}

QDataStream& operator <<(QDataStream& dst, const ButtonsStateMessage& src)
{
    dst << ButtonsStateMessage::size()
        << src.serialize();
    return dst;
}

QDataStream& operator >>(QDataStream& src, DeviceIdentityMessage& dst)
{
    // TODO
    Q_UNUSED(dst);

    return src;
}

QDataStream& operator >>(QDataStream& src, ButtonsStateMessage& dst)
{
    // TODO
    Q_UNUSED(dst);

    return src;
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

Message::Message(MessageType type) :
    AbstractMessage(MessageDirection::Outcoming),
    m_type(type)
{

}

Message::~Message()
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
            in >> type;
        }

        switch (static_cast<MessageType>(type))
        {
        case MessageType::DeviceAddress:
            if (!::deserializeMessage<Message, DeviceAddressMessage>(result, content))
            {
                // TODO
            }
            break;
        case MessageType::DisplayImages:
            if (!::deserializeMessage<Message, DisplayImagesMessage>(result, content))
            {
                // TODO
            }
            break;
        case MessageType::DisplayOptions:
            if (!::deserializeMessage<Message, DisplayOptionsMessage>(result, content))
            {
                // TODO
            }
            break;
        case MessageType::BlinkOptions:
            if (!::deserializeMessage<Message, BlinkOptionsMessage>(result, content))
            {
                // TODO
            }
            break;
        case MessageType::BrightOptions:
            if (!::deserializeMessage<Message, BrightOptionsMessage>(result, content))
            {
                // TODO
            }
            break;
        case MessageType::ImagesData:
            if (!::deserializeMessage<Message, ImagesDataMessage>(result, content))
            {
                // TODO
            }
            break;
        case MessageType::Unknown:
        default:
            Q_ASSERT_X(false, "incoming::Message::deserialize", "Unknown message type");
            break;
        }
    }

    return result;
}

MessageType Message::type() const
{
    return m_type;
}

DeviceAddressMessage::DeviceAddressMessage() :
    Message(MessageType::DeviceAddress)
{

}

DeviceAddressMessage::~DeviceAddressMessage()
{

}

const QByteArray DeviceAddressMessage::serialize() const
{
    // TODO
    return QByteArray(size(), '\0');
}

quint16 DeviceAddressMessage::size()
{
    return ::kDeviceAddressSize();
}

DisplayImagesMessage::DisplayImagesMessage() :
    Message(MessageType::DisplayImages)
{

}

DisplayImagesMessage::~DisplayImagesMessage()
{

}

const QByteArray DisplayImagesMessage::serialize() const
{
    // TODO
    return QByteArray(size(), '\0');
}

quint16 DisplayImagesMessage::size()
{
    return ::kDisplayImagesSize();
}

DisplayOptionsMessage::DisplayOptionsMessage() :
    Message(MessageType::DisplayOptions)
{

}

DisplayOptionsMessage::~DisplayOptionsMessage()
{

}

const QByteArray DisplayOptionsMessage::serialize() const
{
    // TODO
    return QByteArray(size(), '\0');
}

quint16 DisplayOptionsMessage::size()
{
    return ::kDisplayOptionsSize();
}

BlinkOptionsMessage::BlinkOptionsMessage() :
    Message(MessageType::BlinkOptions)
{

}

BlinkOptionsMessage::~BlinkOptionsMessage()
{

}

const QByteArray BlinkOptionsMessage::serialize() const
{
    // TODO
    return QByteArray(size(), '\0');
}

quint16 BlinkOptionsMessage::size()
{
    return ::kBlinkOptionsSize();
}

BrightOptionsMessage::BrightOptionsMessage() :
    Message(MessageType::BrightOptions)
{

}

BrightOptionsMessage::~BrightOptionsMessage()
{

}

const QByteArray BrightOptionsMessage::serialize() const
{
    // TODO
    return QByteArray(size(), '\0');
}

quint16 BrightOptionsMessage::size()
{
    return ::kBrightOptionsSize();
}

ImagesDataMessage::ImagesDataMessage() :
    Message(MessageType::ImagesData)
{

}

ImagesDataMessage::~ImagesDataMessage()
{

}

const QByteArray ImagesDataMessage::serialize() const
{
    // TODO
    return QByteArray(size(), '\0');
}

quint16 ImagesDataMessage::size()
{
    return ::kImagesDataSize();
}

QDataStream& operator <<(QDataStream& dst, const DeviceAddressMessage& src)
{
    dst << DeviceAddressMessage::size()
        << src.serialize();
    return dst;
}

QDataStream& operator <<(QDataStream& dst, const DisplayImagesMessage& src)
{
    dst << DisplayImagesMessage::size()
        << src.serialize();
    return dst;
}

QDataStream& operator <<(QDataStream& dst, const DisplayOptionsMessage& src)
{
    dst << DisplayOptionsMessage::size()
        << src.serialize();
    return dst;
}

QDataStream& operator <<(QDataStream& dst, const BlinkOptionsMessage& src)
{
    dst << BlinkOptionsMessage::size()
        << src.serialize();
    return dst;
}

QDataStream& operator <<(QDataStream& dst, const BrightOptionsMessage& src)
{
    dst << BrightOptionsMessage::size()
        << src.serialize();
    return dst;
}

QDataStream& operator <<(QDataStream& dst, const ImagesDataMessage& src)
{
    dst << ImagesDataMessage::size()
        << src.serialize();
    return dst;
}

QDataStream& operator >>(QDataStream& src, DeviceAddressMessage& dst)
{
    // TODO
    Q_UNUSED(dst);

    return src;
}

QDataStream& operator >>(QDataStream& src, DisplayImagesMessage& dst)
{
    // TODO
    Q_UNUSED(dst);

    return src;
}

QDataStream& operator >>(QDataStream& src, DisplayOptionsMessage& dst)
{
    // TODO
    Q_UNUSED(dst);

    return src;
}

QDataStream& operator >>(QDataStream& src, BlinkOptionsMessage& dst)
{
    // TODO
    Q_UNUSED(dst);

    return src;
}

QDataStream& operator >>(QDataStream& src, BrightOptionsMessage& dst)
{
    // TODO
    Q_UNUSED(dst);

    return src;
}

QDataStream& operator >>(QDataStream& src, ImagesDataMessage& dst)
{
    // TODO
    Q_UNUSED(dst);

    return src;
}

} // outcoming
} // protocol
