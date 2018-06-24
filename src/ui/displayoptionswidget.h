#ifndef UI_DISPLAY_OPTIONS_WIDGET_H
#define UI_DISPLAY_OPTIONS_WIDGET_H

#include <QWidget>

class QPixmap;

namespace Ui
{
class DisplayOptions;
} // Ui

class DisplayOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayOptionsWidget(QWidget* parent = nullptr);
    ~DisplayOptionsWidget();

    void setFirstImage(const QPixmap& img);
    void setSecondImage(const QPixmap& img);

signals:
    void imageFirstEnabled(bool enabled);
    void imageSecondEnabled(bool enabled);
    void blinkingEnabled(bool enabled);

    void timeOnChanged(int msec);
    void timeOffChanged(int msec);

    void brightChanged(int level);

private:
    Ui::DisplayOptions* m_ui = nullptr;

};

#endif // UI_DISPLAY_OPTIONS_WIDGET_H
