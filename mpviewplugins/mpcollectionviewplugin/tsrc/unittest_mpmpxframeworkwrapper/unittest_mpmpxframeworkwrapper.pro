# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description: Unit test for mpmpxframeworkwrapper
TEMPLATE = app
CONFIG += qtestlib \
    hb
TARGET = 

TARGET.CAPABILITY = All -TCB

DEPENDPATH += .
INCLUDEPATH += . \
    ../../inc \
    ../../../../inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

LIBS += -lestor.dll \
    -lfbscli.dll \
    -lmpxcommon.dll \
    -lmpxcollectionutility.dll \
    -lmpsettingsmanager

HEADERS += inc/unittest_mpmpxframeworkwrapper.h \
    ../../inc/mpmpxframeworkwrapper.h \
    ../../inc/mpmpxframeworkwrapper_p.h \
    stub/inc/mpmpxcollectiondata.h \
    stub/inc/mpxcollectionopenutility.h \
    stub/inc/mpxcollectionuihelper.h \
    stub/inc/mpxcollectionutility.h \
    stub/inc/mpxcollectionhelperfactory.h \
    stub/inc/mpxharvesterutility.h \
    stub/inc/mpxplaybackutility.h \
    stub/inc/mpmpxisolatedcollectionhelper.h

SOURCES += src/unittest_mpmpxframeworkwrapper.cpp \
    ../../src/mpmpxframeworkwrapper.cpp \
    stub/src/mpmpxcollectiondata.cpp \
    stub/src/mpxcollectionopenutility.cpp \
    stub/src/mpxcollectionuihelper.cpp \
    stub/src/mpxcollectionutility.cpp \
    stub/src/mpxcollectionhelperfactory.cpp \
    stub/src/mpxharvesterutility.cpp \
    stub/src/mpxplaybackutility.cpp \
    stub/src/mpmpxisolatedcollectionhelper.cpp

RESOURCES += ../../resources/mpcollectionviewresources.qrc
