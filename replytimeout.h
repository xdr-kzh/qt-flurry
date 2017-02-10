#ifndef REPLYTIMEOUT_H
#define REPLYTIMEOUT_H

#include <QObject>
#include <QNetworkReply>
#include <QTimer>

class ReplyTimeout : public QObject
{
    Q_OBJECT
public:
    ReplyTimeout(QNetworkReply* reply, const int timeout) : QObject(reply) {
        Q_ASSERT(reply);
        if (reply) {
          QTimer::singleShot(timeout, this, SLOT(timeout()));
        }
      }

private slots:
    void timeout() {
        QNetworkReply* reply = static_cast<QNetworkReply*>(parent());
        if (reply->isRunning()) {
          reply->close();
        }
      }
};

#endif // REPLYTIMEOUT_H
