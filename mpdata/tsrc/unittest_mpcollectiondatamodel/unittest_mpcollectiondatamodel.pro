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
# Description: Unit test for mpcollectiondatamodel
#


TEMPLATE = app
CONFIG += qtestlib hb
TARGET = 

DEPENDPATH += .
INCLUDEPATH += . \
    stub/inc \
    ../../inc \
    ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
INCLUDEPATH += $$MW_LAYER_PUBLIC_EXPORT_PATH(hgwidgets)

LIBS += -lestor.dll \
        -lfbscli.dll

HEADERS += inc/unittest_mpcollectiondatamodel.h \
           inc/unittest_helper.h \
           stub/inc/mpcollectionalbumartmanager.h \
           stub/inc/mpmpxcollectiondata.h \
           ../../../inc/mpcollectiondatamodel.h \
               
SOURCES += src/unittest_mpcollectiondatamodel.cpp \
           src/unittest_helper.cpp \
           stub/src/mpcollectionalbumartmanager.cpp \
           stub/src/mpmpxcollectiondata.cpp \
           ../../src/mpcollectiondatamodel.cpp

DEFINES += BUILD_MPDATA_LIB

RESOURCES += resources/unittest_mpcollectiondatamodelresources.qrc
