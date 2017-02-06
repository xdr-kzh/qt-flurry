#include "flurryagent.h"

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QLocale>

#include "utils.h"
#include "qreplytimeout.h"

const QString FlurryAgent::FLURRY_BASE_URL = QString::fromUtf8("https://data.flurry.com/aah.do");
qint64 FlurryAgent::CURRENT_EVENT_ID = 0;
qint64 FlurryAgent::CURRENT_ERROR_ID = 0;
#ifdef DEBUG
    const int FlurryAgent::DEFAULT_SENDING_INTERVAL = 20 * 1 * 1000; // 20 seconds for debug
#else
    const int FlurryAgent::DEFAULT_SENDING_INTERVAL = 60 * 60 * 1000; // 1 hour for release
#endif // DEBUG

FlurryAgent::FlurryAgent() : latitude_(0.0), longitude_(0.0),
    locationAccuracy_(0.0), isSending_(false)
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
    if(events_.size() <= 0 && errorEvents_.size() <= 0)
    {
        isSending_ = false;
        return;
    }
    else
    {
        isSending_  = true;
    }

    const QString& postData = formData();

    QNetworkRequest sendDataRequest;

    QByteArray base64Data;
    base64Data.append(postData);

    QString urlQueryData = FLURRY_BASE_URL + "?d=" + base64Data.toBase64() +
            "&c=" + QString::fromStdString(utils::adler32(postData.toStdString()));

    sendDataRequest.setUrl(urlQueryData);

    qDebug() << "[FlurryAgent] " << "Query: " << urlQueryData;

    qDebug() << "[FlurryAgent] " << "Data: " << postData;

    emit isSendingChanged(isSending_);

    QNetworkReply* postDataReply = networkManager_.get(sendDataRequest);
    new QReplyTimeout(postDataReply, 5000);
    postDataReply->connect(postDataReply, &QNetworkReply::finished, [this, postDataReply]
    {
        clearData();
        qDebug() << "[FlurryAgent] " << "data has been sent successfully";
        postDataReply->deleteLater();
        isSending_ = false;
        emit isSendingChanged(isSending_);
    });
}

void FlurryAgent::clearData()
{
    events_.clear();
    errorEvents_.clear();
}

QString FlurryAgent::formData()
{
    auto time1 = sessionStartTime_ + 4;
    auto time3 = QDateTime::currentMSecsSinceEpoch();
    auto delta = time3 - sessionStartTime_;

    QString baseData;
    QTextStream stream(&baseData);
    stream << "{\"a\":{\"af\":" << time3
         <<",\"aa\":1,\"ab\":10,\"ac\":9,\"ae\":\""<< appVersion_
         << "\",\"ad\":\"" << apiKey_
         << "\",\"ag\":" << sessionStartTime_
         << ",\"ah\":" << time1
         << ",\"ak\":1,"
         << "\"cg\":\"" << userIdHash_
         << "\"},\"b\":[{\"bd\":\"\",\"be\":\"\",\"bk\":-1,\"bl\":0,";

    if(longitude_ != 0.0 && latitude_ != 0.0)
    {
        stream << "\"bf\":{\"bg\":" << latitude_
               << ",\"bh\":" << longitude_
               << ",\"bi\":" << locationAccuracy_
               << "},";
    }

    stream << "\"bj\":\"" << QLocale::system().name().toLower().replace("_", "-") << "\",\"bo\":[";

    QMap<QString, int> eventsAndCounts;

    auto it = events_.begin();
    while(it != events_.end())
    {
        if(it->isReadyToSend())
        {
            ++eventsAndCounts[it->eventName()];
            if(it != events_.begin())
            {
                stream << ",";
            }
            stream << formEventToJson(*it);
        }
        ++it;
    }

    stream << "],\"bm\":false,\"bn\":{";

    QMap<QString, int>::ConstIterator statEventIt = eventsAndCounts.begin();
    while(statEventIt != eventsAndCounts.end())
    {
        if (statEventIt != eventsAndCounts.begin())
        {
            stream << ",";
        }
        stream << "\"" << statEventIt.key() << "\":" << statEventIt.value();
        ++statEventIt;
    }

    stream << "}" <<",\"bv\":[],\"bt\":false,\"bu\":{},\"by\":[";

    auto error = errorEvents_.begin();
    while(error != errorEvents_.end())
    {
        if(error != errorEvents_.begin())
        {
            stream << ",";
        }
        stream << formErrorToJson(*error);
        ++error;
    }

    stream <<  "],\"cd\":0,"
        << "\"ba\":" << time1
        << ",\"bb\":" << delta
        << ",\"bc\":-1,\"ch\":\"Etc/GMT-3\"}]}";

    return baseData;
}

QString FlurryAgent::formEventToJson(const FlurryEvent& event)
{
    QString eventData;
    QTextStream stream(&eventData);

    stream << "{\"ce\":" << event.id()
            << ",\"bp\":\"" << event.eventName()
            << "\",\"bq\":" << event.deltaTime()
            << ",\"bs\":{";

    QMap<QString, QString>::ConstIterator it = event.parameters().begin();
    while(it != event.parameters().end())
    {
        if(it != event.parameters().begin())
        {
            stream << ",";
        }
        stream << "\"" << it.key() << "\":\"" << it.value() << "\"";
        ++it;
    }

    stream << "}," << "\"br\":" << event.duration() << "}";

    return eventData;
}

QString FlurryAgent::formErrorToJson(const ErrorEvent& error)
{
    QString errorData;
    QTextStream stream(&errorData);

    stream << "{\"cf\":" << CURRENT_ERROR_ID
            << ",\"bz\":\"" << error.errorName()
            << "\",\"ca\":\"" << error.errorDesc()
            << "\",\"cb\":\"" << error.lineNumber()
            << "\",\"cc\":" << error.timestamp() << "}";

    return errorData;
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

