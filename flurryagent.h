#ifndef FLURRYAGENT_H
#define FLURRYAGENT_H

#include <QString>
#include <QMap>
#include <QList>
#include <QNetworkAccessManager>

class FlurryAgent
{
    const static QString FLURRY_BASE_URL;
    static quint64 CURRENT_EVENT_ID;

private:
    class FlurryEvent
    {
    public:
        FlurryEvent(QString eventName, const QMap<QString, QString>& params, qint64 startTime);

        const QString& eventName();

        const QMap<QString, QString>& parameters();

        quint64 startTime() const;

        quint64 id() const;
    private:
        QString eventName_;
        QMap<QString, QString> parameters_;
        quint64 startTime_;
        quint64 id_;
    };

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

    QJsonObject formData();

private:
    void sendData(QString postData);

    QJsonObject formEvent(const FlurryAgent::FlurryEvent& event);
    QJsonObject formEvent(QString eventName, const QMap<QString, QString>& parameters, qint64 startTime);

private:
    QString apiKey_;
    QString appVersion_;
    QNetworkAccessManager networkManager_;
    QList<FlurryEvent> events_;
};

#endif // FLURRYAGENT_H
