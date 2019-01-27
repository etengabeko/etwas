#include "beginlog.h"

#include <QtTest>
#include <QByteArray>
#include <QDataStream>

#include "protocol/protocol.h"

using protocol::AbstractMessage;
using namespace protocol::incoming;

namespace test
{
namespace serialization
{

BeginLog::BeginLog(QObject* parent) :
    BasicTest(parent)
{
    setObjectName("BeginLog");
}

void BeginLog::makeTestData()
{
    BasicTest::makeTestData();

    BeginLogMessage message;
    QByteArray content;
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x03) // type
               << quint32(0);  // log-file messages count
    }
    QTest::newRow("Empty message")
            << QSharedPointer<AbstractMessage>(new BeginLogMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new BeginLogMessage(message));
    content.clear();

    message.setCount(quint32(1234567890));
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x03)         // type
               << quint32(1234567890); // log-file messages count
    }
    QTest::newRow("Set count")
            << QSharedPointer<AbstractMessage>(new BeginLogMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new BeginLogMessage(message));
    content.clear();
}

} // serialization
} // test
