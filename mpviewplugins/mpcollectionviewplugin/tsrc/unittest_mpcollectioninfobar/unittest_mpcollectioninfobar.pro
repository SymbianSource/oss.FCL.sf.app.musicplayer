# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description: Unit test for mpcollectioninfobar
TEMPLATE = app
CONFIG += qtestlib \
    hb
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . \
    ../../inc \
    ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
LIBS += -lestor.dll \
    -lfbscli.dll

HEADERS += inc/unittest_mpcollectioninfobar.h \
    ../../inc/mpcollectioninfobar.h 
SOURCES += src/unittest_mpcollectioninfobar.cpp \
    ../../src/mpcollectioninfobar.cpp 
RESOURCES += ../../resources/mpcollectionviewresources.qrc

