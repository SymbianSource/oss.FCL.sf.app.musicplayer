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
    hb
TARGET = 

TARGET.CAPABILITY = All -TCB

DEPENDPATH += .
INCLUDEPATH += ./stub/inc \
    ../../inc \
    ../../../inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

LIBS += -lestor.dll \
    -lfbscli.dll \
     -lmpxcommon.dll \
     -lthumbnailmanagerqt.dll


HEADERS += inc/unittest_mpmpxdetailsframeworkwrapper.h \
    stub/inc/mpsongdata.h \
    stub/inc/mpxplaybackutility.h \
    stub/inc/mpxplayermanager.h \
    stub/inc/mpxsource.h \
    ../../inc/mpmpxdetailsframeworkwrapper.h \
    ../../inc/mpmpxdetailsframeworkwrapper_p.h
 
SOURCES += src/unittest_mpmpxdetailsframeworkwrapper.cpp \
    stub/src/mpxplaybackutility.cpp \
    stub/src/mpsongdata.cpp \    
    stub/src/mpxplayermanager.cpp \
    stub/src/mpxsource.cpp \    
    ../../src/mpmpxdetailsframeworkwrapper.cpp \
    ../../src/mpmpxdetailsframeworkwrapper_p.cpp

