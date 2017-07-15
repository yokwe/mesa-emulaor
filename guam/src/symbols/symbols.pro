######################################################################
# Automatically generated by qmake (3.0) Wed Oct 30 15:10:08 2013
######################################################################

TARGET = symbols

TEMPLATE = app

QMAKE_LFLAGS   += -g

unix {
        QMAKE_LFLAGS   += -rdynamic
}

win32 {
        QMAKE_LFLAGS   += -static
}

contains(QT_MAJOR_VERSION, 4) {
        QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
}

LIBS += ../../tmp/build/mesa/libmesa.a
LIBS += ../../tmp/build/util/libutil.a

LIBS += -llog4cpp

POST_TARGETDEPS += ../../tmp/build/mesa/libmesa.a
POST_TARGETDEPS += ../../tmp/build/util/libutil.a

# Input
HEADERS += BCD.h   BCDFile.h   BTIndex.h   CTXIndex.h   ExtIndex.h   HTIndex.h   LTIndex.h   MDIndex.h   SEIndex.h   Symbols.h   Tree.h
SOURCES += BCD.cpp BCDFile.cpp BTIndex.cpp CTXIndex.cpp ExtIndex.cpp HTIndex.cpp LTIndex.cpp MDIndex.cpp SEIndex.cpp Symbols.cpp Tree.cpp

SOURCES += main.cpp


###############################################

INCLUDEPATH += .

QMAKE_CXXFLAGS += -std=c++0x -Wall -Werror -g

DESTDIR     = ../../tmp/build/$$TARGET
OBJECTS_DIR = ../../tmp/build/$$TARGET
MOC_DIR     = ../../tmp/build/$$TARGET
RCC_DIR     = ../../tmp/build/$$TARGET
UI_DIR      = ../../tmp/build/$$TARGET
