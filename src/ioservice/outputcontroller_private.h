#ifndef IOSERVICE_OUTPUT_CONTROLLER_PRIVATE_H
#define IOSERVICE_OUTPUT_CONTROLLER_PRIVATE_H

#include <QObject>

namespace protocol
{
class AbstractMessage;
} // protocol

namespace ioservice
{
class Transport;

namespace details
{
class OutputControllerPrivate : public QObject
{
    Q_OBJECT

public:
    explicit OutputControllerPrivate(Transport* transport,
                                     QObject* parent = nullptr);
    ~OutputControllerPrivate() NOEXCEPT;

    void send(const protocol::AbstractMessage& message);

signals:
    void sent(const QByteArray& data);

private:
    Transport* m_transport = nullptr;

};

} // details
} // ioservice

#endif // IOSERVICE_OUTPUT_CONTROLLER_PRIVATE_H
