#include "currenttime.h"

#include <QtTest>
#include <QByteArray>
#include <QDataStream>

#include "protocol/protocol.h"

using protocol::AbstractMessage;
using namespace protocol::outcoming;

namespace test
{
namespace serialization
{

CurrentTime::CurrentTime(QObject* parent) :
    BasicTest(parent)
{
    setObjectName("CurrentTime");
}

void CurrentTime::makeTestData()
{
    BasicTest::makeTestData();

    CurrentTimeMessage message;
    QByteArray content;
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x09) // type
               << quint32(0);  // time seconds since epoch
    }
    QTest::newRow("Empty message")
            << QSharedPointer<AbstractMessage>(new CurrentTimeMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new CurrentTimeMessage(message));
    content.clear();

    message.setCurrentTime(quint32(1234567890));
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x09)         // type
               << quint32(1234567890); // time seconds since epoch
    }
    QTest::newRow("Set current time")
            << QSharedPointer<AbstractMessage>(new CurrentTimeMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new CurrentTimeMessage(message));
    content.clear();
}

} // serialization
} // test
