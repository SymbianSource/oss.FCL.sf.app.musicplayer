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
    ../../inc \
    ../../../../inc \
    ../../../../mpserviceplugins/inc

    
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lmpxviewframeworkqt.dll
				
HEADERS += inc/unittest_mpmpxasframeworkwrapper.h \        
   		   ../../inc/mpmpxasframeworkwrapper.h \
   		   ../../inc/mpmpxasframeworkwrapper_p.h \
           stub/inc/mpxaudioeffectproperties.h \
           stub/inc/mpxplaybackutility.h
               
SOURCES += src/unittest_mpmpxasframeworkwrapper.cpp \
		   ../../src/mpmpxasframeworkwrapper.cpp \
		   stub/src/mpxaudioeffectproperties.cpp \
		   stub/src/mpxplaybackutility.cpp
		   
           
           
