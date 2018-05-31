#include "basictest.h"

#include <memory>

#include <QtTest>
#include <QByteArray>

using protocol::AbstractMessage;

namespace test
{
namespace serialization
{

void BasicTest::serialize()
{
    QFETCH(QSharedPointer<AbstractMessage>, source);
    QFETCH(QByteArray, serialized);

    QVERIFY(source != nullptr);

    QCOMPARE(source->serialize(), serialized);
}

void BasicTest::deserialize()
{
    QFETCH(QSharedPointer<AbstractMessage>, source);
    QFETCH(QByteArray, serialized);
    QFETCH(QSharedPointer<AbstractMessage>, repaired);

    QVERIFY(source != nullptr);

    std::unique_ptr<AbstractMessage> parsed = AbstractMessage::deserialize(source->direction(), serialized);

    if (repaired == nullptr)
    {
        QVERIFY(parsed == nullptr);
    }
    else
    {
        QCOMPARE(repaired->serialize(), parsed->serialize());
    }
}

void BasicTest::serialize_data()
{
    makeTestData();
}

void BasicTest::deserialize_data()
{
    makeTestData();
}

void BasicTest::makeTestData()
{
    QTest::addColumn<QSharedPointer<AbstractMessage>>("source");
    QTest::addColumn<QByteArray>("serialized");
    QTest::addColumn<QSharedPointer<AbstractMessage>>("repaired");
}

} // serialization
} // test
