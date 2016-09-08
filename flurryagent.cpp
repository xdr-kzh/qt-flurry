#include "flurryagent.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonArray>

#include "utils.h"

const QString FlurryAgent::FLURRY_BASE_URL = QString::fromUtf8("https://data.flurry.com/aah.do");
qint64 FlurryAgent::CURRENT_EVENT_ID = 0;

enum InfoTypes
{
    //0,2 and 3 reserved
    EVENT_NAME = 1,
    EVENT_PROPS = 4,
    EVENT_PROP_NAME = 5,
    EVENT_PROP_VALUE = 6,
    LAST_SENT_TIME = 7,
    EVENT_TIME = 8,
    EVENT_ID = 9,
};

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
{}

void FlurryAgent::startSession(QString apiKey)
{
    apiKey_ = apiKey;
}

void FlurryAgent::endSession()
{}

void FlurryAgent::pauseSession()
{}

void FlurryAgent::setUserId(QString userId)
{}

void FlurryAgent::setLocation(float latitude, float longitude, float accuracy)
{}

void FlurryAgent::logEvent(QString eventName, QMap<QString, QString> parameters, bool timedEvent)
{
    CURRENT_EVENT_ID++;
    if(!parameters.isEmpty())
    {

        //TODO make an explicit parameter
        auto startTime = QDateTime::currentMSecsSinceEpoch();
        QJsonObject eventJson = formEvent(eventName, parameters, startTime);
    }
}

void FlurryAgent::endTimedEvent(QString eventName, QMap<QString, QString> parameters)
{
    if(parameters.isEmpty())
    {
        ;
    }
}

void FlurryAgent::setRequestInterval(int timeInSeconds)
{}

void FlurryAgent::setAppVersion(QString appVersion)
{}

void FlurryAgent::logError(QString errorName, QString errorMessage, int lineNumber)
{}

void FlurryAgent::setSessionContinueSeconds(int seconds)
{}

void FlurryAgent::sendData(QString postData)
{
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

    networkManager_.get(sendDataRequest);
}

QJsonObject FlurryAgent::formData()
{
    //    auto time = std::chrono::system_clock::to_time_t(begin->get_time()) * 1000; // milliseconds;
    //    auto time1 = time + 4;
    //    auto time2 = time + 6;
    //    auto time3 = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) * 1000;
    //    auto delta = time3 - time;
    //    auto bq = 11;

    //    auto version = core::utils::get_user_agent();

    //TODO it is time of the event creation
    auto time = QDateTime::currentMSecsSinceEpoch(); // milliseconds;
    auto time1 = time + 4;
    auto time3 = QDateTime::currentMSecsSinceEpoch();
    auto delta = time3 - time;

    QJsonObject flurryBaseData;
    flurryBaseData.insert("af", time3);
    flurryBaseData.insert("aa", 1);
    flurryBaseData.insert("ad", apiKey_);
    flurryBaseData.insert("ag", time);
    flurryBaseData.insert("ah", time1);
    flurryBaseData.insert("ak", 1);
    //TODO generate some unique user key
    flurryBaseData.insert("cg", "user_key");

    QJsonArray eventsData;
    QJsonObject eventsDataObject;
    eventsDataObject.insert("bd", "");
    eventsDataObject.insert("be", "");
    eventsDataObject.insert("bk", -1);
    eventsDataObject.insert("bj", "ru");

    QJsonArray events;
//    data_stream << events_to_json(begin, end, time);
//  TODO something like FOREACH
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

QJsonObject FlurryAgent::formEvent(QString eventName, const QMap<QString, QString>& parameters, qint64 startTime)
{
    QJsonObject eventParameters;
    foreach(QString key, parameters.keys()){
        eventParameters.insert(key, parameters.value(key));
    }

    QJsonObject event;
    event.insert("ce", CURRENT_EVENT_ID);
    event.insert("bp", eventName);
    //TODO event creation milliseonds timestamp
    event.insert("bq", QDateTime::currentMSecsSinceEpoch() - startTime);
    event.insert("bs", eventParameters);
    event.insert("br", 0);

//    result << "{\"ce\":" << event_id_
//            << ",\"bp\":\"" << name_
//            << "\",\"bq\":" << std::chrono::system_clock::to_time_t(event_time_) * 1000 - _start_time// milliseconds
//            << ",\"bs\":{" << params_in_json.str() <<"},"
//    << "\"br\":" << br << "}";
    return event;
}
