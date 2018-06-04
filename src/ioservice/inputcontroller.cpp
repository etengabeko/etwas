#include "inputcontroller.h"
#include "inputcontroller_private.h"

#include <functional>

#include <QSharedPointer>

#include "transport.h"
#include "protocol/protocol.h"

namespace ioservice
{

InputController::InputController(Transport* transport,
                                 protocol::MessageDirection direction,
                                 QObject* parent) :
    QObject(parent),
    m_pimpl(new details::InputControllerPrivate(transport,
                                                direction,
                                                std::bind(&InputController::onMessageReceived,
                                                          this,
                                                          std::placeholders::_1)))
{

}

InputController::~InputController() noexcept
{
    m_pimpl.reset();
}

void InputController::onMessageReceived(const QSharedPointer<protocol::AbstractMessage>& message)
{
    if (message != nullptr)
    {
        emit messageReceived(message);
    }
}

} // ioservice
