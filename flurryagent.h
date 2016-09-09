#ifndef FLURRYAGENT_H
#define FLURRYAGENT_H

#include <QString>
#include <QMap>
#include <QList>
#include <QNetworkAccessManager>
#include <QTimer>

class FlurryAgent : public QObject
{
    const static QString FLURRY_BASE_URL;
    static qint64 CURRENT_EVENT_ID;
    const static int DEFAULT_SENDING_INTERVAL;

private:
    class FlurryEvent
    {
    public:
        FlurryEvent(QString eventName, const QMap<QString, QString>& params, qint64 deltaTime);

        const QString& eventName() const;
        const QMap<QString, QString>& parameters() const;
        qint64 deltaTime() const;
        qint64 id() const;

        qint64 duration() const;
        void setDuration(const qint64 &duration);

        void setParameters(const QMap<QString, QString> &parameters);

    private:
        QString eventName_;
        QMap<QString, QString> parameters_;
        qint64 deltaTime_;
        qint64 duration_;
        qint64 id_;
    };

public:
    FlurryAgent();

    void startSession(QString apiKey);
    void endSession();

    void setUserId(QString userId);
    void setLocation(float latitude, float longitude, float accuracy);

    void logEvent(QString eventName, QMap<QString, QString> parameters = QMap<QString, QString>(), bool timedEvent = false);
    void endTimedEvent(QString eventName, QMap<QString, QString> parameters = QMap<QString, QString>());

    void setRequestInterval(int timeInSeconds);
    void setAppVersion(QString appVersion);

    void logError(QString errorName, QString errorMessage, int lineNumber);

    void setSessionContinueSeconds(int seconds);

    QJsonObject formData();

private slots:
    void sendData();

private:
    void clearData();

    QJsonObject formEventToJson(const FlurryAgent::FlurryEvent& event);

private:
    QString apiKey_;    
    QString userIdHash_;
    QString appVersion_;
    qint64 sessionStartTime_;
    int sendingInterval_;

    QList<FlurryEvent> events_;

    QNetworkAccessManager networkManager_;
    QTimer sendTimer_;
};

#endif // FLURRYAGENT_H
