#ifndef TESTS_SERIALIZATION_DISPLAY_IMAGES_H
#define TESTS_SERIALIZATION_DISPLAY_IMAGES_H

#include "basictest.h"

namespace test
{
namespace serialization
{

class DisplayImages : public BasicTest
{
public:
    explicit DisplayImages(QObject* parent = nullptr);

private:
    void makeTestData() override;

};

} // serialization
} // test

#endif // TESTS_SERIALIZATION_DISPLAY_IMAGES_H
