#ifndef TESTS_SERIALIZATION_DISPLAY_OPTIONS_H
#define TESTS_SERIALIZATION_DISPLAY_OPTIONS_H

#include "basictest.h"

namespace test
{
namespace serialization
{

class DisplayOptions : public BasicTest
{
public:
    explicit DisplayOptions(QObject* parent = nullptr);

private:
    void makeTestData() override;

};

} // serialization
} // test

#endif // TESTS_SERIALIZATION_DISPLAY_OPTIONS_H
