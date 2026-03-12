#include <QApplication>
#include <QFontDatabase>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>

#include "mainwindow.h"
#include "ui_mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int id = QFontDatabase::addApplicationFont(":/fonts/oetztyp.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QApplication::setFont(QFont(family, 12));
    MainWindow w;
    w.show();
    /* w.setMobileData(mobileSave); */
    return a.exec();
}
