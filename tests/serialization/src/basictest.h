#ifndef TESTS_SERIALIZATION_BASIC_TEST_H
#define TESTS_SERIALIZATION_BASIC_TEST_H

#include <QObject>
#include <QSharedPointer>

#include "protocol/protocol.h"

Q_DECLARE_METATYPE(QSharedPointer<protocol::AbstractMessage>)

namespace test
{
namespace serialization
{

class BasicTest : public QObject
{
    Q_OBJECT

public:
    BasicTest() = default;
    virtual ~BasicTest() = default;

private slots:
    void serialize_data();
    void serialize();

    void deserialize_data();
    void deserialize();

protected:
    virtual void makeTestData() = 0;

};

} // serialization
} // test

#endif // TESTS_SERIALIZATION_BASIC_TEST_H
