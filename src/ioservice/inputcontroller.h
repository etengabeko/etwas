#ifndef IOSERVICE_INPUT_CONTROLLER_H
#define IOSERVICE_INPUT_CONTROLLER_H

#include <memory>

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
namespace details
{
class InputControllerPrivate;
} // details

class Transport;

class InputController : public QObject
{
    Q_OBJECT

public:
    explicit InputController(Transport* transport,
                             protocol::MessageDirection direction,
                             QObject* parent = nullptr);
    ~InputController() NOEXCEPT;

signals:
    void messageReceived(const QSharedPointer<protocol::AbstractMessage>& message);

private:
    void onMessageReceived(const QSharedPointer<protocol::AbstractMessage>& message);

private:
    std::unique_ptr<details::InputControllerPrivate> m_pimpl;

};

} // ioservice

#endif // IOSERVICE_INPUT_CONTROLLER_H
