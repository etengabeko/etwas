#ifndef TESTS_SERIALIZATION_NEXT_LOG_H
#define TESTS_SERIALIZATION_NEXT_LOG_H

#include "basictest.h"

namespace test
{
namespace serialization
{

class NextLog : public BasicTest
{
public:
    explicit NextLog(QObject* parent = nullptr);

private:
    void makeTestData() override;

};

} // serialization
} // test

#endif // TESTS_SERIALIZATION_NEXT_LOG_H
