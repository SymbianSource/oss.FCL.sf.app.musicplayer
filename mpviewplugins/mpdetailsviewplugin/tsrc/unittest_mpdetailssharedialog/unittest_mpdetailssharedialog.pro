#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#
TEMPLATE = app
CONFIG += qtestlib \
          symbian_test \
          hb \
          mobility

QT += webkit \
    network \
    xml

MOBILITY += bearer systeminfo

TARGET = unittest_mpdetailssharedialog

TARGET.CAPABILITY = All -TCB

DEPENDPATH += .
INCLUDEPATH += ./inc \
    ./stub/inc \
    ../../inc \
    ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += SHARE_FUNC_ENABLED SHARE_PLAYER_RND

# Input
HEADERS += inc/unittest_mpdetailssharedialog.h \
    ../../inc/mpdetailssharedialog.h \
    ../../inc/mpsharedata.h \
    stub/inc/mpsongdata.h \
    stub/inc/hbmessagebox.h
win32:{
HEADERS += stub/inc/qsysteminfo.h
}
SOURCES += src/unittest_mpdetailssharedialog.cpp \
    ../../src/mpdetailssharedialog.cpp \
    ../../src/mpsharedata.cpp \
    stub/src/mpsongdata.cpp \
    stub/src/hbmessagebox.cpp

win32:{
SOURCES += stub/src/qsysteminfo.cpp
}
