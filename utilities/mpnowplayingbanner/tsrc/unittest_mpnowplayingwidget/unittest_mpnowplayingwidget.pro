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

DEPENDPATH += .
INCLUDEPATH += . \
    stub/inc \
    ../../inc \
    ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor.dll \
        -lfbscli.dll

# Input
HEADERS += inc/unittest_mpnowplayingwidget.h \
           stub/inc/mpnowplayingbackend.h \
           ../../../../inc/mpnowplayingwidget.h \
           ../../inc/mpnowplayingwidget_p.h
               
SOURCES += src/unittest_mpnowplayingwidget.cpp \
           stub/src/mpnowplayingbackend.cpp \
           ../../src/mpnowplayingwidget.cpp \
           ../../src/mpnowplayingwidget_p.cpp

RESOURCES += ../../resources/mpnowplayingbanner.qrc

DEFINES += BUILD_MPNOWPLAYINGBANNER
