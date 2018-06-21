#ifndef TESTS_SERIALIZATION_BLINK_OPTIONS_H
#define TESTS_SERIALIZATION_BLINK_OPTIONS_H

#include "basictest.h"

namespace test
{
namespace serialization
{

class BlinkOptions : public BasicTest
{
public:
    explicit BlinkOptions(QObject* parent = nullptr);

private:
    void makeTestData() override;

};

} // serialization
} // test

#endif // TESTS_SERIALIZATION_BLINK_OPTIONS_H
