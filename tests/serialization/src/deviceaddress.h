#ifndef TESTS_SERIALIZATION_DEVICE_ADDRESS_H
#define TESTS_SERIALIZATION_DEVICE_ADDRESS_H

#include "basictest.h"

namespace test
{
namespace serialization
{

class DeviceAddress : public BasicTest
{
public:
    explicit DeviceAddress(QObject* parent = nullptr);

private:
    void makeTestData() override;

};

} // serialization
} // test

#endif // TESTS_SERIALIZATION_DEVICE_ADDRESS_H
