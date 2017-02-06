INCLUDEPATH += $$PWD
HEADERS += $$PWD/flurryagent.h  \
    $$PWD/utils.h               \
    $$PWD/qreplytimeout.h

SOURCES += $$PWD/flurryagent.cpp    \
            $$PWD/qreplytimeout.cpp

DISTFILES += \
    $$PWD/flurry.js \
    $$PWD/main.html
