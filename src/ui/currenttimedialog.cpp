#include "currenttimedialog.h"
#include "ui_currenttime.h"

#include <QDateTime>

CurrentTimeDialog::CurrentTimeDialog(QWidget* parent) :
    QDialog(parent),
    m_ui(new Ui::CurrentTime())
{
    m_ui->setupUi(this);

    m_ui->timeEdit->setDisplayFormat("hh:mm:ss");

    QObject::connect(m_ui->applyButton, &QPushButton::clicked,
                     this, &CurrentTimeDialog::accept);
    QObject::connect(m_ui->cancelButton, &QPushButton::clicked,
                     this, &CurrentTimeDialog::reject);
    QObject::connect(m_ui->dateEdit, &QDateEdit::dateChanged,
                     this, &CurrentTimeDialog::slotUpdateTimestamp);
    QObject::connect(m_ui->timeEdit, &QTimeEdit::timeChanged,
                     this, &CurrentTimeDialog::slotUpdateTimestamp);
}

CurrentTimeDialog::~CurrentTimeDialog()
{
    delete m_ui;
    m_ui = nullptr;
}

quint32 CurrentTimeDialog::currentTimestamp() const
{
    return (m_ui->epochLineEdit->text().toULongLong());
}

void CurrentTimeDialog::setCurrentTimestamp(quint32 epoch)
{
    const QDateTime dt = QDateTime::fromSecsSinceEpoch(epoch);
    m_ui->dateEdit->setDate(dt.date());
    m_ui->timeEdit->setTime(dt.time());
}

void CurrentTimeDialog::slotUpdateTimestamp()
{
    const QDateTime dt = QDateTime(m_ui->dateEdit->date(),
                                   m_ui->timeEdit->time());
    m_ui->epochLineEdit->setText(QString::number(dt.toSecsSinceEpoch()));
}
