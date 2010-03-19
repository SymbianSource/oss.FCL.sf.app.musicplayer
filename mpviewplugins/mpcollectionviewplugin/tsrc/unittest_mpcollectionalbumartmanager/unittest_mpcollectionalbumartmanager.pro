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
# Description: Unit test for mpcollectionalbumartmanager
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

HEADERS += inc/unittest_mpcollectionalbumartmanager.h \
           stub/inc/thumbnailmanager_qt.h \
           ../../inc/mpcollectionalbumartmanager.h \
               
SOURCES += src/unittest_mpcollectionalbumartmanager.cpp \
           stub/src/thumbnailmanager_qt.cpp \
           ../../src/mpcollectionalbumartmanager.cpp

RESOURCES += ../../resources/mpcollectionviewresources.qrc
