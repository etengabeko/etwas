#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QMainWindow>

class QMdiArea;
class QString;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void slotNewConnection();
    void slotNewControl();

    void slotOnError(const QString& message);

private:
    void initMenu();

private:
    QMdiArea* m_central = nullptr;

};

#endif // UI_MAINWINDOW_H
