#ifndef PROTOCOL_PROTOCOL_H
#define PROTOCOL_PROTOCOL_H

#include <memory>

#include <QtGlobal>

class QByteArray;
class QDataStream;
class QString;

namespace protocol
{

enum class MessageDirection
{
    Unknown   = 0x0000, //!< Тип не определён
    Incoming  = 0x0100, //!< Входящее сообщение (от устройства)
    Outcoming = 0x1000  //!< Исходящее сообщение (на устройство)
};

class AbstractMessage
{
public:
    explicit AbstractMessage(MessageDirection direction);
    virtual ~AbstractMessage();

    static std::unique_ptr<AbstractMessage> deserialize(MessageDirection direction,
                                                        const QByteArray& content);

    MessageDirection direction() const;

    virtual const QByteArray serialize() const = 0;

protected:
    MessageDirection m_direction = MessageDirection::Unknown;

};

namespace incoming
{
enum class MessageType
{
    Unknown        = 0x00, //!< Тип не определён
    DeviceIdentity = 0x01, //!< Идентификация панели
    ButtonsState   = 0x02  //!< Изменение состояния клавиш
};

const QString typeToString(MessageType type);

class Message : public AbstractMessage
{
public:
    explicit Message(MessageType type);
    ~Message() override;

    static std::unique_ptr<Message> deserialize(const QByteArray& content);

    MessageType type() const;

protected:
    const MessageType m_type = MessageType::Unknown;

};

class DeviceIdentityMessage : public Message
{
public:
    DeviceIdentityMessage();
    ~DeviceIdentityMessage() override;

    const QByteArray serialize() const override;

    static quint16 size();

};

class ButtonsStateMessage : public Message
{
public:
    ButtonsStateMessage();
    ~ButtonsStateMessage() override;

    const QByteArray serialize() const override;

    static quint16 size();

};

QDataStream& operator <<(QDataStream& dst, const DeviceIdentityMessage& src);
QDataStream& operator <<(QDataStream& dst, const ButtonsStateMessage& src);

QDataStream& operator >>(QDataStream& src, DeviceIdentityMessage& dst);
QDataStream& operator >>(QDataStream& src, ButtonsStateMessage& dst);

} // incoming

namespace outcoming
{
enum class MessageType
{
    Unknown        = 0x00, //!< Тип не определён
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

    static std::unique_ptr<Message> deserialize(const QByteArray& content);

    MessageType type() const;

protected:
    const MessageType m_type = MessageType::Unknown;

};

class DeviceAddressMessage : public Message
{
public:
    DeviceAddressMessage();
    ~DeviceAddressMessage() override;

    const QByteArray serialize() const override;

    static quint16 size();

};

class DisplayImagesMessage : public Message
{
public:
    DisplayImagesMessage();
    ~DisplayImagesMessage() override;

    const QByteArray serialize() const override;

    static quint16 size();

};

class DisplayOptionsMessage : public Message
{
public:
    DisplayOptionsMessage();
    ~DisplayOptionsMessage() override;

    const QByteArray serialize() const override;

    static quint16 size();

};

class BlinkOptionsMessage : public Message
{
public:
    BlinkOptionsMessage();
    ~BlinkOptionsMessage() override;

    const QByteArray serialize() const override;

    static quint16 size();

};

class BrightOptionsMessage : public Message
{
public:
    BrightOptionsMessage();
    ~BrightOptionsMessage() override;

    const QByteArray serialize() const override;

    static quint16 size();

};

class ImagesDataMessage : public Message
{
public:
    ImagesDataMessage();
    ~ImagesDataMessage() override;

    const QByteArray serialize() const override;

    static quint16 size();

};

QDataStream& operator <<(QDataStream& dst, const DeviceAddressMessage& src);
QDataStream& operator <<(QDataStream& dst, const DisplayImagesMessage& src);
QDataStream& operator <<(QDataStream& dst, const DisplayOptionsMessage& src);
QDataStream& operator <<(QDataStream& dst, const BlinkOptionsMessage& src);
QDataStream& operator <<(QDataStream& dst, const BrightOptionsMessage& src);
QDataStream& operator <<(QDataStream& dst, const ImagesDataMessage& src);

QDataStream& operator >>(QDataStream& src, DeviceAddressMessage& dst);
QDataStream& operator >>(QDataStream& src, DisplayImagesMessage& dst);
QDataStream& operator >>(QDataStream& src, DisplayOptionsMessage& dst);
QDataStream& operator >>(QDataStream& src, BlinkOptionsMessage& dst);
QDataStream& operator >>(QDataStream& src, BrightOptionsMessage& dst);
QDataStream& operator >>(QDataStream& src, ImagesDataMessage& dst);

} // outcoming
} // protocol

#endif // PROTOCOL_PROTOCOL_H
