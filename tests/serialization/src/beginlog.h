#ifndef TESTS_SERIALIZATION_BEGIN_LOG_H
#define TESTS_SERIALIZATION_BEGIN_LOG_H

#include "basictest.h"

namespace test
{
namespace serialization
{

class BeginLog : public BasicTest
{
public:
    explicit BeginLog(QObject* parent = nullptr);

private:
    void makeTestData() override;

};

} // serialization
} // test

#endif // TESTS_SERIALIZATION_BEGIN_LOG_H
