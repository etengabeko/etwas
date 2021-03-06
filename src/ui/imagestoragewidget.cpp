#include "imagestoragewidget.h"
#include "ui_imagestorage.h"

#include <QAction>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QTableWidgetItem>

#include "storage/imagestorage.h"

namespace
{

const int kMaxImagesCount = 256;

enum Column
{
    Number = 0,
    Image,
    FileName
};

}

ImageStorageWidget::ImageStorageWidget(storage::ImageStorage* storage,
                                       QWidget* parent) :
    SubWindow(parent),
    m_ui(new Ui::ImageStorage()),
    m_storage(storage)
{
    Q_ASSERT(m_storage);

    m_ui->setupUi(this);

    m_ui->imagesTableWidget->setSelectionBehavior(QTableWidget::SelectionBehavior::SelectRows);
    m_ui->imagesTableWidget->setSelectionMode(QTableWidget::ExtendedSelection);
    m_ui->imagesTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(m_ui->imagesTableWidget->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &ImageStorageWidget::slotChangeSelection);
    QObject::connect(m_ui->imagesTableWidget, &QTableWidget::customContextMenuRequested,
                     this, &ImageStorageWidget::slotShowContextMenu);

    QObject::connect(m_ui->addButton, &QPushButton::clicked,
                     this, &ImageStorageWidget::slotAddImages);
    QObject::connect(m_ui->removeButton, &QPushButton::clicked,
                     this, &ImageStorageWidget::slotRemoveImages);
    QObject::connect(m_ui->uploadButton, &QPushButton::clicked,
                     this, &ImageStorageWidget::uploadNeeded);
    QObject::connect(m_ui->selectButton, &QPushButton::clicked,
                     this, &ImageStorageWidget::slotSelect);
    QObject::connect(m_ui->cancelButton, &QPushButton::clicked,
                     this, &ImageStorageWidget::close);

    loadAllImages();
}

ImageStorageWidget::~ImageStorageWidget()
{
    delete m_ui;
    m_ui = nullptr;
}

void ImageStorageWidget::loadAllImages()
{
    m_ui->imagesTableWidget->clearContents();

    QMapIterator<quint8, QString> it(m_storage->availableImages());
    while (it.hasNext())
    {
        quint8 index = it.peekNext().key();
        const QString& fileName = it.next().value();
        addRow(index, fileName);
    }

    m_ui->imagesTableWidget->resizeColumnsToContents();
}

void ImageStorageWidget::addRow(quint8 imageIndex, const QString& fileName)
{
    const int row = m_ui->imagesTableWidget->rowCount();
    m_ui->imagesTableWidget->setRowCount(row + 1);

    QTableWidgetItem* item = new QTableWidgetItem(QString::number(imageIndex));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_ui->imagesTableWidget->setItem(row, Column::Number, item);
    item = new QTableWidgetItem(fileName);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_ui->imagesTableWidget->setItem(row, Column::FileName, item);

    QLabel* label = new QLabel(this);
    const QPixmap pxmap(fileName);
    label->setPixmap(pxmap);
    m_ui->imagesTableWidget->setCellWidget(row, Column::Image, label);
    m_ui->imagesTableWidget->setRowHeight(row, pxmap.height());
}

void ImageStorageWidget::slotChangeSelection()
{
    const bool enabled = m_ui->imagesTableWidget->selectionModel()->hasSelection();
    m_ui->removeButton->setEnabled(enabled);
    m_ui->selectButton->setEnabled(enabled);
}

void ImageStorageWidget::slotSelect()
{
    if (m_ui->imagesTableWidget->selectionModel()->hasSelection())
    {
        const int row = m_ui->imagesTableWidget->selectedItems().first()->row();
        emit imageSelected(static_cast<quint8>(m_ui->imagesTableWidget->item(row, Column::Number)->text().toUShort()));
    }
}

void ImageStorageWidget::slotAddImages()
{
    const QStringList fileNames = QFileDialog::getOpenFileNames(nullptr,
                                                                tr("Select Images files"),
                                                                QApplication::applicationDirPath(),
                                                                tr("Images (*.bmp);;All files (*)"));
    const int kCount = m_ui->imagesTableWidget->rowCount();
    int index = kCount > 0 ? (m_ui->imagesTableWidget->item(kCount-1, Column::Number)->text().toInt() + 1)
                           : kCount;
    for (const QString& each : fileNames)
    {
        if (index >= kMaxImagesCount)
        {
            QMessageBox::warning(nullptr,
                                 tr("Error Load images"),
                                 tr("Count of loaded images (%1) greater than available (%2).")
                                 .arg(kCount + fileNames.size()).arg(kMaxImagesCount));
            break;
        }
        addRow(static_cast<quint8>(index++), each);
    }

    m_ui->imagesTableWidget->resizeColumnsToContents();

    syncStorage();
}

void ImageStorageWidget::syncStorage()
{
    const int count = m_ui->imagesTableWidget->rowCount();
    if (count > kMaxImagesCount)
    {
        return;
    }

    for (int row = 0; row < count; ++row)
    {
        m_storage->addImage(static_cast<quint8>(row),
                            m_ui->imagesTableWidget->item(row, Column::FileName)->text());
    }
    m_ui->imagesTableWidget->scrollToBottom();
}

void ImageStorageWidget::slotRemoveImages()
{
    if (m_ui->imagesTableWidget->selectionModel()->hasSelection())
    {
        const int row = m_ui->imagesTableWidget->selectedItems().first()->row();
        m_storage->removeImage(static_cast<quint8>(m_ui->imagesTableWidget->item(row, Column::Number)->text().toUShort()));
        m_ui->imagesTableWidget->removeRow(row);
    }
}

void ImageStorageWidget::slotShowContextMenu(const QPoint& pos)
{
    QMenu menu;

    QAction* addAction = menu.addAction(tr("Add"));
    QAction* removeAction = menu.addAction(tr("Remove"));
    QAction* selectAction = nullptr;

    QTableWidgetItem* item = m_ui->imagesTableWidget->itemAt(pos);
    if (item != nullptr)
    {
        menu.addSeparator();
        selectAction = menu.addAction(tr("Select"));
    }
    else
    {
        removeAction->setEnabled(false);
    }

    QAction* answer = menu.exec(m_ui->imagesTableWidget->viewport()->mapToGlobal(pos));
    if (answer == addAction)
    {
        slotAddImages();
    }
    else if (answer == removeAction)
    {
        slotRemoveImages();
    }
    else if (answer == selectAction)
    {
        slotSelect();
    }
}
