#ifndef FLURRYAGENT_H
#define FLURRYAGENT_H

#include <QString>

class FlurryAgent
{
    const static QString FLURRY_BASE_URL;

public:
    FlurryAgent();
    void init();
    void load();
    void startSave();
    void delayedStartSend();
};

#endif // FLURRYAGENT_H
