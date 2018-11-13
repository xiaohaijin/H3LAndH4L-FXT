QT += core

INCLUDEPATH += $$(ROOTSYS)/include
INCLUDEPATH += $$(BOOSTSYS)/include

LIBS += $$system(root-config --libs)
LIBS += $$system(root-config --glibs)

HEADERS += \
    V0PicoDst.h

SOURCES += \
    V0PicoDst.cpp \
    analysis.cpp

