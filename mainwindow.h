#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

    void mobileToPC();
    void PCToMobile();
    QPair<QString, QString> getSaveFileData(QByteArray save);
    void setMobileData(const QByteArray &data);
    void setMobileText(const QString &rank, const QString &mm);
    void setPCData();
    void setPCText(const QString &rank, const QString &mm);

private:
    QByteArray mobileSave;
    QByteArray PCSave;
    Ui::MainWindow *ui;
    uint32_t nk_crc32(const QByteArray& message);
    QJsonObject nk_decrypt(const QByteArray &data_in);
    void getMobileSaveFile();
    void getPCSaveFile();
    bool mobileIsReady();
    void setTextColors();
};

#endif // MAINWINDOW_H
