#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

#include "flurryagent.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    FlurryAgent fa;
    fa.startSession("IAMGOINGIN");

    QJsonDocument doc(fa.formData());

    qDebug() << doc.toJson(QJsonDocument::Compact);

    return app.exec();
}
