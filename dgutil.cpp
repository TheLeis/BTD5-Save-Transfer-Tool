#include <QJsonDocument>

#include "dgutil.h"

quint32 DgUtil::nk_crc32(const QByteArray& message)
{
    qint64 crc = 0;
    int32_t POLY = 0xdb710641;

    for (int i = 0; i < message.length(); i++) {
        qint64 v = static_cast<unsigned char>(message[i]);
        v ^= crc;
        v &= 0xFF;

        for (int k = 0; k < 8; k++) {
            if ((v & 1) != 0) {
                v ^= static_cast<qint64>(POLY);
            }
            v >>= 1;
        }

        crc >>= 8;
        crc &= 0x00FFFFFF;
        crc ^= v;
    }

    quint64 result = static_cast<quint64>(crc & 0xFFFFFFFFULL);
    return result;
}

QJsonObject DgUtil::nk_decrypt(const QByteArray& data_in)
{
    if (data_in.length() < 14) {
        return QJsonObject();
    }

    QString header = QString::fromLatin1(data_in.left(6));
    qDebug() << "[Decrypt] Header:" << header;

    QString nk_crc = QString::fromLatin1(data_in.mid(6, 8));
    qDebug() << "[Decrypt] CRC from data:" << nk_crc;

    if (header != "DGDATA") {
        qDebug() << "[Decrypt] Invalid header";
        return QJsonObject();
    }

    QByteArray save = data_in.mid(14);
    qDebug() << "[Decrypt] Payload length:" << save.length();

    for (int counter = 0; counter < save.length(); counter++) {
        int v = counter / 6;
        v *= 6;
        v -= (counter & 0xFF);
        v -= 0x15;

        save[counter] = static_cast<char>(static_cast<unsigned char>(save[counter]) + static_cast<signed char>(v));
    }

    QString crc_mine = QString("%1").arg(nk_crc32(save), 8, 16, QChar('0')).toLower();

    QJsonDocument doc = QJsonDocument::fromJson(save);
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "[Decrypt] Failed to parse JSON";
        return QJsonObject();
    }
    return doc.object();
}
