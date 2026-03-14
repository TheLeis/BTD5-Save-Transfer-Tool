#include <QByteArray>
#include <QDebug>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QSaveFile>

#include "adb.h"
#include "dgutil.h"
#include "mainwindow.h"
#include "outlinelabel.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());

    setTextColors();
    connect(ui->mobile_button, &QPushButton::clicked, this, &MainWindow::getMobileSaveFile);
    connect(ui->pc_button, &QPushButton::clicked, this, &MainWindow::getPCSaveFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setTextColors() {
    ui->pc_account->setTextColor(QColor(88, 183, 240));
    ui->pc_account->setOutlineColor(QColor(23, 62, 88));
    ui->mobile_account->setTextColor(QColor(88, 183, 240));
    ui->mobile_account->setOutlineColor(QColor(23, 62, 88));
    ui->pc_rank->setTextColor(QColor(88, 183, 240));
    ui->pc_rank->setOutlineColor(QColor(23, 62, 88));
    ui->mobile_rank->setTextColor(QColor(88, 183, 240));
    ui->mobile_rank->setOutlineColor(QColor(23, 62, 88));
    ui->pc_mm->setTextColor(QColor(94, 214, 0));
    ui->pc_mm->setOutlineColor(QColor(53, 120, 0));
    ui->mobile_mm->setTextColor(QColor(94, 214, 0));
    ui->mobile_mm->setOutlineColor(QColor(53, 120, 0));
}

// UI functions
void MainWindow::setPCText(const QString &rank, const QString &mm)
{
    ui->pc_rank->setText("Rank " + rank);
    ui->pc_mm->setText("$" + mm);
}

void MainWindow::setMobileText(const QString &rank, const QString &mm)
{
    ui->mobile_rank->setText("Rank " + rank);
    ui->mobile_mm->setText("$" + mm);
}

// Save file operations
QPair<QString, QString> MainWindow::getSaveFileData(QByteArray save) {
    int rank = 0;
    int monkeyMoney = 0;

    QJsonObject obj = DgUtil::nk_decrypt(save);
    QJsonObject levels = obj["Levels"].toObject();
    rank = levels["Rank"].toInt();
    QJsonObject items = obj["Items"].toObject();
    monkeyMoney = items["MonkeyMoney"].toInt();

        qDebug() << "Rank:" << rank;
        qDebug() << "Monkey money: $" << monkeyMoney;

    return QPair<QString, QString>(QString::number(rank), QString::number(monkeyMoney));
}

// Mobile operations
bool MainWindow::mobileIsReady() {
    if (ADB::hasConnectedDevices() == -1) {
        QMessageBox::critical(nullptr,
                              "Error",
                              "ADB not found.");
        return false;
    }

    if (ADB::hasConnectedDevices() == 0) {
        QMessageBox::critical(nullptr,
                              "Error",
                              "No devices connected.");
        return false;
    }

    if (ADB::deviceIsRooted() == false) {
        QMessageBox::critical(nullptr,
                              "Error",
                              "ADB does not have root access.\n\n"
                              "Please ensure:\n"
                              "- The device is rooted.\n"
                              "- USB debugging is enabled.\n"
                              "- Root permission is granted to ADB.");
        return false;
    }

    return true;
}

// PC save operations
void MainWindow::getPCSaveFile()
{
    QString defaultPath;
    #ifdef Q_OS_WIN
        defaultPath = "C:/Program Files (x86)/Steam/userdata";
    #elif defined(Q_OS_LINUX)
        defaultPath = QDir::homePath() + "/.steam/steam/userdata";
    #else
        defaultPath = QDir::homePath();
    #endif

    QMessageBox::information(nullptr,
                            "File location info",
                            "Please select your Bloons Tower Defense 5 PC save file.\n"
                            "The save file should be named \"Profile\", and located in your Steam folder:\n"
                            "[Steam ID Number]/306020/local/Data/Docs");
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Load BTD5 PC save file"),
                                                    defaultPath,
                                                    tr("BTD5 Save Files (*.save)"));
    if (!fileName.isEmpty()) {
        QFile File(fileName);
        if (File.open(QIODevice::ReadOnly)) {
            PCSave = File.readAll();
            File.close();
            QPair<QString, QString> PCData = getSaveFileData(PCSave);
            setPCText(PCData.first, PCData.second);
            disconnect(ui->pc_button, &QPushButton::clicked, this, &MainWindow::getPCSaveFile);
            ui->pc_button->setText("PC -> Mobile");
            connect(ui->pc_button, &QPushButton::clicked, this, &MainWindow::PCToMobile);
        }
    }
}

void MainWindow::PCToMobile() {
    if (mobileIsReady()) {
        QSaveFile file("PC_Profile.save");
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << "Failed to open file for writing:" << file.errorString();
            return;
        }
        file.write(PCSave);
        file.commit();
        ADB::executeADBCommand({"push", "PC_Profile.save", "data/local/tmp/"});
        ADB::executeADBCommand({"shell", "su -c mv /data/local/tmp/PC_Profile.save /data_mirror/data_ce/null/0/com.ninjakiwi.bloonstd5/files/Profile.save"});
        QFile::remove("PC_Profile.save");
        QMessageBox::information(nullptr,
                                 "Save data transferred successfully!",
                                 "Your PC save has been transferred to your mobile.");
        this->close();
    }
}

// Mobile save operations
void MainWindow::getMobileSaveFile()
{
    if (mobileIsReady()) {
        mobileSave = ADB::executeADBCommand({"exec-out", "su -c cat /data_mirror/data_ce/null/0/com.ninjakiwi.bloonstd5/files/Profile.save"});
        QPair<QString, QString> mobileData = getSaveFileData(mobileSave);
        setMobileText(mobileData.first, mobileData.second);
        disconnect(ui->mobile_button, &QPushButton::clicked, this, &MainWindow::getMobileSaveFile);
        ui->mobile_button->setText("Mobile -> PC");
        connect(ui->mobile_button, &QPushButton::clicked, this, &MainWindow::mobileToPC);
    }
}

void MainWindow::mobileToPC() {
    QSaveFile file("Profile.save");
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << file.errorString();
        return;
    };
    file.write(mobileSave);
    file.commit();
    QMessageBox::information(nullptr,
                             "Save data transferred successfully!",
                             "Your mobile save, Profile.save, has been exported to this app's folder.\n"
                             "The save file should be moved to:\n"
                             "C:/Program Files (x86)/Steam/userdata/[Steam ID Number]/306020/local/Data/Docs");
    this->close();
}
