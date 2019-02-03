#include "outputcontroller_private.h"

#include <QByteArray>
#include <QDataStream>

#include "transport.h"
#include "protocol/protocol.h"

namespace ioservice
{
namespace details
{

OutputControllerPrivate::OutputControllerPrivate(Transport* transport,
                                                 QObject *parent) :
    QObject(parent),
    m_transport(transport)
{
    Q_CHECK_PTR(transport);

    m_transport->setDataQueue(&m_dataQueue);
    QObject::connect(this, &OutputControllerPrivate::hasDataToSend,
                     m_transport, &Transport::sendQueuedData);
}

OutputControllerPrivate::~OutputControllerPrivate() NOEXCEPT
{

}

void OutputControllerPrivate::send(const protocol::AbstractMessage& message)
{
    QByteArray ba;
    {
        QDataStream stream(&ba, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << message;
    }
    m_dataQueue.enqueue(ba);

    emit hasDataToSend();
}

} // details
} // ioservice
