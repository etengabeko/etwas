#ifndef UI_IMAGE_STORAGE_WIDGET_H
#define UI_IMAGE_STORAGE_WIDGET_H

#include <ui/subwindow.h>

class QAction;

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

private slots:
    void slotApply();
    void slotAddImages();
    void slotRemoveImages();

private:
    void loadAllImages();
    void syncStorage();

private:
    Ui::ImageStorage* m_ui;
    storage::ImageStorage* m_storage;

    QAction* m_addAction = nullptr;
    QAction* m_rmAction = nullptr;

};

#endif // UI_IMAGE_STORAGE_WIDGET_H
