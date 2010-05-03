# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description: Unit test for MusicServices

TEMPLATE = app

CONFIG += qtestlib \
    hb
TARGET = 

DEFINES += BUILD_MUSICSERVICES_LIB

TARGET.CAPABILITY = All -TCB

DEPENDPATH += .
INCLUDEPATH += . \
    ../../inc \
    ../../../inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

LIBS += -lestor.dll \
    -lfbscli.dll \
    -lxqservice

HEADERS += inc/unittest_musicservices.h \
    ../../inc/musicservices.h \ 
    ../../inc/getmusicservice.h \
    ../../inc/playmusicservice.h 

SOURCES += src/unittest_musicservices.cpp \
        ../../src/musicservices.cpp \
        ../../src/getmusicservice.cpp \
    	../../src/playmusicservice.cpp  
