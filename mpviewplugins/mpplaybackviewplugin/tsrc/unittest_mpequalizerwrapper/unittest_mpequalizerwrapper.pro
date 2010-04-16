# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description: Unit test for mpequalizerwidget
TEMPLATE = app
CONFIG += qtestlib \
    hb \
TARGET = 
TARGET.CAPABILITY = All -TCB
DEPENDPATH += .
INCLUDEPATH += . \
    ../../inc \
    ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

LIBS += lestor.dll \
    -lfbscli.dll \
    -laudioequalizerutility.dll \
    -lmpsettingsmanager
    
symbian: TARGET.EPOCALLOWDLLDATA = 1
    
HEADERS += stub/inc/mpxuser.h \
    stub/inc/mpxplaybackutility.h \
    stub/inc/audioequalizerutility.h \
    inc/unittest_mpequalizerwrapper.h \
    ../../inc/mpequalizerwrapper.h \
    ../../inc/mpequalizerwrapper_p.h

SOURCES += stub/src/mpxuser.cpp \
    stub/src/mpxplaybackutility.cpp \
    stub/src/audioequalizerutility.cpp \
    src/unittest_mpequalizerwrapper.cpp \
    ../../src/mpequalizerwrapper.cpp
