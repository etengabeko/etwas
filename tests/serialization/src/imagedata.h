#ifndef TESTS_SERIALIZATION_IMAGE_DATA_H
#define TESTS_SERIALIZATION_IMAGE_DATA_H

#include "basictest.h"

namespace test
{
namespace serialization
{

class ImageData : public BasicTest
{
public:
    explicit ImageData(QObject* parent = nullptr);

private:
    void makeTestData() override;

};

} // serialization
} // test

#endif // TESTS_SERIALIZATION_IMAGE_DATA_H
