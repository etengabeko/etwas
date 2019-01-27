#include <QApplication>

#include "storage/imagestorage.h"
#include "ui/displaycontrolwidget.h"
#include "ui/displayoptionswidget.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    enum
    {
        First  = 1,
        Second = 2
    };

    storage::ImageStorage storage;
    storage.addImage(First,  ":/on.bmp");
    storage.addImage(Second, ":/off.bmp");

    DisplayOptionsWidget* opt = new DisplayOptionsWidget(&storage);
    DisplayControlWidget* ctrl = new DisplayControlWidget(&storage, 255, true);

    opt->setFirstImage(First);
    ctrl->setFirstImage(First);

    opt->setSecondImage(Second);
    ctrl->setSecondImage(Second);

    ctrl->setFirstImageEnable(false);
    ctrl->setSecondImageEnable(false);

    QObject::connect(opt, &DisplayOptionsWidget::imageFirstEnabled,
                     ctrl, &DisplayControlWidget::setFirstImageEnable);
    QObject::connect(opt, &DisplayOptionsWidget::imageSecondEnabled,
                     ctrl, &DisplayControlWidget::setSecondImageEnable);
    QObject::connect(opt, &DisplayOptionsWidget::blinkingEnabled,
                     ctrl, &DisplayControlWidget::setBlinkingEnabled);
    QObject::connect(opt, &DisplayOptionsWidget::timeOnChanged,
                     ctrl, &DisplayControlWidget::setTimeOn);
    QObject::connect(opt, &DisplayOptionsWidget::timeOffChanged,
                     ctrl, &DisplayControlWidget::setTimeOff);
    QObject::connect(opt, &DisplayOptionsWidget::brightChanged,
                     ctrl, &DisplayControlWidget::setBrightLevel);

    opt->show();
    ctrl->show();

    int res = app.exec();

    delete opt;
    delete ctrl;

    return res;
}
