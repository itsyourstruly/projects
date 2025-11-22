#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStorageInfo> // pull drives
#include <QList> // store drives

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // slots to connect to our buttons
    void on_refresh_clicked();
    void on_openDefault_clicked();
    void on_eject_clicked();


private:
    Ui::MainWindow *ui;

    // list to hold info for the drives
    QList<QStorageInfo> m_drives;
};
#endif // MAINWINDOW_H
