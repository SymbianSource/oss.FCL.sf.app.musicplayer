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

LIBS += -lmpxviewframeworkqt.dll
				
HEADERS += inc/unittest_mpsettingsviewplugin.h \        
           ../../inc/mpsettingsviewplugin.h \
           stub/inc/mpsettingsview.h
               
SOURCES += stub/src/mpsettingsview.cpp \
           ../../src/mpsettingsviewplugin.cpp \
           src/unittest_mpsettingsviewplugin.cpp
