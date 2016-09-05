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

//appVersion: "bd",
//Sa: "be",
//Ga: "bk",
//xa: "age",
//ya: "bl",
//location: "bf",
//vb: "bj",
//k: "timedEvents",
//C: "eventCounter",
//D: "bo",
//V: "numEventsLogged",
//Aa: "bm",
//T: "totalEventNames",
//s: "bn",
//ma: "numEventNames",
//H: "purchaseCounter",
//X: "bv",
//W: "numPurchasesLogged",
//Ba: "bt",
//oa: "totalPurchaseNames",
//v: "bu",
//na: "numPurchaseNames",
//B: "errorCounter",
//S: "by",
//la: "numErrorsLogged",
//Ia: "cd",
//timestamp: "ba",
//duration: "bb",
//ja: "eventLogging",
//pa: "sessionContinue",
//r: "pauseTimestamp",
//u: "bc",
//La: "requestsMade",
//Lb: "ch"
//timedEvents: "timedEvents",
//eventLogging: "eventLogging",
//sessionContinue: "sessionContinue",
//pauseTimestamp: "pauseTimestamp",
//age: "age",
//numEventNames: "numEventNames",
//numPurchaseNames: "numPurchaseNames",
//requestsMade: "requestsMade",
//totalEventNames: "totalEventNames",
//totalPurchaseNames: "totalPurchaseNames",
//numEventsLogged: "numEventsLogged",
//numPurchasesLogged: "numPurchasesLogged",
//numErrorsLogged: "numErrorsLogged",
//eventCounter: "eventCounter",
//purchaseCounter: "purchaseCounter",
//errorCounter: "errorCounter"

FlurryAgent::FlurryAgent()
{
}

void FlurryAgent::init()
{
    load();
    startSave();
    delayedStartSend();
}
