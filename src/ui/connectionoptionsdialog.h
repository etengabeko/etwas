#ifndef UI_CONNECTION_OPTIONS_DIALOG_H
#define UI_CONNECTION_OPTIONS_DIALOG_H

#include <QDialog>

class QString;

namespace Ui
{
class ConnectionOptions;
} // Ui

class ConnectionOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionOptionsDialog(QWidget* parent = nullptr);
    ~ConnectionOptionsDialog();

    QString address() const;
    void setAddress(const QString& addr);

    quint16 port() const;
    void setPort(quint16 portnum);

private:
    Ui::ConnectionOptions* m_ui = nullptr;

};

#endif // UI_CONNECTION_OPTIONS_DIALOG_H
