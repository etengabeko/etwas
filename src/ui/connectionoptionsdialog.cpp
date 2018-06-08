#include "connectionoptionsdialog.h"
#include "ui_connectionoptions.h"

#include <QString>

ConnectionOptionsDialog::ConnectionOptionsDialog(QWidget* parent) :
    QDialog(parent),
    m_ui(new Ui::ConnectionOptions())
{
    m_ui->setupUi(this);

    QObject::connect(m_ui->applyButton, &QPushButton::clicked,
                     this, &ConnectionOptionsDialog::accept);
    QObject::connect(m_ui->cancelButton, &QPushButton::clicked,
                     this, &ConnectionOptionsDialog::reject);
}

ConnectionOptionsDialog::~ConnectionOptionsDialog()
{
    delete m_ui;
    m_ui = nullptr;
}

QString ConnectionOptionsDialog::address() const
{
    return m_ui->addressLineEdit->text();
}

void ConnectionOptionsDialog::setAddress(const QString& addr)
{
    m_ui->addressLineEdit->setText(addr);
}

quint16 ConnectionOptionsDialog::port() const
{
    return m_ui->portSpinBox->value();
}

void ConnectionOptionsDialog::setPort(quint16 portnum)
{
    m_ui->portSpinBox->setValue(portnum);
}
