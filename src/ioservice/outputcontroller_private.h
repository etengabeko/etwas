#ifndef IOSERVICE_OUTPUT_CONTROLLER_PRIVATE_H
#define IOSERVICE_OUTPUT_CONTROLLER_PRIVATE_H

#include <QObject>

#include "asyncqueue.h"

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
    void hasDataToSend();

private:
    Transport* m_transport = nullptr;
    AsyncQueue<QByteArray> m_dataQueue;

};

} // details
} // ioservice

#endif // IOSERVICE_OUTPUT_CONTROLLER_PRIVATE_H
