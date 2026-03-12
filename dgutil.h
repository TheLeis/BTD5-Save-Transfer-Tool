#ifndef DGUTIL_H
#define DGUTIL_H

#include <QByteArray>
#include <QJsonObject>
#include <QString>

class DgUtil
{
public:
    static quint32 nk_crc32(const QByteArray& message);
    static QJsonObject nk_decrypt(const QByteArray& data_in);
};

#endif // DGUTIL_H
