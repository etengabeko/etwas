#include "nextlog.h"

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

NextLog::NextLog(QObject* parent) :
    BasicTest(parent)
{
    setObjectName("NextLog");
}

void NextLog::makeTestData()
{
    BasicTest::makeTestData();

    NextLogMessage message;
    QByteArray content;
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x04); // type
    }
    QTest::newRow("Empty message")
            << QSharedPointer<AbstractMessage>(new NextLogMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new NextLogMessage(message));
    content.clear();

    const QByteArray data("Hello world!");

    message.setData(data);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x04); // type
        stream.writeRawData(data.constData(), data.size()); // log-file messages count
    }
    QTest::newRow("Set data")
            << QSharedPointer<AbstractMessage>(new NextLogMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new NextLogMessage(message));
    content.clear();
}

} // serialization
} // test
