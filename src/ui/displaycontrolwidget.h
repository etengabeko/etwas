#ifndef UI_DISPLAY_CONTROL_WIDGET_H
#define UI_DISPLAY_CONTROL_WIDGET_H

#include <QPixmap>
#include <QWidget>

class QLabel;
class QString;
class QTimer;

namespace Ui
{
class DisplayControl;
} // Ui

enum class ImageNumber
{
    First = 1,
    Second
};

class DisplayControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayControlWidget(bool isDebugMode = false, QWidget* parent = nullptr);
    ~DisplayControlWidget();

    void setFirstImage(const QString& pixmapFileName);
    void setSecondImage(const QString& pixmapFileName);

    void resetFirstImage();
    void resetSecondImage();

    void setBrightLevel(int level);

    void setBlinkingEnabled(bool enabled);
    void setTimeOn(int msec);
    void setTimeOff(int msec);

public slots:
    void setActive(bool enabled);

signals:
    void activated(bool enabled);

private slots:
    void slotTimeout();

private:
    void setImage(ImageNumber num, const QPixmap& img);
    void resetImage(ImageNumber num);
    void setCurrentImage(ImageNumber num);

    void resetTimer();
    void resetCurrentImage();

private:
    Ui::DisplayControl* m_ui = nullptr;
    const bool m_isDebugMode;

    int m_brightLevel = 0;
    int m_timeOnMsec = 0;
    int m_timeOffMsec = 0;
    bool m_blinkingEnabled = false;

    QPixmap m_firstImage;
    QPixmap m_secondImage;

    QTimer* m_timer = nullptr;
    QLabel* m_displayLabel = nullptr;

};

#endif // UI_DISPLAY_CONTROL_WIDGET_H
