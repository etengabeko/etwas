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
class PingMessagePrivate;
class DeviceIdentityMessagePrivate;
class ButtonsStateMessagePrivate;
class BeginLogMessagePrivate;
class NextLogMessagePrivate;
class EndLogMessagePrivate;
class DeviceAddressMessagePrivate;
class DisplayImagesMessagePrivate;
class DisplayOptionsMessagePrivate;
class BlinkOptionsMessagePrivate;
class BrightOptionsMessagePrivate;
class ImageDataMessagePrivate;
class SendLogMessagePrivate;
class ClearLogMessagePrivate;
class CurrentTimeMessagePrivate;

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
    Ping           = 0x00, //!< Сообщение о наличии соединения
    DeviceIdentity = 0x01, //!< Идентификация устройства
    ButtonsState   = 0x02, //!< Изменение состояния клавиш
    BeginLog       = 0x03, //!< Начало передачи лог-файла
    NextLog        = 0x04, //!< Сообщение лог-файла
    EndLog         = 0x05, //!< Конец передычи лог-файла
    Unknown        = 0xFF  //!< Тип не определён
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
 * @class PingMessage
 * @brief Реализация сообщений о наличии соединения
 */
class PingMessage final : public Message
{
public:
    PingMessage();
    ~PingMessage() NOEXCEPT override;

    PingMessage(PingMessage&& other) NOEXCEPT;
    PingMessage& operator =(PingMessage&& other) NOEXCEPT;

    PingMessage(const PingMessage& other);
    PingMessage& operator =(const PingMessage& other);

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

private:
    std::unique_ptr<details::PingMessagePrivate> m_pimpl;

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

    /**
     * @brief maxBytesCount - Возвращает установленное ранее количество байт для сериализации состояний клавиш
     * @note Зависит от количества клавиш панели: от 1 до 32 байт (Каждый байт отвечает за состояние 8 клавиш - по 1 биту на клавишу)
     * @return Размер данных сериализованного сообщения в байтах
     */
    static quint8 maxButtonsStatesCount() NOEXCEPT;

    /**
     * @brief setMaxButtonsStatesCount - Устанавливает количество байт для сериализации состояний клавиш
     * @param count - Размер данных сериализованного сообщения в байтах
     */
    static void setMaxButtonsStatesCount(quint8 count) NOEXCEPT;

private:
    std::unique_ptr<details::ButtonsStateMessagePrivate> m_pimpl;

};

/**
 * @class BeginLogMessage
 * @brief Реализация сообщений о начале передачи лог-файла
 */
class BeginLogMessage final : public Message
{
public:
    BeginLogMessage();
    ~BeginLogMessage() NOEXCEPT override;

    BeginLogMessage(BeginLogMessage&& other) NOEXCEPT;
    BeginLogMessage& operator =(BeginLogMessage&& other) NOEXCEPT;

    BeginLogMessage(const BeginLogMessage& other);
    BeginLogMessage& operator =(const BeginLogMessage& other);

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

    /**
     * @brief count - Возвращает количество сообщений лог-файла
     * @return Количество сообщений лог-файла
     */
    quint32 count() const NOEXCEPT;

    /**
     * @brief setCount - Устанавливает значение количества сообщений лог-файла
     * @param num - Новое значение количества сообщений лог-файла
     */
    void setCount(quint32 num) NOEXCEPT;

private:
    std::unique_ptr<details::BeginLogMessagePrivate> m_pimpl;

};

/**
 * @class NextLogMessage
 * @brief Реализация передачи сообщения лог-файла
 */
class NextLogMessage final : public Message
{
public:
    NextLogMessage();
    ~NextLogMessage() NOEXCEPT override;

    NextLogMessage(NextLogMessage&& other) NOEXCEPT;
    NextLogMessage& operator =(NextLogMessage&& other) NOEXCEPT;

    NextLogMessage(const NextLogMessage& other);
    NextLogMessage& operator =(const NextLogMessage& other);

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

    /**
     * @brief data - Возвращает данные сообщения лог-файла
     * @return Данные сообщения лог-файла
     */
    const QByteArray& data() const NOEXCEPT;

    /**
     * @brief setData - Устанавливает новые данные сообщения лог-файла
     * @param content - Новое значение данных сообщения лог-файла
     */
    void setData(const QByteArray& content);

private:
    std::unique_ptr<details::NextLogMessagePrivate> m_pimpl;

};

/**
 * @class EndLogMessage
 * @brief Реализация сообщений об окончании передачи лог-файла
 */
class EndLogMessage final : public Message
{
public:
    EndLogMessage();
    ~EndLogMessage() NOEXCEPT override;

    EndLogMessage(EndLogMessage&& other) NOEXCEPT;
    EndLogMessage& operator =(EndLogMessage&& other) NOEXCEPT;

    EndLogMessage(const EndLogMessage& other);
    EndLogMessage& operator =(const EndLogMessage& other);

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

private:
    std::unique_ptr<details::EndLogMessagePrivate> m_pimpl;

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
    Ping           = 0x00, //!< Сообщение о наличии соединения
    DeviceAddress  = 0x01, //!< Установка адреса и порта устройства
    DisplayImages  = 0x02, //!< Назначение изображений дисплеям
    DisplayOptions = 0x03, //!< Управление избражениями дисплея
    BlinkOptions   = 0x04, //!< Управление параметрами мигания дисплея
    BrightOptions  = 0x05, //!< Управление яркостью дисплея
    ImageData      = 0x06, //!< Загрузка изображений в память устройства
    SendLog        = 0x07, //!< Передача устройством лог-файла
    ClearLog       = 0x08, //!< Очистка устройством лог-файла
    CurrentTime    = 0x09, //!< Установка текущего времени
    Unknown        = 0xFF  //!< Тип не определён
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
 * @class PingMessage
 * @brief Реализация сообщений о наличии соединения
 */
class PingMessage final : public Message
{
public:
    PingMessage();
    ~PingMessage() NOEXCEPT override;

    PingMessage(PingMessage&& other) NOEXCEPT;
    PingMessage& operator =(PingMessage&& other) NOEXCEPT;

    PingMessage(const PingMessage& other);
    PingMessage& operator =(const PingMessage& other);

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

private:
    std::unique_ptr<details::PingMessagePrivate> m_pimpl;

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

    /**
     * @brief serializeWithoutData - Возвращает только заголовок сообщения (не включая данные)
     * @return Заголовок сообщения в виде массива байт
     */
    const QByteArray serializeWithoutData() const;

private:
    std::unique_ptr<details::ImageDataMessagePrivate> m_pimpl;

};

/**
 * @class SendLogMessage
 * @brief Реализация сообщений Чтение лог-файла
 */
class SendLogMessage final : public Message
{
public:
    SendLogMessage();
    ~SendLogMessage() NOEXCEPT override;

    SendLogMessage(const SendLogMessage& other);
    SendLogMessage& operator =(const SendLogMessage& other);

    SendLogMessage(SendLogMessage&& other) NOEXCEPT;
    SendLogMessage& operator =(SendLogMessage&& other) NOEXCEPT;

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

private:
    std::unique_ptr<details::SendLogMessagePrivate> m_pimpl;

};

/**
 * @class ClearLogMessage
 * @brief Реализация сообщений Очистка лог-файла
 */
class ClearLogMessage final : public Message
{
public:
    ClearLogMessage();
    ~ClearLogMessage() NOEXCEPT override;

    ClearLogMessage(const ClearLogMessage& other);
    ClearLogMessage& operator =(const ClearLogMessage& other);

    ClearLogMessage(ClearLogMessage&& other) NOEXCEPT;
    ClearLogMessage& operator =(ClearLogMessage&& other) NOEXCEPT;

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

private:
    std::unique_ptr<details::ClearLogMessagePrivate> m_pimpl;

};

/**
 * @class CurrentTimeMessage
 * @brief Реализация сообщений Установка текущего времени
 */
class CurrentTimeMessage final : public Message
{
public:
    CurrentTimeMessage();
    ~CurrentTimeMessage() NOEXCEPT override;

    CurrentTimeMessage(const CurrentTimeMessage& other);
    CurrentTimeMessage& operator =(const CurrentTimeMessage& other);

    CurrentTimeMessage(CurrentTimeMessage&& other) NOEXCEPT;
    CurrentTimeMessage& operator =(CurrentTimeMessage&& other) NOEXCEPT;

    /**
     * @brief serialize [override]
     */
    const QByteArray serialize() const override;

    /**
     * @brief parse [override]
     */
    bool parse(const QByteArray& src) override;

    /**
     * @brief currentTime - Возвращает установленное значение текущего времени
     * @return Значение текущего времени в секундах с начала эпохи (00:00:00 01-01-1970)
     */
    quint32 currentTime() const NOEXCEPT;

    /**
     * @brief setCurrentTime - Устанавливает значение текущего времени
     * @param epoch - Новое значение текущего времени в секундах с начала эпохи (00:00:00 01-01-1970)
     */
    void setCurrentTime(quint32 epoch) NOEXCEPT;

private:
    std::unique_ptr<details::CurrentTimeMessagePrivate> m_pimpl;

};

} // outcoming
} // protocol

#endif // PROTOCOL_PROTOCOL_H
