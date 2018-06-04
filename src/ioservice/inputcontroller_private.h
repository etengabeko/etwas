#ifndef IOSERVICE_INPUT_CONTROLLER_PRIVATE_H
#define IOSERVICE_INPUT_CONTROLLER_PRIVATE_H

#include <functional>

#include <QObject>

template <typename T>
class QSharedPointer;

namespace protocol
{
class AbstractMessage;
enum class MessageDirection;
} // protocol

namespace ioservice
{
class Transport;

namespace details
{
class InputControllerPrivate : public QObject
{
    Q_OBJECT

public:
    explicit InputControllerPrivate(Transport* transport,
                                    protocol::MessageDirection direction,
                                    std::function<void(const QSharedPointer<protocol::AbstractMessage>&)> onReceive,
                                    QObject* parent = nullptr);
    ~InputControllerPrivate() noexcept = default;

private slots:
    void slotReceive(const QByteArray& data);

private:
    void tryParseMessages();

private:
    Transport* m_transport = nullptr;
    const protocol::MessageDirection m_direction;
    std::function<void(const QSharedPointer<protocol::AbstractMessage>&)> m_onReceive;

    QByteArray m_buffer;

};

} // details
} // ioservice

#endif // IOSERVICE_INPUT_CONTROLLER_PRIVATE_H
