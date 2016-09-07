#include "flurryagent.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonArray>

#include "utils.h"

const QString FlurryAgent::FLURRY_BASE_URL = QString::fromUtf8("https://data.flurry.com/aah.do");

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
    if(parameters.isEmpty())
    {
        ;
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

void FlurryAgent::sendData(const QString& postData)
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
    urlQuery.addQueryItem("c", utils::adler32(postData.toStdString()));

    sendDataRequest.setUrl(urlQuery);

    networkManager_.get(sendDataRequest);
}

void FlurryAgent::formData()
{
//    data_stream << "{\"a\":{\"af\":" << time3
//                <<",\"aa\":1,\"ab\":10,\"ac\":9,\"ae\":\""<< version
//                << "\",\"ad\":\"" << flurry_key
//                << "\",\"ag\":" << time
//                << ",\"ah\":" << time1
//                << ",\"ak\":1,"
//                << "\"cg\":\"" << user_key
//    << "\"},\"b\":[{\"bd\":\"\",\"be\":\"\",\"bk\":-1,\"bl\":0,\"bj\":\"ru\",\"bo\":[";

    QJsonObject flurryBaseData;
    flurryBaseData.insert("af", time3);
    flurryBaseData.insert("aa", 1);
    flurryBaseData.insert("ad", apiKey_);
    flurryBaseData.insert("ag", time);
    flurryBaseData.insert("ah", time1);
    flurryBaseData.insert("ak", 1);
    flurryBaseData.insert("cg", user_key);

    QJsonArray bData;
    QJsonObject partData;
    partData.insert("bd", "");
    partData.insert("be", "");
    partData.insert("bk", -1);
    partData.insert("bj", "ru");

    QJsonArray events;
//    data_stream << events_to_json(begin, end, time);
//  TODO something like FOREACH
    events.append(formEvent());
    partData.insert("bo", events);

//    data_stream << "}"
//        <<",\"bv\":[],\"bt\":false,\"bu\":{},\"by\":[],\"cd\":0,"
//        << "\"ba\":" << time1
//        << ",\"bb\":" << delta
//        << ",\"bc\":-1,\"ch\":\"Etc/GMT-3\"}]}";
    QJsonObject finalLine;
    finalLine.insert("bv", QJsonArray());
    finalLine.insert("bt", false);
    finalLine.insert("bu", QJsonObject());
    finalLine.insert("by", QJsonArray());
    finalLine.insert("ba", time1);
    finalLine.insert("bb", delta);
    finalLine.insert("bc", -1);
    finalLine.insert("ch", "Etc/GMT-3");

    QJsonObject data;
    data.insert("a", flurryBaseData);
//    data.insert("b", );
}

QJsonObject FlurryAgent::formEvent()
{
    QJsonObject event;
    event.insert("ce", eventId);
    event.insert("bp", name);
    event.insert("bq", milliseconds);
    event.insert("bs", milliseconds);
    event.insert("br", br);

//    result << "{\"ce\":" << event_id_
//            << ",\"bp\":\"" << name_
//            << "\",\"bq\":" << std::chrono::system_clock::to_time_t(event_time_) * 1000 - _start_time// milliseconds
//            << ",\"bs\":{" << params_in_json.str() <<"},"
//    << "\"br\":" << br << "}";
    return event;
}
