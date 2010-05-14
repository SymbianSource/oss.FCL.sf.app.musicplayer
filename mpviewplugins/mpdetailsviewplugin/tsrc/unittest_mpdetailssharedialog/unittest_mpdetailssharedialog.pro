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
# Description: Unit test for mpdetailssharedialog
#

TEMPLATE = app
CONFIG += qtestlib \
          symbian_test \
          hb

QT += webkit \
    network \
    xml

TARGET = unittest_mpdetailssharedialog
DEPENDPATH += .
INCLUDEPATH += ./inc \
    ./stub/inc \
    ../../inc \
    ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += SHARE_FUNC_ENABLED

# Input
HEADERS += inc/unittest_mpdetailssharedialog.h \
    ../../inc/mpdetailssharedialog.h \
    ../../inc/mpsharedata.h \
    stub/inc/mpsongdata.h
SOURCES += src/unittest_mpdetailssharedialog.cpp \
    ../../src/mpdetailssharedialog.cpp \
    ../../src/mpsharedata.cpp \
    stub/src/mpsongdata.cpp
