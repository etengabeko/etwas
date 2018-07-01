#ifndef UI_DISPLAY_OPTIONS_WIDGET_H
#define UI_DISPLAY_OPTIONS_WIDGET_H

#include <ui/subwindow.h>

class QLabel;

namespace storage
{
class ImageStorage;
} // storage

namespace Ui
{
class DisplayOptions;
} // Ui

class DisplayOptionsWidget : public SubWindow
{
    Q_OBJECT

public:
    explicit DisplayOptionsWidget(const storage::ImageStorage* const storage,
                                  QWidget* parent = nullptr);
    ~DisplayOptionsWidget();

    void setFirstImage(int imageIndex);
    void setFirstImageEnabled(bool enabled);

    void setSecondImage(int imageIndex);
    void setSecondImageEnabled(bool enabled);

    void setBlinkingEnabled(bool enabled);

    void setTimeOn(int msec);
    void setTimeOff(int msec);

    void setBrightLevel(int level);

signals:
    void imageFirstEnabled(bool enabled);
    void imageSecondEnabled(bool enabled);
    void blinkingEnabled(bool enabled);

    void timeOnChanged(int msec);
    void timeOffChanged(int msec);

    void brightChanged(int level);

    void imageFirstSelected();
    void imageSecondSelected();

public slots:
    void reloadImages();

private slots:
    void slotChangeBrightness(int level);

private:
    void reloadImage(int imageIndex, QLabel* dest);

private:
    Ui::DisplayOptions* m_ui = nullptr;

    const storage::ImageStorage* const m_imgStorage;

    int m_firstImageIndex;
    int m_secondImageIndex;

};

#endif // UI_DISPLAY_OPTIONS_WIDGET_H
