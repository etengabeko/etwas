#ifndef IOSERVICE_INPUT_CONTROLLER_PRIVATE_H
#define IOSERVICE_INPUT_CONTROLLER_PRIVATE_H

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
                                    QObject* parent = nullptr);
    ~InputControllerPrivate() NOEXCEPT;

signals:
    void messageReceived(const QSharedPointer<protocol::AbstractMessage>& message);

private slots:
    void slotReceive(const QByteArray& data);

private:
    void tryParseMessages();

private:
    Transport* m_transport = nullptr;
    const protocol::MessageDirection m_direction;

    QByteArray m_buffer;

};

} // details
} // ioservice

#endif // IOSERVICE_INPUT_CONTROLLER_PRIVATE_H
