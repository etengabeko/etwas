#ifndef UI_DISPLAY_CONTROL_WIDGET_H
#define UI_DISPLAY_CONTROL_WIDGET_H

#include <QPixmap>
#include <QString>
#include <QWidget>

class QLabel;
class QTimer;

namespace storage
{
class ImageStorage;
} // storage

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
    explicit DisplayControlWidget(const storage::ImageStorage* const storage,
                                  bool isDebugMode,
                                  QWidget* parent = nullptr);
    ~DisplayControlWidget();

    bool isFirstImageEnabled() const;
    void setFirstImageEnable(bool enabled);

    bool isSecondImageEnabled() const;
    void setSecondImageEnable(bool enabled);

    void resetFirstImage();
    void setFirstImage(quint8 imageIndex);
    int firstImageIndex() const;

    void resetSecondImage();
    void setSecondImage(quint8 imageIndex);
    int secondImageIndex() const;

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
    void reloadImages();

signals:
    void activated(bool enabled);

private slots:
    void slotTimeoutFirst();
    void slotTimeoutSecond();

private:
    void reloadImage(ImageNumber num);
    void setImage(ImageNumber num, const QPixmap& img);
    void resetImage(ImageNumber num);
    void setCurrentImage(ImageNumber num);

    void resetTimer();
    void resetCurrentImage();

private:
    Ui::DisplayControl* m_ui = nullptr;

    const storage::ImageStorage* const m_imgStorage;
    const bool m_isDebugMode;

    int m_brightLevel = 0;
    int m_timeOnMsec = 0;
    int m_timeOffMsec = 0;
    bool m_blinkingEnabled = false;

    int m_firstImageIndex;
    int m_secondImageIndex;

    QPixmap m_firstImagePixmap;
    QPixmap m_secondImagePixmap;
    QString m_withoutPixmapLabel;

    QTimer* m_timer = nullptr;
    QLabel* m_displayLabel = nullptr;

};

#endif // UI_DISPLAY_CONTROL_WIDGET_H
