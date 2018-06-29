#ifndef UI_DISPLAY_CONTROL_WIDGET_H
#define UI_DISPLAY_CONTROL_WIDGET_H

#include <QPixmap>
#include <QString>
#include <QWidget>

class QLabel;
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

    bool isFirstImageEnabled() const;
    const QString& firstImage() const;
    void setFirstImage(const QString& pixmapFileName);

    bool isSecondImageEnabled() const;
    const QString& secondImage() const;
    void setSecondImage(const QString& pixmapFileName);

    void resetFirstImage();
    void resetSecondImage();

    int brightLevel() const;
    void setBrightLevel(int level);

    bool isBlinkingEnabled() const;
    void setBlinkingEnabled(bool enabled);

    int timeOn() const;
    void setTimeOn(int msec);

    int timeOff() const;
    void setTimeOff(int msec);

    bool isActive() const;

public slots:
    void highlight(bool enabled);
    void setActive(bool enabled);

signals:
    void activated(bool enabled);

private slots:
    void slotTimeoutFirst();
    void slotTimeoutSecond();

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

    QString m_firstImage;
    QString m_secondImage;

    QPixmap m_firstImagePixmap;
    QPixmap m_secondImagePixmap;

    QTimer* m_timer = nullptr;
    QLabel* m_displayLabel = nullptr;

};

#endif // UI_DISPLAY_CONTROL_WIDGET_H
