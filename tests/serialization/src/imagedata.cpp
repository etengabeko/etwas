#include "imagedata.h"

#include <QtTest>
#include <QByteArray>
#include <QDataStream>

#include "protocol/protocol.h"
#include "protocol/types.h"

using protocol::AbstractMessage;
using namespace protocol::outcoming;

namespace test
{
namespace serialization
{

ImageData::ImageData(QObject* parent) :
    BasicTest(parent)
{
    setObjectName("ImageData");
}

void ImageData::makeTestData()
{
    BasicTest::makeTestData();

    ImageDataMessage message;
    QByteArray content;
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x06) // type
               << quint8(0);   // image number
    }
    QTest::newRow("Empty message")
            << QSharedPointer<AbstractMessage>(new ImageDataMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new ImageDataMessage(message));
    content.clear();

    message.setImageNumber(5);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x06) // type
               << quint8(5);   // image number
    }
    QTest::newRow("Image number only")
            << QSharedPointer<AbstractMessage>(new ImageDataMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new ImageDataMessage(message));
    content.clear();

    message.setImageNumber(55);
    message.setImageColors({ qRgb(5, 5, 5) });
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x06)     // type
               << quint8(55)       // image number
               << quint16(0x28A5); // color
    }
    QTest::newRow("image number & one color")
            << QSharedPointer<AbstractMessage>(new ImageDataMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new ImageDataMessage(message));
    content.clear();

    message.setImageNumber(255);
    message.setImageColors({
                               qRgb(0, 0, 0),
                               qRgb(1, 1, 1),
                               qRgb(16, 16, 16),
                               qRgb(31, 31, 31),
                               qRgb(32, 32, 32),
                               qRgb(32, 63, 32),
                               qRgb(63, 63, 63),
                               qRgb(255, 255, 255)
                           });
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x06)     // type
               << quint8(255);     // image number
        // colors
        stream << quint16(0x0000)
               << quint16(0x0821)
               << quint16(0x8210)
               << quint16(0xFBFF)
               << quint16(0x0400)
               << quint16(0x07e0)
               << quint16(0xFFFF)
               << quint16(0xFFFF);
    }
    QTest::newRow("image number & many colors")
            << QSharedPointer<AbstractMessage>(new ImageDataMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new ImageDataMessage(message));
    content.clear();
}

} // serialization
} // test
