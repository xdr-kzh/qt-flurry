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
    fa.setUserId("porracaralho");
    fa.setAppVersion(QString::number(2));
    fa.startSession("YOUR_API_KEY");

    QMap<QString, QString> props;
    props.insert(QString::fromStdString("os"), QString::fromStdString("MacOSX"));
    props.insert(QString::fromStdString("browser"), QString::fromStdString("Chromium"));
    fa.logEvent("environment", props);
    sleep(1);

    props.clear();
    props.insert(QString::fromStdString("ram"), QString::fromStdString("16"));
    props.insert(QString::fromStdString("cores"), QString::fromStdString("4"));
    fa.logEvent("system", props);
    sleep(1);
    fa.logEvent("system", props);

    fa.endSession();

    return app.exec();
}
