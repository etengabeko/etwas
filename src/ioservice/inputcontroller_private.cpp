#include "inputcontroller_private.h"

#include <memory>

#include <QByteArray>
#include <QDataStream>
#include <QSharedPointer>

#include "transport.h"
#include "protocol/protocol.h"

namespace ioservice
{
namespace details
{

InputControllerPrivate::InputControllerPrivate(Transport* transport,
                                               protocol::MessageDirection direction,
                                               QObject* parent) :
    QObject(parent),
    m_transport(transport),
    m_direction(direction)
{
    Q_CHECK_PTR(m_transport);

    QObject::connect(m_transport, &Transport::received,
                     this, &InputControllerPrivate::slotReceive);
}

InputControllerPrivate::~InputControllerPrivate() NOEXCEPT
{

}

void InputControllerPrivate::slotReceive(const QByteArray& data)
{
    m_buffer += data;
    tryParseMessages();
}

void InputControllerPrivate::tryParseMessages()
{
    using protocol::AbstractMessage;

    QList<QSharedPointer<AbstractMessage>> parsedMessages;

    while (!m_buffer.isEmpty())
    {
        quint16 size = 0;
        {
            QDataStream in(&m_buffer, QIODevice::ReadOnly);
            in.setByteOrder(QDataStream::LittleEndian);
            in >> size;
            if (0 < size && size <= static_cast<size_t>(m_buffer.size()))
            {
                QByteArray forParsing(size, '\0');
                in.readRawData(forParsing.data(), size);
                std::unique_ptr<AbstractMessage> message = AbstractMessage::deserialize(m_direction, forParsing);
                if (message != nullptr)
                {
                    parsedMessages.append(QSharedPointer<AbstractMessage>(message.release()));
                }
            }
            else
            {
                break;
            }
        }
        m_buffer = m_buffer.right(m_buffer.size() - size - sizeof(size));
    }

    for (const QSharedPointer<AbstractMessage>& each : parsedMessages)
    {
        emit messageReceived(each);
    }
}

} // details
} // ioservice
