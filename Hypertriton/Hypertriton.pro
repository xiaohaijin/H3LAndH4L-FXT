INCLUDEPATH += $$system(include_star_files)

SOURCES += \
    reco.C \
    StRoot/StV0Maker/StV0Maker.cxx \
    StRoot/StV0Maker/StDcaService.cxx

HEADERS += \
    StRoot/StV0Maker/StV0Type.h \
    StRoot/StV0Maker/StV0Maker.h \
    StRoot/StV0Maker/StV0Dst.h \
    StRoot/StV0Maker/StDcaService.h
