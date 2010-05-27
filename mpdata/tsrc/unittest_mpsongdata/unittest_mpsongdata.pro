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
# Description: Unit test for mpsongdata
#

TEMPLATE = app
CONFIG += qtestlib \
					symbian_test \
							hb

TARGET = 
DEPENDPATH += . 
INCLUDEPATH += ./stub/inc \   
    		   ../../../inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
DEFINES += BUILD_MPDATA_LIB
LIBS += -lmpxviewframeworkqt.dll 

# Input
HEADERS += inc/unittest_mpsongdata.h \
	   ../../../inc/mpsongdata.h \
	   stub/inc/thumbnailmanager_qt.h 
	   
SOURCES += src/unittest_mpsongdata.cpp \
	   ../../src/mpsongdata.cpp \
	   stub/src/thumbnailmanager_qt.cpp


