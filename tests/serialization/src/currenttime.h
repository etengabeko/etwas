#ifndef TESTS_SERIALIZATION_CURRENT_TIME_H
#define TESTS_SERIALIZATION_CURRENT_TIME_H

#include "basictest.h"

namespace test
{
namespace serialization
{

class CurrentTime : public BasicTest
{
public:
    explicit CurrentTime(QObject* parent = nullptr);

private:
    void makeTestData() override;

};

} // serialization
} // test

#endif // TESTS_SERIALIZATION_CURRENT_TIME_H
