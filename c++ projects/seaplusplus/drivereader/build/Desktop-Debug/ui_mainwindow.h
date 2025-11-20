/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QListWidget *driveList;
    QPushButton *eject;
    QLabel *label;
    QPushButton *openDefault;
    QPushButton *openInApp;
    QPushButton *refresh;
    QMenuBar *menubar;
    QMenu *menudrive_reader;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        driveList = new QListWidget(centralwidget);
        driveList->setObjectName("driveList");
        driveList->setGeometry(QRect(220, 80, 301, 131));
        eject = new QPushButton(centralwidget);
        eject->setObjectName("eject");
        eject->setGeometry(QRect(440, 370, 181, 61));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(130, 350, 85, 24));
        openDefault = new QPushButton(centralwidget);
        openDefault->setObjectName("openDefault");
        openDefault->setGeometry(QRect(50, 390, 116, 32));
        openInApp = new QPushButton(centralwidget);
        openInApp->setObjectName("openInApp");
        openInApp->setGeometry(QRect(190, 390, 116, 32));
        refresh = new QPushButton(centralwidget);
        refresh->setObjectName("refresh");
        refresh->setGeometry(QRect(320, 240, 116, 32));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 24));
        menudrive_reader = new QMenu(menubar);
        menudrive_reader->setObjectName("menudrive_reader");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menudrive_reader->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        eject->setText(QCoreApplication::translate("MainWindow", "eject disk", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "open via", nullptr));
        openDefault->setText(QCoreApplication::translate("MainWindow", "default", nullptr));
        openInApp->setText(QCoreApplication::translate("MainWindow", "in app", nullptr));
        refresh->setText(QCoreApplication::translate("MainWindow", "refresh", nullptr));
        menudrive_reader->setTitle(QCoreApplication::translate("MainWindow", "drive reader", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
