#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <unistd.h>

#include "flurryagent.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    FlurryAgent fa;
    fa.setUserId("ratamahata@gmail.com");
    fa.setAppVersion(QString::number(1));
    fa.startSession("YOUR_API_KEY");

    QMap<QString, QString> props;
    props.insert(QString::fromStdString("os"), QString::fromStdString("iOS"));
    props.insert(QString::fromStdString("browser"), QString::fromStdString("Safari"));
    fa.logEvent("environment", props);
    sleep(3);
    fa.endSession();

    return app.exec();
}
