#ifndef FLURRYAGENT_H
#define FLURRYAGENT_H

#include <QString>
#include <QMap>
#include <QNetworkAccessManager>

class FlurryAgent
{
    const static QString FLURRY_BASE_URL;
    static qint64 CURRENT_EVENT_ID;
public:
    FlurryAgent();

    void startSession(QString apiKey);
    void endSession();
    void pauseSession();

    void setUserId(QString userId);
    void setLocation(float latitude, float longitude, float accuracy);

    void logEvent(QString eventName, QMap<QString, QString> parameters = QMap<QString, QString>(), bool timedEvent = false);
    void endTimedEvent(QString eventName, QMap<QString, QString> parameters = QMap<QString, QString>());

    void setRequestInterval(int timeInSeconds);
    void setAppVersion(QString appVersion);

    void logError(QString errorName, QString errorMessage, int lineNumber);

    void setSessionContinueSeconds(int seconds);

private:
    void sendData(QString postData);
    void formData();
    QJsonObject formEvent(QString eventName, const QMap<QString, QString>& parameters);

private:
    QString apiKey_;
    QNetworkAccessManager networkManager_;
};

#endif // FLURRYAGENT_H
