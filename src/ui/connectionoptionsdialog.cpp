#include "connectionoptionsdialog.h"
#include "ui_connectionoptions.h"

#include <QApplication>
#include <QCursor>
#include <QHostInfo>
#include <QString>

ConnectionOptionsDialog::ConnectionOptionsDialog(QWidget* parent) :
    QDialog(parent),
    m_ui(new Ui::ConnectionOptions())
{
    m_ui->setupUi(this);
    m_ui->errorLabel->setStyleSheet("QLabel { color: red }");

    QObject::connect(m_ui->applyButton, &QPushButton::clicked,
                     this, &ConnectionOptionsDialog::slotApply);
    QObject::connect(m_ui->cancelButton, &QPushButton::clicked,
                     this, &ConnectionOptionsDialog::reject);
}

ConnectionOptionsDialog::~ConnectionOptionsDialog()
{
    delete m_ui;
    m_ui = nullptr;
}

void ConnectionOptionsDialog::setErrorString(const QString& message)
{
    m_ui->errorLabel->setText(message);
}

void ConnectionOptionsDialog::slotApply()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QHostInfo info = QHostInfo::fromName(m_ui->addressLineEdit->text());
    if (!info.addresses().isEmpty())
    {
        m_address = info.addresses().first();
        QApplication::restoreOverrideCursor();
        accept();
    }
    else
    {
        setErrorString(info.errorString());
        QApplication::restoreOverrideCursor();
    }
}

const QHostAddress& ConnectionOptionsDialog::address() const
{
    return m_address;
}

void ConnectionOptionsDialog::setAddress(const QHostAddress& addr)
{
    m_address = addr;
    m_ui->addressLineEdit->setText(m_address.toString());
}

quint16 ConnectionOptionsDialog::port() const
{
    return m_ui->portSpinBox->value();
}

void ConnectionOptionsDialog::setPort(quint16 portnum)
{
    m_ui->portSpinBox->setValue(portnum);
}

bool ConnectionOptionsDialog::isDebugMode() const
{
    return m_isDebugMode;
}

void ConnectionOptionsDialog::setDebugMode(bool enabled)
{
    m_isDebugMode = enabled;
}
