#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <locale>

#include "flurryagent.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    FlurryAgent fa;
    fa.setUserId("ratamahata@gmail.com");
    fa.startSession("YOUR_API_KEY");

    std::locale loc("");
    QMap<QString, QString> props;
    props.insert(QString::fromStdString("Sys_Language"), QString::fromStdString(loc.name()));
    fa.logEvent("StartSession", props);
    fa.endSession();

    return app.exec();
}
