#ifndef TESTS_SERIALIZATION_BUTTONS_STATE_H
#define TESTS_SERIALIZATION_BUTTONS_STATE_H

#include "basictest.h"

namespace test
{
namespace serialization
{

class ButtonsState : public BasicTest
{
public:
    explicit ButtonsState(QObject* parent = nullptr);

private:
    void makeTestData() override;

};

} // serialization
} // test

#endif // TESTS_SERIALIZATION_BUTTONS_STATE_H
