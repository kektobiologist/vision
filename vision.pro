#-------------------------------------------------
#
# Project created by QtCreator 2012-12-17T00:20:21
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = vision
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    calibrator.cpp \
    lut3d.cpp \
    yuv-conversions.cpp \
    yuvlabel.cpp \
    cameraworker.cpp \
    calibratorworker.cpp \
    lineinterpolate.cpp \
    camlabel.cpp \
    arena.cpp \
    blob.cpp \
    beliefstate.cpp \
    algoworker.cpp \
    serial.cpp

HEADERS  += mainwindow.h \
    calibrator.h \
    lut3d.h \
    yuv-conversions.h \
    yuvlabel.h \
    cameraworker.h \
    calibratorworker.h \
    lineinterpolate.h \
    camlabel.h \
    arena.h \
    blob.h \
    beliefstate.h \
    algoworker.h \
    config.h \
    serial.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/include/opencv \
               /usr/include/opencv2 \
               /usr/local/include
LIBS += -L/usr/local/lib \
        -lopencv_calib3d \
        -lopencv_contrib \
        -lopencv_core \
        -lopencv_features2d \
        -lopencv_flann \
        -lopencv_gpu \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lopencv_legacy \
        -lopencv_ml \
        -lopencv_objdetect \
        -lopencv_photo \
        -lopencv_stitching \
        -lopencv_ts \
        -lopencv_video \
        -lopencv_videostab
