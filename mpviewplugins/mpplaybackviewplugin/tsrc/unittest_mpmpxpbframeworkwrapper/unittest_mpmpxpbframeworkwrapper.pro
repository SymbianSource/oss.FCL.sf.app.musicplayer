# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description: Unit test for mpmpxpbframeworkwrapper
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
    -lmpxcollectionutility.dll 

HEADERS += inc/unittest_mpmpxpbframeworkwrapper.h \
    ../../inc/mpmpxpbframeworkwrapper.h \
    ../../inc/mpmpxpbframeworkwrapper_p.h \
    stub/inc/mpplaybackdata.h \
	stub/inc/mpxcollectionutility.h \
	stub/inc/mpxplaybackutility.h 

SOURCES += src/unittest_mpmpxpbframeworkwrapper.cpp \
	stub/src/mpplaybackdata.cpp \
	stub/src/mpxcollectionutility.cpp \
	stub/src/mpxplaybackutility.cpp 

RESOURCES += ../../resources/mpplaybackviewpluginresources.qrc
