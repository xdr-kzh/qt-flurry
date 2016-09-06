#include "flurryagent.h"

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
{
}

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
