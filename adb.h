#ifndef ADB_H
#define ADB_H

#include <QByteArray>
#include <QStringList>

class ADB
{
public:
    static QByteArray executeADBCommand(const QStringList &arguments);
    static QString executeADBCommandResult(const QStringList &arguments);
    static bool deviceIsRooted();
    static bool hasConnectedDevices();
};

#endif
