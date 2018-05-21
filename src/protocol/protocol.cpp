#include "protocol.h"

#include <QCoreApplication>
#include <QDataStream>
#include <QMap>
#include <QString>

namespace
{

template <typename T>
const QString typeToHex(T type)
{
    return QString("0x%1")
           .arg(QString::number(static_cast<quint8>(type), 16).toUpper());
}

}

namespace protocol
{
AbstractMessage::AbstractMessage()
{

}

AbstractMessage::~AbstractMessage()
{

}

namespace incoming
{
const QString typeToString(MessageType type)
{
    static const QMap<MessageType, QString> types({
                                                      { MessageType::DeviceIdentity, qApp->tr("DeviceIdentity = %1")
                                                                                     .arg(::typeToHex(MessageType::DeviceIdentity)) },
                                                      { MessageType::ButtonsState,   qApp->tr("ButtonsState = %1")
                                                                                     .arg(::typeToHex(MessageType::ButtonsState)) }
                                                  });
    auto founded = types.find(type);
    return (founded != types.cend() ? founded.value()
                                    : qApp->tr("Unknown incoming type = %1").arg(::typeToHex(type)));
}

Message::Message(MessageType type) :
    m_type(type)
{

}

Message::~Message()
{

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

ButtonsStateMessage::ButtonsStateMessage() :
    Message(MessageType::ButtonsState)
{

}

ButtonsStateMessage::~ButtonsStateMessage()
{

}

} // incoming

namespace outcoming
{
const QString typeToString(MessageType type)
{
    static const QMap<MessageType, QString> types({
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
                                                  });
    auto founded = types.find(type);
    return (founded != types.cend() ? founded.value()
                                    : qApp->tr("Unknown outcoming type = %1").arg(::typeToHex(type)));
}

Message::Message(MessageType type) :
    m_type(type)
{

}

Message::~Message()
{

}

MessageType Message::type() const
{
    return m_type;
}

DisplayImagesMessage::DisplayImagesMessage() :
    Message(MessageType::DisplayImages)
{

}

DisplayImagesMessage::~DisplayImagesMessage()
{

}

DisplayOptionsMessage::DisplayOptionsMessage() :
    Message(MessageType::DisplayOptions)
{

}

DisplayOptionsMessage::~DisplayOptionsMessage()
{

}

BlinkOptionsMessage::BlinkOptionsMessage() :
    Message(MessageType::BlinkOptions)
{

}

BlinkOptionsMessage::~BlinkOptionsMessage()
{

}

BrightOptionsMessage::BrightOptionsMessage() :
    Message(MessageType::BrightOptions)
{

}

BrightOptionsMessage::~BrightOptionsMessage()
{

}

ImagesDataMessage::ImagesDataMessage() :
    Message(MessageType::ImagesData)
{

}

ImagesDataMessage::~ImagesDataMessage()
{

}

} // outcoming
} // protocol
