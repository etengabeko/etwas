#include "buttonsstate.h"

#include <QtTest>
#include <QByteArray>
#include <QDataStream>

#include "protocol/protocol.h"

namespace
{

int maxButtonsStatesCount() { return 32; }

}

using protocol::AbstractMessage;
using namespace protocol::incoming;

namespace test
{
namespace serialization
{

void ButtonsState::makeTestData()
{
    BasicTest::makeTestData();

    QByteArray states(::maxButtonsStatesCount(), '\0');

    ButtonsStateMessage message;
    QByteArray content;
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x02); // type
        stream.writeRawData(states.constData(), states.size());
    }
    QTest::newRow("Empty message")
            << QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message));
    content.clear();
}

} // serialization
} // test
