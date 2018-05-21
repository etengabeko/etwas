#ifndef PROTOCOL_PROTOCOL_H
#define PROTOCOL_PROTOCOL_H

#include <QtGlobal>

class QString;

namespace protocol
{
class AbstractMessage
{
public:
    AbstractMessage();
    virtual ~AbstractMessage() = 0;

};

namespace incoming
{
enum class MessageType
{
    DeviceIdentity = 0x01, //!< Идентификация панели
    ButtonsState   = 0x02  //!< Изменение состояния клавиш
};

const QString typeToString(MessageType type);

class Message : public AbstractMessage
{
public:
    explicit Message(MessageType type);
    ~Message() override;

    MessageType type() const;

protected:
    const MessageType m_type;

};

class DeviceIdentityMessage : public Message
{
public:
    DeviceIdentityMessage();
    ~DeviceIdentityMessage() override;
};

class ButtonsStateMessage : public Message
{
public:
    ButtonsStateMessage();
    ~ButtonsStateMessage() override;
};

} // incoming

namespace outcoming
{
enum class MessageType
{
    DeviceAddress  = 0x01, //!< Установка адреса и порта панели
    DisplayImages  = 0x02, //!< Назначение изображений дисплеям
    DisplayOptions = 0x03, //!< Управление избражениями дисплея
    BlinkOptions   = 0x04, //!< Управление параметрами мигания дисплея
    BrightOptions  = 0x05, //!< Управление яркостью дисплея
    ImagesData     = 0x06  //!< Загрузка изображений в память панели
};

const QString typeToString(MessageType type);

class Message : public AbstractMessage
{
public:
    explicit Message(MessageType type);
    ~Message() override;

    MessageType type() const;

protected:
    const MessageType m_type;

};

class DisplayImagesMessage : public Message
{
public:
    DisplayImagesMessage();
    ~DisplayImagesMessage() override;
};

class DisplayOptionsMessage : public Message
{
public:
    DisplayOptionsMessage();
    ~DisplayOptionsMessage() override;
};

class BlinkOptionsMessage : public Message
{
public:
    BlinkOptionsMessage();
    ~BlinkOptionsMessage() override;
};

class BrightOptionsMessage : public Message
{
public:
    BrightOptionsMessage();
    ~BrightOptionsMessage() override;
};

class ImagesDataMessage : public Message
{
public:
    ImagesDataMessage();
    ~ImagesDataMessage() override;
};

} // outcoming
} // protocol

#endif // PROTOCOL_PROTOCOL_H
