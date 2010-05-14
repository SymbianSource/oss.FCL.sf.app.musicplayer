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
CONFIG += qtestlib
CONFIG += symbian_test
TARGET = unittest_mpnowplayingbackend
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
    stub/inc \
    ../../inc \
    ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor.dll \
        -lfbscli.dll \
        -lmpxcommon.dll

HEADERS += inc/unittest_mpnowplayingbackend.h \
           stub/inc/mpxplaybackutility.h \
           ../../inc/mpnowplayingbackend.h \
           ../../inc/mpnowplayingbackend_p.h \
               
SOURCES += src/unittest_mpnowplayingbackend.cpp \
           stub/src/mpxplaybackutility.cpp \
           ../../src/mpnowplayingbackend.cpp \
           ../../src/mpnowplayingbackend_p.cpp
