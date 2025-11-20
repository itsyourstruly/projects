#include "mainwindow.h"

#include <QApplication>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // create application object

    MainWindow w; // create instance of main window

    w.show(); // show the window

    return a.exec(); // start the event loop
}
