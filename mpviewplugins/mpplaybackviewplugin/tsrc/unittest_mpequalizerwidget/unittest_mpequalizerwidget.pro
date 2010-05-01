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
    hb
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . \
    ../../inc \
    ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
LIBS += lestor.dll \
    -lfbscli.dll \

symbian: TARGET.EPOCALLOWDLLDATA = 1

HEADERS += stub/inc/mpequalizerwrapper.h \
    inc/unittest_mpequalizerwidget.h \
    ../../inc/mpequalizerwidget.h

SOURCES += stub/src/mpequalizerwrapper.cpp \
    src/unittest_mpequalizerwidget.cpp
