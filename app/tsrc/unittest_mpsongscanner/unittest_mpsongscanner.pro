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
CONFIG += qtestlib hb
TARGET = 

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += . \
    stub/inc \
    ../../../inc \
    
symbian:
{
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
}

LIBS += -lestor.dll \
        -lfbscli.dll \
        -lmpxcommon.dll \

# Input
HEADERS += inc/unittest_mpsongscanner.h \
           ../../inc/mpsongscanner.h \
           ../../inc/mpsongscanner_p.h \
           stub/inc/mpxharvesterutility.h \
           stub/inc/mpxcollectionutility.h \
               
SOURCES += src/unittest_mpsongscanner.cpp \
           ../../src/mpsongscanner.cpp \
           ../../src/mpsongscanner_p.cpp \
           stub/src/mpxharvesterutility.cpp \
           stub/src/mpxcollectionutility.cpp \
