#include "brightoptions.h"

#include <QtTest>
#include <QByteArray>
#include <QDataStream>

#include "protocol/protocol.h"
#include "protocol/types.h"

using protocol::AbstractMessage;
using protocol::BrightLevel;
using namespace protocol::outcoming;

namespace test
{
namespace serialization
{
void BrightOptions::makeTestData()
{
    BasicTest::makeTestData();

    BrightOptionsMessage message;
    QByteArray content;
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x05) // type
               << quint8(0)    // display number
               << quint8(0);   // bright level
    }
    QTest::newRow("Empty message")
            << QSharedPointer<AbstractMessage>(new BrightOptionsMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new BrightOptionsMessage(message));
    content.clear();

    message.setDisplayNumber(2);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x05) // type
               << quint8(2)    // display number
               << quint8(0);   // bright level
    }
    QTest::newRow("Display number only")
            << QSharedPointer<AbstractMessage>(new BrightOptionsMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new BrightOptionsMessage(message));
    content.clear();

    message.setDisplayNumber(22);
    message.setBrightLevel(5);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x05) // type
               << quint8(22)   // display number
               << quint8(5);   // bright level
    }
    QTest::newRow("Display number & bright level")
            << QSharedPointer<AbstractMessage>(new BrightOptionsMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new BrightOptionsMessage(message));
    content.clear();

    message.setDisplayNumber(222);
    message.setBrightLevel(16);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x05) // type
               << quint8(222)  // display number
               << quint8(15); // bright level
    }
    QTest::newRow("Display number & incorrect bright level")
            << QSharedPointer<AbstractMessage>(new BrightOptionsMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new BrightOptionsMessage(message));
    content.clear();
}

} // serialization
} // test
