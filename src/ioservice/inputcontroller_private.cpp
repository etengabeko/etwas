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
                                               std::function<void (const QSharedPointer<protocol::AbstractMessage>&)> onReceive,
                                               QObject* parent) :
    QObject(parent),
    m_transport(transport),
    m_direction(direction),
    m_onReceive(onReceive)
{
    Q_CHECK_PTR(m_transport);
    Q_ASSERT(m_onReceive);

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

    const int kBufferSize = m_buffer.size();
    int currentIndex = 0;
    int lastSuccessIndex = 0;

    while (currentIndex < kBufferSize)
    {
        quint16 size = 0;
        if (static_cast<size_t>(kBufferSize - currentIndex) <= sizeof(size))
        {
            break;
        }

        QByteArray stub(m_buffer.right(kBufferSize - currentIndex));
        QDataStream in(&stub, QIODevice::ReadOnly);
        in.setByteOrder(QDataStream::LittleEndian);
        in >> size;
        if (0 < size && size <= static_cast<size_t>(kBufferSize))
        {
            QByteArray forParsing(size, '\0');
            in.readRawData(forParsing.data(), size);
            std::unique_ptr<AbstractMessage> message = AbstractMessage::deserialize(m_direction, forParsing);
            if (message != nullptr)
            {
                parsedMessages.append(QSharedPointer<AbstractMessage>(message.release()));
                currentIndex += sizeof(size) + size;
                lastSuccessIndex = currentIndex;
                continue;
            }
        }
        ++currentIndex;
    }

    if (!parsedMessages.isEmpty())
    {
        m_buffer = m_buffer.right(kBufferSize - lastSuccessIndex);

        for (const QSharedPointer<AbstractMessage>& each : parsedMessages)
        {
            m_onReceive(each);
        }
    }
}

} // details
} // ioservice
