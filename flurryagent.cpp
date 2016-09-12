#include "flurryagent.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>

#include "utils.h"

const QString FlurryAgent::FLURRY_BASE_URL = QString::fromUtf8("https://data.flurry.com/aah.do");
qint64 FlurryAgent::CURRENT_EVENT_ID = 0;
qint64 FlurryAgent::CURRENT_ERROR_ID = 0;
#ifdef DEBUG
    const int FlurryAgent::DEFAULT_SENDING_INTERVAL = 20 * 1 * 1000; // 20 seconds for debug
#else
    const int FlurryAgent::DEFAULT_SENDING_INTERVAL = 60 * 60 * 1000; // 1 hour for release
#endif // DEBUG

FlurryAgent::FlurryAgent() : latitude_(0.0), longitude_(0.0), locationAccuracy_(0.0)
{
    appVersion_ = QCoreApplication::instance()->applicationVersion();
    sendingInterval_ = DEFAULT_SENDING_INTERVAL;
}

void FlurryAgent::startSession(QString apiKey)
{
    apiKey_ = apiKey;
    sessionStartTime_ = QDateTime::currentMSecsSinceEpoch();
    connect(&sendTimer_, &QTimer::timeout, this, &FlurryAgent::sendData);
    sendTimer_.start(sendingInterval_);
}

void FlurryAgent::endSession()
{
    sendTimer_.stop();
    sendData();
    sessionStartTime_ = 0;
}

void FlurryAgent::setUserId(QString userId)
{
    userIdHash_ = userId;
}

void FlurryAgent::setLocation(double latitude, double longitude, float accuracy)
{
    latitude_ = latitude;
    longitude_ = longitude;
    locationAccuracy_ = accuracy;
}

void FlurryAgent::logEvent(QString eventName, QMap<QString, QString> parameters, bool timedEvent)
{
    ++CURRENT_EVENT_ID;
    FlurryEvent event(eventName, parameters, QDateTime::currentMSecsSinceEpoch() - sessionStartTime_, timedEvent);
    events_.push_back(event);
}

void FlurryAgent::endTimedEvent(QString eventName, QMap<QString, QString> parameters)
{
    qint64 endTime = QDateTime::currentMSecsSinceEpoch() - sessionStartTime_;
    foreach(FlurryEvent event, events_)
    {
        if(event.eventName() == eventName)
        {
            if(!parameters.isEmpty())
            {
                event.setParameters(parameters);
            }
            event.setDuration(endTime - event.deltaTime());
            break;
        }
    }
}

void FlurryAgent::setRequestInterval(int timeInSeconds)
{
    sendingInterval_ = timeInSeconds;
}

void FlurryAgent::setAppVersion(QString appVersion)
{
    appVersion_ = appVersion;
}

void FlurryAgent::logError(QString errorName, QString errorMessage, int lineNumber)
{
    ++CURRENT_ERROR_ID;
    ErrorEvent event(errorName, errorMessage, lineNumber, QDateTime::currentMSecsSinceEpoch());
    errorEvents_.push_back(event);
}

void FlurryAgent::sendData()
{
    QJsonDocument doc(formData());
    QString postData = doc.toJson(QJsonDocument::Compact);

    QNetworkRequest sendDataRequest;

    QUrlQuery urlQuery(FLURRY_BASE_URL);
    QByteArray base64Data;
    base64Data.append(postData);
    urlQuery.addQueryItem("d", base64Data.toBase64());
    urlQuery.addQueryItem("c", QString::fromStdString(utils::adler32(postData.toStdString())));

    sendDataRequest.setUrl(urlQuery.query());

    qDebug() << "[FlurryAgent] " << "Query: " << urlQuery.query();

    qDebug() << "[FlurryAgent] " << "Data: " << postData;

    QNetworkReply* postDataReply = networkManager_.get(sendDataRequest);
    postDataReply->connect(postDataReply, &QNetworkReply::finished, [this, postDataReply]
    {
        clearData();
        qDebug() << "[FlurryAgent] " << "data has been sent successfully";
        postDataReply->deleteLater();
    });
}

void FlurryAgent::clearData()
{
    events_.clear();
    errorEvents_.clear();
}

QJsonObject FlurryAgent::formData()
{
    auto time1 = sessionStartTime_ + 4;
    auto time3 = QDateTime::currentMSecsSinceEpoch();
    auto delta = time3 - sessionStartTime_;

    QJsonObject flurryBaseData;
    flurryBaseData.insert("af", time3);
    flurryBaseData.insert("aa", 1);
    flurryBaseData.insert("ab", 10);
    flurryBaseData.insert("ac", 9);
    flurryBaseData.insert("ae", appVersion_);
    flurryBaseData.insert("ad", apiKey_);
    flurryBaseData.insert("ag", sessionStartTime_);
    flurryBaseData.insert("ah", time1);
    flurryBaseData.insert("cg", userIdHash_);
    flurryBaseData.insert("ak", 1);

    QJsonArray eventsData;
    QJsonObject eventsDataObject;
    eventsDataObject.insert("bd", "");
    eventsDataObject.insert("be", "");
    eventsDataObject.insert("bk", -1);
    eventsDataObject.insert("bl", 0);
    if(longitude_ != 0.0 && latitude_ != 0.0)
    {
        QJsonObject locationData;
        locationData.insert("bg", latitude_);
        locationData.insert("bh", longitude_);
        locationData.insert("bi", locationAccuracy_);
        eventsDataObject.insert("bf", locationData);
    }
    eventsDataObject.insert("bj", "ru");

    QJsonArray events;
    QMap<QString, int> eventsAndCounts;
    foreach (FlurryEvent event, events_)
    {
        if(event.isReadyToSend())
        {
            events.push_back(formEventToJson(event));
            ++eventsAndCounts[event.eventName()];
        }
    }
    eventsDataObject.insert("bo", events);

    eventsDataObject.insert("bm", false);

    QJsonObject uniqueEventCount;
    foreach (QString key, eventsAndCounts.keys()) {
        uniqueEventCount.insert(key, eventsAndCounts[key]);
    }
    eventsDataObject.insert("bn", uniqueEventCount);

    eventsDataObject.insert("bv", QJsonArray());
    eventsDataObject.insert("bt", false);
    eventsDataObject.insert("bu", QJsonObject());

    QJsonArray errorDataArray;
    foreach(ErrorEvent error, errorEvents_)
    {
        errorDataArray.push_back(formErrorToJson(error));
    }

    eventsDataObject.insert("by", errorDataArray);
    eventsDataObject.insert("cd", 0);
    eventsDataObject.insert("ba", time1);
    eventsDataObject.insert("bb", delta);
    eventsDataObject.insert("bc", -1);
    eventsDataObject.insert("ch", "Etc/GMT-3");

    eventsData.append(eventsDataObject);

    QJsonObject data;
    data.insert("a", flurryBaseData);
    data.insert("b", eventsData);
    return data;
}

QJsonObject FlurryAgent::formEventToJson(const FlurryEvent& event)
{
    QJsonObject eventParameters;
    foreach(QString key, event.parameters().keys()){
        eventParameters.insert(key, event.parameters().value(key));
    }

    QJsonObject jsonEvent;
    jsonEvent.insert("ce", CURRENT_EVENT_ID);
    jsonEvent.insert("bp", event.eventName());
    jsonEvent.insert("bq", event.deltaTime());
    jsonEvent.insert("bs", eventParameters);
    jsonEvent.insert("br", event.duration());

    return jsonEvent;
}

QJsonObject FlurryAgent::formErrorToJson(const ErrorEvent& error)
{
    QJsonObject jsonEvent;
    jsonEvent.insert("cf", CURRENT_ERROR_ID);
    jsonEvent.insert("bz", error.errorName());
    jsonEvent.insert("ca", error.errorDesc());
    jsonEvent.insert("cb", QString::number(error.lineNumber()));
    jsonEvent.insert("cc", error.timestamp());

    return jsonEvent;
}

FlurryAgent::FlurryEvent::FlurryEvent(QString eventName, const QMap<QString, QString>& params, qint64 deltaTime, bool isTimed):
    eventName_(eventName), parameters_(params), deltaTime_(deltaTime), duration_(0), id_(CURRENT_EVENT_ID),
    isTimed_(isTimed), isReadyToSend_(!isTimed)
{}

QString FlurryAgent::FlurryEvent::eventName() const
{
    return eventName_;
}

const QMap<QString, QString>& FlurryAgent::FlurryEvent::parameters() const
{
    return parameters_;
}

qint64 FlurryAgent::FlurryEvent::deltaTime() const
{
    return deltaTime_;
}

qint64 FlurryAgent::FlurryEvent::id() const
{
    return id_;
}

qint64 FlurryAgent::FlurryEvent::duration() const
{
    return duration_;
}

void FlurryAgent::FlurryEvent::setDuration(const qint64 &duration)
{
    if(isTimed_)
    {
        duration_ = duration;
        isReadyToSend_ = true;
    }
}

void FlurryAgent::FlurryEvent::setParameters(const QMap<QString, QString> &parameters)
{
    parameters_ = parameters;
}

bool FlurryAgent::FlurryEvent::isReadyToSend() const
{
    return isReadyToSend_;
}

FlurryAgent::ErrorEvent::ErrorEvent(QString errorName, QString errorDesc, int lineNumber, qint64 timestamp):
    errorName_(errorName), errorDesc_(errorDesc), lineNumber_(lineNumber),
    timestamp_(timestamp), id_(CURRENT_ERROR_ID)
{}

QString FlurryAgent::ErrorEvent::errorName() const
{
    return errorName_;
}

QString FlurryAgent::ErrorEvent::errorDesc() const
{
    return errorDesc_;
}

qint64 FlurryAgent::ErrorEvent::timestamp() const
{
    return timestamp_;
}

qint64 FlurryAgent::ErrorEvent::id() const
{
    return id_;
}

int FlurryAgent::ErrorEvent::lineNumber() const
{
    return lineNumber_;
}

