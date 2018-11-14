INCLUDEPATH += /opt/root/include
LIBS += $$system(root-config --libs)
LIBS += $$system(root-config --glibs)


HEADERS += \
    v0dst.h

SOURCES += \
    v0dst.cxx \
    la.cxx
