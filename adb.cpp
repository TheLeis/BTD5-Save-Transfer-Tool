#include <QDebug>
#include <QProcess>

#include "adb.h"

QByteArray ADB::executeADBCommand(const QStringList &arguments) {
    QProcess process;
    process.start("adb", arguments);

    if (!process.waitForFinished(-1)) { // Wait indefinitely until finished
        qWarning() << "Command failed:" << "adb" << arguments;
        return 0;
    }

    QByteArray output = process.readAllStandardOutput();
    return output;
}

QString ADB::executeADBCommandResult(const QStringList &arguments) {
    return QString::fromUtf8(executeADBCommand(arguments));
}

bool ADB::deviceIsRooted() {
    QString output = executeADBCommandResult({"shell", "su -c id"});
    qDebug() << output;
    if (output.contains("uid=0")) {
        return true;
    }
    return false;
}

int ADB::hasConnectedDevices() {
    // Retrns 3 possible int values:
    // -1 if ADB is not installed
    // 0 if ADB is installed, but has no connected devices
    // 1 if ADB is installed and has connected devices
    QString output = executeADBCommandResult({"devices"});
    int pos = output.indexOf('\n');
    if (pos != -1) { // Ignore List of devices attached line
        output = output.mid(pos + 1);
        qDebug() << output;
        if (output.contains("device")) {
            return 1;
        }
        return 0;
    }
    return -1;
}
