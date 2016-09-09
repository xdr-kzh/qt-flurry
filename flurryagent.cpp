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
#ifdef DEBUG
    const uint FlurryAgent::DEFAULT_SENDING_INTERVAL = 20 * 1; // 20 seconds for debug
#else
    const uint FlurryAgent::DEFAULT_SENDING_INTERVAL = 60 * 60; // 1 hour for release
#endif // DEBUG

//age: "age",
//timestamp: "ba",
//duration: "bb",
//u: "bc",
//appVersion: "bd",
//Sa: "be",
//location: "bf",
//vb: "bj",
//Ga: "bk",
//ya: "bl",
//Aa: "bm",
//s: "bn",
//D: "bo",
//Ba: "bt",
//v: "bu",
//X: "bv",
//S: "by",
//Ia: "cd",
//Lb: "ch"
//eventLogging: "eventLogging",
//eventCounter: "eventCounter",
//errorCounter: "errorCounter"
//numEventNames: "numEventNames",
//numPurchaseNames: "numPurchaseNames",
//numEventsLogged: "numEventsLogged",
//numPurchasesLogged: "numPurchasesLogged",
//numErrorsLogged: "numErrorsLogged",
//timedEvents: "timedEvents",
//totalEventNames: "totalEventNames",
//totalPurchaseNames: "totalPurchaseNames",
//purchaseCounter: "purchaseCounter",
//sessionContinue: "sessionContinue",
//pauseTimestamp: "pauseTimestamp",
//requestsMade: "requestsMade",

FlurryAgent::FlurryAgent()
{
    appVersion_ = QCoreApplication::instance()->applicationVersion();
    sendingInterval_ = DEFAULT_SENDING_INTERVAL;
}

void FlurryAgent::startSession(QString apiKey)
{
    apiKey_ = apiKey;
    sessionStartTime_ = QDateTime::currentMSecsSinceEpoch();
    connect(&sendTimer_, SIGNAL(timeout()), this, SLOT(sendData()));
    sendTimer_.start(sendingInterval_ * 1000);
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

void FlurryAgent::setLocation(float latitude, float longitude, float accuracy)
{}

void FlurryAgent::logEvent(QString eventName, QMap<QString, QString> parameters, bool timedEvent)
{
    if(!timedEvent)
    {
        CURRENT_EVENT_ID++;
        FlurryEvent event(eventName, parameters, QDateTime::currentMSecsSinceEpoch());
        events_.push_back(event);
    }
}

void FlurryAgent::endTimedEvent(QString eventName, QMap<QString, QString> parameters)
{}

void FlurryAgent::setRequestInterval(int timeInSeconds)
{
    sendingInterval_ = timeInSeconds;
}

void FlurryAgent::setAppVersion(QString appVersion)
{
    appVersion_ = appVersion;
}

void FlurryAgent::logError(QString errorName, QString errorMessage, int lineNumber)
{}

void FlurryAgent::setSessionContinueSeconds(int seconds)
{}

void FlurryAgent::sendData()
{
    QJsonDocument doc(formData());
    QString postData = doc.toJson(QJsonDocument::Compact);

    QNetworkRequest sendDataRequest;
//    core::http_request_simple post_request(_user_proxy, utils::get_user_agent(), stop_handler);
//    post_request.set_connect_timeout(1000);
//    post_request.set_timeout(1000);
//    post_request.set_keep_alive();

    QUrlQuery urlQuery(FLURRY_BASE_URL);
    QByteArray base64Data;
    base64Data.append(postData);
    urlQuery.addQueryItem("d", base64Data.toBase64());
    urlQuery.addQueryItem("c", QString::fromStdString(utils::adler32(postData.toStdString())));

    sendDataRequest.setUrl(urlQuery.query());

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
}

QJsonObject FlurryAgent::formData()
{
    //    auto version = core::utils::get_user_agent();

    auto time1 = sessionStartTime_ + 4;
    auto time3 = QDateTime::currentMSecsSinceEpoch();
    auto delta = time3 - sessionStartTime_;

    QJsonObject flurryBaseData;
    flurryBaseData.insert("af", time3);
    flurryBaseData.insert("aa", 1);
    flurryBaseData.insert("ad", apiKey_);
    flurryBaseData.insert("ag", sessionStartTime_);
    flurryBaseData.insert("ah", time1);
    flurryBaseData.insert("ak", 1);
    flurryBaseData.insert("cg", userIdHash_);

    QJsonArray eventsData;
    QJsonObject eventsDataObject;
    //TODO It is an app version?
    eventsDataObject.insert("bd", appVersion_);
    eventsDataObject.insert("be", "");
    eventsDataObject.insert("bk", -1);
    eventsDataObject.insert("bj", "ru");

    QJsonArray events;
    foreach (FlurryEvent event, events_) {
        events.append(formEvent(event, sessionStartTime_));
    }
    eventsDataObject.insert("bo", events);

    eventsDataObject.insert("bv", QJsonArray());
    eventsDataObject.insert("bt", false);
    eventsDataObject.insert("bu", QJsonObject());
    eventsDataObject.insert("by", QJsonArray());
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

QJsonObject FlurryAgent::formEvent(const FlurryEvent& event, qint64 sessionStartTime)
{
    QJsonObject eventParameters;
    foreach(QString key, event.parameters().keys()){
        eventParameters.insert(key, event.parameters().value(key));
    }

    QJsonObject jsonEvent;
    jsonEvent.insert("ce", CURRENT_EVENT_ID);
    jsonEvent.insert("bp", event.eventName());
    jsonEvent.insert("bq", event.startTime() - sessionStartTime);
    jsonEvent.insert("bs", eventParameters);
    jsonEvent.insert("br", 0);

    return jsonEvent;
}

FlurryAgent::FlurryEvent::FlurryEvent(QString eventName, const QMap<QString, QString>& params, qint64 startTime):
    eventName_(eventName), parameters_(params), startTime_(startTime), id_(CURRENT_EVENT_ID)
{}

const QString& FlurryAgent::FlurryEvent::eventName() const
{
    return eventName_;
}

const QMap<QString, QString>& FlurryAgent::FlurryEvent::parameters() const
{
    return parameters_;
}

qint64 FlurryAgent::FlurryEvent::startTime() const
{
    return startTime_;
}

qint64 FlurryAgent::FlurryEvent::id() const
{
    return id_;
}
