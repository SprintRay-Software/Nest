QT -= gui
TEMPLATE = lib
DEFINES += AUTOPLACE_LIBRARY
DEFINES += SEGMENTS_LIBRARY
CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
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
    autoplace.cpp \
    nestpath.cpp

HEADERS += \
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
    Segments_global.h \
    SvgUtil.h \
    Vector.h \
    clipper.hpp \
    AutoPlace_global.h \
    autoplace.h \
    nestpath.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
win32{
    CONFIG(release,debug|release){
message(release)
        TARGET = AutoPlace
        target.path = D:\QTProjects\MR\Moonray\PlanU\Moonray-0319\Moonray-0319\AutoPlace\AutoPlace\lib\
        target.files += AutoPlace.dll
        #target.files += D:\QTProjects\MR\Nest_DLL\build-AutoPlace-Desktop_Qt_5_14_2_MSVC2017_64bit-Release\release\AutoPlace.lib
    }else{
message(debug)
        #CONFIG += debug     //not useful
        TARGET = AutoPlaced
        target.path = D:\QTProjects\MR\Moonray\PlanU\Moonray-0319\Moonray-0319\AutoPlace\AutoPlace\lib\
        #target.files += D:\QTProjects\MR\Nest_DLL\build-AutoPlace-Desktop_Qt_5_14_2_MSVC2017_64bit-Debug\debug\AutoPlace.dll
        target.files += AutoPlaced.dll
    }
}
mac{
DEFINES += OM_STATIC_BUILD      #for openMesh
message(Mac)
CONFIG(release,debug|release){
message(release)
    TARGET = AutoPlace
    target.path = /Users/qiangchen/Desktop/Junxiang/Moonray-UNested/AutoPlace/AutoPlace/lib/
    target.files = libAutoPlace.dylib
}else{
message(debug)
    TARGET = AutoPlaced
    target.path = /Users/qiangchen/Desktop/Junxiang/Moonray-UNested/AutoPlace/AutoPlace/lib/
    target.files = libAutoPlaced.dylib
}
}
INSTALLS += target
#Note: Project - Config - Make --> add arguments install .otherwise the script up will not work
