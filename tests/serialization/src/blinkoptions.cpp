#include "blinkoptions.h"

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

BlinkOptions::BlinkOptions(QObject* parent) :
    BasicTest(parent)
{
    setObjectName("BlinkOptions");
}

void BlinkOptions::makeTestData()
{
    BasicTest::makeTestData();

    BlinkOptionsMessage message;
    QByteArray content;
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x04) // type
               << quint8(0)    // display number
               << quint8(0)    // time on
               << quint8(0);   // time off
    }
    QTest::newRow("Empty message")
            << QSharedPointer<AbstractMessage>(new BlinkOptionsMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new BlinkOptionsMessage(message));
    content.clear();

    message.setDisplayNumber(1);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x04) // type
               << quint8(1)    // display number
               << quint8(0)    // time on
               << quint8(0);   // time off
    }
    QTest::newRow("Display number only")
            << QSharedPointer<AbstractMessage>(new BlinkOptionsMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new BlinkOptionsMessage(message));
    content.clear();

    message.setDisplayNumber(11);
    message.setTimeOn(10);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x04) // type
               << quint8(11)   // display number
               << quint8(10)   // time on
               << quint8(0);   // time off
    }
    QTest::newRow("Display number & time on")
            << QSharedPointer<AbstractMessage>(new BlinkOptionsMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new BlinkOptionsMessage(message));
    content.clear();

    message.setDisplayNumber(111);
    message.setTimeOn(100);
    message.setTimeOff(200);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x04) // type
               << quint8(111)  // display number
               << quint8(100)  // time on
               << quint8(200); // time off
    }
    QTest::newRow("Display number & time on & time off")
            << QSharedPointer<AbstractMessage>(new BlinkOptionsMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new BlinkOptionsMessage(message));
    content.clear();
}

} // serialization
} // test
