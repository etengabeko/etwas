#ifndef UI_CURRENT_TIME_DIALOG_H
#define UI_CURRENT_TIME_DIALOG_H

#include <QDialog>

namespace Ui
{
class CurrentTime;
} // Ui

class CurrentTimeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CurrentTimeDialog(QWidget* parent = nullptr);
    ~CurrentTimeDialog();

    quint32 currentTimestamp() const;
    void setCurrentTimestamp(quint32 epoch);

private slots:
    void slotUpdateTimestamp();

private:
    Ui::CurrentTime* m_ui = nullptr;

};

#endif // UI_CURRENT_TIME_DIALOG_H
