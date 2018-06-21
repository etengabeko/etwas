#ifndef PROTOCOL_PROTOCOL_H
#define PROTOCOL_PROTOCOL_H

#include <memory>

#include <QtGlobal>
#include <QRgb>

class QByteArray;
class QDataStream;
class QString;
template <typename T> class QVector;

/**
 * @namespace protocol
 * @brief Реализация протокола взаимодействия
 */
namespace protocol
{
/**
 * @enum ButtonState
 * @brief Значения состояний клавиш
 */
enum class ButtonState;

/**
 * @enum ImageSelection
 * @brief Выбранные изображения
 */
enum class ImageSelection;

/**
 * @enum BlinkState
 * @brief Параметры мигания
 */
enum class BlinkState;

/**
 * @namespace internal
 * @brief private implementation only
 */
namespace details
{
class DeviceIdentityMessagePrivate;
class ButtonsStateMessagePrivate;
class DeviceAddressMessagePrivate;
class DisplayImagesMessagePrivate;
class DisplayOptionsMessagePrivate;
class BlinkOptionsMessagePrivate;
class BrightOptionsMessagePrivate;
class ImageDataMessagePrivate;

} // details

/**
 * @enum MessageDirection
 * @brief Типы сообщений по направлению передачи
 */
enum class MessageDirection
{
    Unknown   = 0x0000, //!< Тип не определён
    Incoming  = 0x0100, //!< Входящее сообщение (от устройства)
    Outcoming = 0x1000  //!< Исходящее сообщение (на устройство)
};

/**
 * @class AbstractMessage
 * @brief Интерфейс классов сообщений
 */
class AbstractMessage
{
public:
    explicit AbstractMessage(MessageDirection direction) NOEXCEPT;
    virtual ~AbstractMessage() NOEXCEPT;

    /**
     * @brief deserialize - Выделяет объект-сообщение конкретного типа из массива байт
     * @param direction - Тип сообщения по направлению передачи
     * @param content - Сериализованное сообщение (массив байт)
     * @return Заполненный из массива байт объект-сообщение
     *         В случае ошибки десериализации - nullptr
     */
    static std::unique_ptr<AbstractMessage> deserialize(MessageDirection direction,
                                                        const QByteArray& content);

    /**
     * @brief direction - Возвращает тип сообщения по направлению передачи
     * @return Тип сообщения (входящее/исходящее)
     */
    MessageDirection direction() const NOEXCEPT;

    /**
     * @brief serialize - Записывает сообщение в массив байт
     * @return Сериализованное сообщение в виде массива байт
     */
    virtual const QByteArray serialize() const = 0;

    /**
     * @brief parse - Заполняет собощение из массива байт
     * @param src - Сериализованное сообщение в виде массива байт
     * @return true в случае успешности выполнения
     */
    virtual bool parse(const QByteArray& src) = 0;

protected:
    MessageDirection m_direction = MessageDirection::Unknown; //!< Тип сообщения по направлению передачи (входящее/исходящее)

};

//!< Запись сообщений в поток
QDataStream& operator <<(QDataStream& dst, const AbstractMessage& src);

//!< Чтение сообщений из потока
QDataStream& operator >>(QDataStream& src, AbstractMessage& dst);

/**
 * @namespace incoming
 * @brief Определения входящих сообщений
 */
namespace incoming
{
/**
 * @enum MessageType
 * @brief Типы входящих сообщений
 */
enum class MessageType
{
    Unknown        = 0x00, //!< Тип не определён
    DeviceIdentity = 0x01, //!< Идентификация устройства
    ButtonsState   = 0x02  //!< Изменение состояния клавиш
};

/**
 * @brief typeToString - Возвращает текстовое представление типа сообщений
 * @param type - Тип сообщения
 * @return Строковое представление типа type
 */
const QString typeToString(MessageType type);

/**
 * @class Message
 * @brief Интерфейс классов входящих сообщений
 */
class Message : public AbstractMessage
{
public:
    explicit Message(MessageType type) NOEXCEPT;
    ~Message() NOEXCEPT override;

    Message(const Message& other) NOEXCEPT;

    /**
     * @brief deserialize - Выделяет объект-сообщение конкретного типа из массива байт
     * @param content - Сериализованное сообщение (массив байт)
     * @return Заполненный из массива байт объект-сообщение
     *         В случае ошибки десериализации - nullptr
     */
    static std::unique_ptr<Message> deserialize(const QByteArray& content);

    /**
     * @brief type - Возвращает тип сообщения
     * @return Тип входящего сообщения
     */
    MessageType type() const NOEXCEPT;

protected:
    const MessageType m_type = MessageType::Unknown; //!< Тип входящего сообщения

};

/**
 * @class DeviceIdentityMessage
 * @brief Реализация сообщений Идентификации устройства
 */
class DeviceIdentityMessage final : public Message
{
public:
    DeviceIdentityMessage();
    ~DeviceIdentityMessage() NOEXCEPT override;

    DeviceIdentityMessage(DeviceIdentityMessage&& other) NOEXCEPT;
    DeviceIdentityMessage& operator =(DeviceIdentityMessage&& other) NOEXCEPT;

    DeviceIdentityMessage(const DeviceIdentityMessage& other);
    DeviceIdentityMessage& operator =(const DeviceIdentityMessage& other);

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

    /**
     * @brief firmwareVersion - Возвращает версию прошивки
     * @return Номер версии прошивки
     */
    quint8 firmwareVersion() const NOEXCEPT;

    /**
     * @brief setFirmwareVersion - Устанавливает версию прошивки
     * @param version - Номер версии прошивки
     */
    void setFirmwareVersion(quint8 version) NOEXCEPT;

    /**
     * @brief buttonsNumbers - Возвращает номера клавиш
     * @return Набор установленных номеров клавиш
     */
    const QVector<quint8> buttonsNumbers() const;

    /**
     * @brief setButtonsNumbers - Устанавливает новый набор номеров клавиш
     * @param numbers - Устанавливаемые номера клавиш
     */
    void setButtonsNumbers(const QVector<quint8>& numbers);
    void setButtonsNumbers(QVector<quint8>&& numbers);

private:
    std::unique_ptr<details::DeviceIdentityMessagePrivate> m_pimpl;

};

/**
 * @class ButtonsStateMessage
 * @brief Реализация сообщений Изменения состояния клавиш
 */
class ButtonsStateMessage final : public Message
{
public:
    ButtonsStateMessage();
    ~ButtonsStateMessage() NOEXCEPT override;

    ButtonsStateMessage(ButtonsStateMessage&& other) NOEXCEPT;
    ButtonsStateMessage& operator =(ButtonsStateMessage&& other) NOEXCEPT;

    ButtonsStateMessage(const ButtonsStateMessage& other);
    ButtonsStateMessage& operator =(const ButtonsStateMessage& other);

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

    /**
     * @brief buttonsStates - Возвращает состояния клавиш
     * @return Набор состояний клавиш
     */
    const QVector<ButtonState> buttonsStates() const;

    /**
     * @brief setButtonsStates - Устанавливает новые состояния клавиш
     * @param states - Набор новых состояний клавиш
     */
    void setButtonsStates(const QVector<ButtonState>& states);
    void setButtonsStates(QVector<ButtonState>&& states) NOEXCEPT;

private:
    std::unique_ptr<details::ButtonsStateMessagePrivate> m_pimpl;

};

} // incoming

/**
 * @namespace outcoming
 * @brief Определения исходящих сообщений
 */
namespace outcoming
{
/**
 * @enum MessageType
 * @brief Типы исходящих сообщений
 */
enum class MessageType
{
    Unknown        = 0x00, //!< Тип не определён
    DeviceAddress  = 0x01, //!< Установка адреса и порта устройства
    DisplayImages  = 0x02, //!< Назначение изображений дисплеям
    DisplayOptions = 0x03, //!< Управление избражениями дисплея
    BlinkOptions   = 0x04, //!< Управление параметрами мигания дисплея
    BrightOptions  = 0x05, //!< Управление яркостью дисплея
    ImagesData     = 0x06  //!< Загрузка изображений в память устройства
};

/**
 * @brief typeToString - Возвращает текстовое представление типа сообщений
 * @param type - Тип сообщения
 * @return Строковое представление типа type
 */
const QString typeToString(MessageType type);

/**
 * @class Message
 * @brief Интерфейс классов исходящих сообщений
 */
class Message : public AbstractMessage
{
public:
    explicit Message(MessageType type) NOEXCEPT;
    ~Message() NOEXCEPT override;

    Message(const Message& other) NOEXCEPT;

    /**
     * @brief deserialize - Выделяет объект-сообщение конкретного типа из массива байт
     * @param content - Сериализованное сообщение (массив байт)
     * @return Заполненный из массива байт объект-сообщение
     *         В случае ошибки десериализации - nullptr
     */
    static std::unique_ptr<Message> deserialize(const QByteArray& content);

    /**
     * @brief type - Возвращает тип сообщения
     * @return Тип входящего сообщения
     */
    MessageType type() const NOEXCEPT;

protected:
    const MessageType m_type = MessageType::Unknown; //!< Тип исходящего сообщения

};

/**
 * @class DeviceAddressMessage
 * @brief Реализация сообщений Установка адреса и порта
 */
class DeviceAddressMessage final : public Message
{
public:
    DeviceAddressMessage();
    ~DeviceAddressMessage() NOEXCEPT override;

    DeviceAddressMessage(const DeviceAddressMessage& other);
    DeviceAddressMessage& operator =(const DeviceAddressMessage& other);

    DeviceAddressMessage(DeviceAddressMessage&& other) NOEXCEPT;
    DeviceAddressMessage& operator =(DeviceAddressMessage&& other) NOEXCEPT;

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

    /**
     * @brief address - Возвращает ip-адрес устройства
     * @return ip-адрес устройства в строковом представлении 'xxx.xxx.xxx.xxx'
     */
    const QString address() const;

    /**
     * @brief setAddress - Устанавливает ip-адрес устройства
     * @param addr - Новый ip-адрес устройства в строковом представлении 'xxx.xxx.xxx.xxx'
     */
    void setAddress(const QString& addr);

    /**
     * @brief port - Возвращает номер порта, используемый устройством
     * @return Номер порта
     */
    quint16 port() const NOEXCEPT;

    /**
     * @brief setPort - Устанавливает используемый устройством номер порта
     * @param num - Новое значение номера порта
     */
    void setPort(quint16 num) NOEXCEPT;

private:
    std::unique_ptr<details::DeviceAddressMessagePrivate> m_pimpl;

};

/**
 * @class DisplayImagesMessage
 * @brief Реализация сообщений Назначение изображений
 */
class DisplayImagesMessage final : public Message
{
public:
    DisplayImagesMessage();
    ~DisplayImagesMessage() NOEXCEPT override;

    DisplayImagesMessage(const DisplayImagesMessage& other);
    DisplayImagesMessage& operator =(const DisplayImagesMessage& other);

    DisplayImagesMessage(DisplayImagesMessage&& other) NOEXCEPT;
    DisplayImagesMessage& operator =(DisplayImagesMessage&& other) NOEXCEPT;

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

    /**
     * @brief displayNumber - Возвращает номер дисплея
     * @return Номер дисплея
     */
    quint8 displayNumber() const NOEXCEPT;

    /**
     * @brief setDisplayNumber - Устанавливает номер дисплея
     * @param num - Новый номер дисплея
     */
    void setDisplayNumber(quint8 num) NOEXCEPT;

    /**
     * @brief firstImageNumber - Возвращает номер изображения для первого дисплея
     * @return Номер изображения для первого дисплея
     */
    quint8 firstImageNumber() const NOEXCEPT;

    /**
     * @brief setFirstImageNumber - Устанавливает номер изображения для первого дисплея
     * @param num - Новый номер изображения
     */
    void setFirstImageNumber(quint8 num) NOEXCEPT;

    /**
     * @brief secondImageNumber- Возвращает номер изображения для второго дисплея
     * @return Номер изображения для второго дисплея
     */
    quint8 secondImageNumber() const NOEXCEPT;

    /**
     * @brief setSecondImageNumber - Устанавливает номер изображения для второго дисплея
     * @param num -Новый номер изображения
     */
    void setSecondImageNumber(quint8 num) NOEXCEPT;

private:
    std::unique_ptr<details::DisplayImagesMessagePrivate> m_pimpl;

};

/**
 * @class DisplayOptionsMessage
 * @brief Реализация сообщений Управление избражениями
 */
class DisplayOptionsMessage final : public Message
{
public:
    DisplayOptionsMessage();
    ~DisplayOptionsMessage() NOEXCEPT override;

    DisplayOptionsMessage(const DisplayOptionsMessage& other);
    DisplayOptionsMessage& operator =(const DisplayOptionsMessage& other);

    DisplayOptionsMessage(DisplayOptionsMessage&& other) NOEXCEPT;
    DisplayOptionsMessage& operator =(DisplayOptionsMessage&& other) NOEXCEPT;

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

    /**
     * @brief displayNumber - Возвращает номер дисплея
     * @return Номер дисплея
     */
    quint8 displayNumber() const NOEXCEPT;

    /**
     * @brief setDisplayNumber - Устанавливает номер дисплея
     * @param num - Новый номер дисплея
     */
    void setDisplayNumber(quint8 num) NOEXCEPT;

    /**
     * @brief imageSelection - Возвращает параметры назначенных для дисплея изображений
     * @return Параметры изображений дисплея
     */
    ImageSelection imageSelection() const NOEXCEPT;

    /**
     * @brief setImageSelection - Устанавливает параметры назначенных для дисплея изображений
     * @param selection - Новые параметры изображений дисплея
     */
    void setImageSelection(ImageSelection selection) NOEXCEPT;

    /**
     * @brief blinkState - Возвращает состояние мигания изображений дисплея
     * @return Состояние мигания изображений
     */
    BlinkState blinkState() const NOEXCEPT;

    /**
     * @brief setBlinkState - Устанавливает состояние мигания изображений дисплея
     * @param blink - Новое состояние мигания изображений дисплея
     */
    void setBlinkState(BlinkState blink) NOEXCEPT;

private:
    std::unique_ptr<details::DisplayOptionsMessagePrivate> m_pimpl;

};

/**
 * @class BlinkOptionsMessage
 * @brief Реализация сообщений Управление параметрами мигания
 */
class BlinkOptionsMessage final : public Message
{
public:
    BlinkOptionsMessage();
    ~BlinkOptionsMessage() NOEXCEPT override;

    BlinkOptionsMessage(const BlinkOptionsMessage& other);
    BlinkOptionsMessage& operator =(const BlinkOptionsMessage& other);

    BlinkOptionsMessage(BlinkOptionsMessage&& other) NOEXCEPT;
    BlinkOptionsMessage& operator =(BlinkOptionsMessage&& other) NOEXCEPT;

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

    /**
     * @brief displayNumber - Возвращает номер дисплея
     * @return Номер дисплея
     */
    quint8 displayNumber() const NOEXCEPT;

    /**
     * @brief setDisplayNumber - Устанавливает номер дисплея
     * @param num - Новый номер дисплея
     */
    void setDisplayNumber(quint8 num) NOEXCEPT;

    /**
     * @brief timeOn - Возвращает время включения первого изображения при мигании
     * @return Время включения в мсек*10
     */
    quint8 timeOn() const NOEXCEPT;

    /**
     * @brief setTimeOn - Устанавливает время включения первого изображения при мигании
     * @param msec10 - Время включения в мсек*10
     */
    void setTimeOn(quint8 msec10) NOEXCEPT;

    /**
     * @brief timeOff - Возвращает время выключения первого изображения (или время включения второго изображения) при мигании
     * @return Время выключения в мсек*10
     */
    quint8 timeOff() const NOEXCEPT;

    /**
     * @brief setTimeOff - Устанавливает время выключения первого изображения (или время включения второго изображения) при мигании
     * @param msec10 - Время выключения (или время включения второго изображения) в мсек*10
     */
    void setTimeOff(quint8 msec10) NOEXCEPT;

private:
    std::unique_ptr<details::BlinkOptionsMessagePrivate> m_pimpl;

};

/**
 * @class BrightOptionsMessage
 * @brief Реализация сообщений Управление яркостью
 */
class BrightOptionsMessage final : public Message
{
public:
    BrightOptionsMessage();
    ~BrightOptionsMessage() NOEXCEPT override;

    BrightOptionsMessage(const BrightOptionsMessage& other);
    BrightOptionsMessage& operator =(const BrightOptionsMessage& other);

    BrightOptionsMessage(BrightOptionsMessage&& other) NOEXCEPT;
    BrightOptionsMessage& operator =(BrightOptionsMessage&& other) NOEXCEPT;

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

    /**
     * @brief displayNumber - Возвращает номер дисплея
     * @return Номер дисплея
     */
    quint8 displayNumber() const NOEXCEPT;

    /**
     * @brief setDisplayNumber - Устанавливает номер дисплея
     * @param num - Новый номер дисплея
     */
    void setDisplayNumber(quint8 num) NOEXCEPT;

    /**
     * @brief brightLevel - Возвращает уровень яркости
     * @return Уровень яркости в диапазоне от 0 до 15
     */
    quint8 brightLevel() const NOEXCEPT;

    /**
     * @brief setBrightLevel - Устанавливает новый уровень яркости
     * @param bright - Новое значение уровня яркости в диапазоне от 0 до 15
     */
    void setBrightLevel(quint8 bright) NOEXCEPT;

private:
    std::unique_ptr<details::BrightOptionsMessagePrivate> m_pimpl;

};

/**
 * @class ImagesDataMessage
 * @brief Реализация сообщений Загрузка изображений
 */
class ImageDataMessage final : public Message
{
public:
    ImageDataMessage();
    ~ImageDataMessage() NOEXCEPT override;

    ImageDataMessage(const ImageDataMessage& other);
    ImageDataMessage& operator =(const ImageDataMessage& other);

    ImageDataMessage(ImageDataMessage&& other) NOEXCEPT;
    ImageDataMessage& operator =(ImageDataMessage&& other) NOEXCEPT;

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

    /**
     * @brief imageNumber - Возвращает номер изображения
     * @return Номер изображения
     */
    quint8 imageNumber() const NOEXCEPT;

    /**
     * @brief setImageNumber - Устанавливает номер изображения
     * @param num - Новый номер изображения
     */
    void setImageNumber(quint8 num) NOEXCEPT;

    /**
     * @brief imageColors - Возвращает изображение в виде набора точек с соответствующими цветами
     * @return Изображение в виде набора точек, заданных их цветами
     */
    const QVector<QRgb> imageColors() const;

    /**
     * @brief setImageColors - Устанавливает для изображения новый набор точек
     * @param colors - Новый набор точек для изображения
     */
    void setImageColors(const QVector<QRgb>& colors);
    void setImageColors(QVector<QRgb>&& colors) NOEXCEPT;

private:
    std::unique_ptr<details::ImageDataMessagePrivate> m_pimpl;

};

} // outcoming
} // protocol

#endif // PROTOCOL_PROTOCOL_H
