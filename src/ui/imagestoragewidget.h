#ifndef UI_IMAGE_STORAGE_WIDGET_H
#define UI_IMAGE_STORAGE_WIDGET_H

#include <ui/subwindow.h>

namespace storage
{
class ImageStorage;
} // storage

namespace Ui
{
class ImageStorage;
} // Ui

class ImageStorageWidget : public SubWindow
{
    Q_OBJECT

public:
    ImageStorageWidget(storage::ImageStorage* storage,
                       QWidget* parent = nullptr);
    ~ImageStorageWidget();

signals:
    void imageSelected(quint8 imageIndex);

private slots:
    void slotSelect();
    void slotAddImages();
    void slotRemoveImages();

private:
    void loadAllImages();
    void syncStorage();

private:
    Ui::ImageStorage* m_ui;

    storage::ImageStorage* m_storage;

};

#endif // UI_IMAGE_STORAGE_WIDGET_H
