QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Autoplace.cpp \
    Bound.cpp \
    ClipperCoor.cpp \
    CommonUtil.cpp \
    Config.cpp \
    DataExchange.cpp \
    GeneticAlgorithm.cpp \
    GeometryUtil.cpp \
    Gson.cpp \
    Individual.cpp \
    Nest.cpp \
    NestCoor.cpp \
    NestPath.cpp \
    NfpKey.cpp \
    NfpPair.cpp \
    NfpUtil.cpp \
    ParallelData.cpp \
    Placement.cpp \
    Placementworker.cpp \
    PostionUtil.cpp \
    Result.cpp \
    SegmentRelation.cpp \
    Segments.cpp \
    SvgUtil.cpp \
    Vector.cpp \
    clipper.cpp \
    nest.cpp

HEADERS += \
    Autoplace.h \
    Bound.h \
    ClipperCoor.h \
    CommonUtil.h \
    Config.h \
    DataExchange.h \
    GeneticAlgorithm.h \
    GeometryUtil.h \
    Gson.h \
    Individual.h \
    Nest.h \
    NestCoor.h \
    NestPath.h \
    NfpKey.h \
    NfpPair.h \
    NfpUtil.h \
    ParallelData.h \
    Placement.h \
    Placementworker.h \
    PostionUtil.h \
    Result.h \
    SegmentRelation.h \
    Segments.h \
    SvgUtil.h \
    Vector.h \
    clipper.hpp \
    nest.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
