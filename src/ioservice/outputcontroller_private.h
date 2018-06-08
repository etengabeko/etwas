#ifndef IOSERVICE_OUTPUT_CONTROLLER_PRIVATE_H
#define IOSERVICE_OUTPUT_CONTROLLER_PRIVATE_H

namespace protocol
{
class AbstractMessage;
} // protocol

namespace ioservice
{
class Transport;

namespace details
{
class OutputControllerPrivate
{
public:
    explicit OutputControllerPrivate(Transport* transport);
    ~OutputControllerPrivate() NOEXCEPT;

    void send(const protocol::AbstractMessage& message);

private:
    Transport* m_transport = nullptr;

};

} // details
} // ioservice

#endif // IOSERVICE_OUTPUT_CONTROLLER_PRIVATE_H
