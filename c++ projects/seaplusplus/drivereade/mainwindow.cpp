#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QProcess>
#include <QStorageInfo>
#include <QDebug>

const char search[] = "Checking if you have a removable drive.";
const char notfound[] = "No removable drive found.";
const char found[] = "Removable drive found!";
const char prompt[] = "What would you like to do?";
const char open[] = "Open the drive";
const char eject[] = "Eject the drive";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow constructor called. Application is starting!";
    ui->setupUi(this);

    // refresh the list as soon as the app starts
    on_refresh_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_refresh_clicked()
{
    // First, clear the list of any old entries
    ui->driveList->clear();
    m_drives.clear();

    // Loop through every volume connected to the system
    for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()) {

        // --- WORKAROUND FOR OLDER QT VERSIONS ---
        // Instead of isRemovable(), we check for other clues.
        // On Linux, removable drives are often mounted under "/media"
        // or have a "vfat" (FAT32) filesystem.
        if (storage.rootPath().startsWith("/media") || storage.fileSystemType() == "vfat") {

            // If it matches our checks, add it to the lists
            m_drives.append(storage);

            QString displayName = QString("%1 (%2)")
                                      .arg(storage.name(), storage.rootPath());

            ui->driveList->addItem(displayName);
        }
    }
}

//open the drive in file manager
void MainWindow::on_openDefault_clicked()
{
    int index = ui->driveList->currentRow();

    // checks if item is selected
    if (index == -1)
    {
        return;
    }

    //get the path
    QString path = m_drives[index].rootPath();

    //cross platform function to open folder
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::on_eject_clicked()
{
    int index = ui->driveList->currentRow();
    if (index == -1) {
        return; // No drive selected
    }

    // This is the device path, e.g., "/dev/sdb1"
    QString devicePath = m_drives[index].device();
    qDebug() << "Attempting to eject device:" << devicePath; // Good for debugging

    QProcess process;

    // --- CORRECTED COMMAND FOR LINUX ---
    // We run "pkexec" and pass our command to it as arguments.
    process.start("pkexec", {"udisksctl", "eject", "-b", devicePath});

    // Wait for the command to finish (e.g., for 10 seconds)
    process.waitForFinished(10000);

    // --- CHECK FOR ERRORS ---
    // Read any output from the command's error stream.
    QByteArray errorOutput = process.readAllStandardError();
    if (!errorOutput.isEmpty()) {
        qDebug() << "Eject command failed with error:" << errorOutput;
    } else {
        qDebug() << "Eject command sent successfully.";
    }

    // Refresh the drive list to see if it disappeared
    on_refresh_clicked();
}

