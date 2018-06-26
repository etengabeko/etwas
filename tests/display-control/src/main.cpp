#include <QApplication>

#include "logger/logger.h"
#include "ui/displaycontrolwidget.h"
#include "ui/displayoptionswidget.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Logger::initialize(Logger::Level::Trace);

    const QString img1st(":/on.bmp");
    const QString img2nd(":/off.bmp");

    DisplayOptionsWidget opt;
    DisplayControlWidget* ctrl = new DisplayControlWidget(true);
    QObject::connect(&opt, &DisplayOptionsWidget::destroyed,
                     ctrl, &DisplayControlWidget::deleteLater);

    opt.setFirstImage(img1st);
    opt.setSecondImage(img2nd);

    QObject::connect(&opt, &DisplayOptionsWidget::imageFirstEnabled,
                     [ctrl, &img1st](bool enabled)
                     {
                         if (enabled)
                             ctrl->setFirstImage(img1st);
                         else
                             ctrl->resetFirstImage();
                     });
    QObject::connect(&opt, &DisplayOptionsWidget::imageSecondEnabled,
                     [ctrl, &img2nd](bool enabled)
                     {
                         if (enabled)
                             ctrl->setSecondImage(img2nd);
                         else
                             ctrl->resetSecondImage();
                     });
    QObject::connect(&opt, &DisplayOptionsWidget::blinkingEnabled,
                     [ctrl](bool enabled)
                     {
                         ctrl->setBlinkingEnabled(enabled);
                     });
    QObject::connect(&opt, &DisplayOptionsWidget::timeOnChanged,
                     [ctrl](int msec)
                     {
                         ctrl->setTimeOn(msec);
                     });
    QObject::connect(&opt, &DisplayOptionsWidget::timeOffChanged,
                     [ctrl](int msec)
                     {
                         ctrl->setTimeOff(msec);
                     });
    QObject::connect(&opt, &DisplayOptionsWidget::brightChanged,
                     [ctrl](int level)
                     {
                         ctrl->setBrightLevel(level);
                     });

    opt.show();
    ctrl->show();

    return app.exec();
}
