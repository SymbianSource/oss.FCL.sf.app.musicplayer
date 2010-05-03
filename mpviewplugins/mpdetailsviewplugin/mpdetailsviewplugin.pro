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

symbian:TARGET.UID3 = 0x2002D0AA

TEMPLATE = lib
CONFIG += hb qt ecomplugin
QT += webkit \
    network \
    xml
TARGET = mpdetailsviewplugin

SERVICE.INTERFACE_NAME = org.nokia.mmdt.MpxViewPlugin/1.0
SERVICE.CONFIGURATION = ""

TARGET.CAPABILITY = All -TCB

DEPENDPATH += .
INCLUDEPATH += . \
     inc \
     ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lmpxviewframeworkqt.dll \
		-lmpxplaybackutility.dll \
        -lmpxcommon.dll \
        -lthumbnailmanagerqt.dll 
    
symbian:TARGET.EPOCALLOWDLLDATA	= 1

# Input
SOURCES += src/mpdetailsviewplugin.cpp \
    	   src/mpdetailsview.cpp \
    	   src/mpmpxdetailsframeworkwrapper.cpp \
    	   src/mpmpxdetailsframeworkwrapper_p.cpp \
    	   src/mpsongdata.cpp

HEADERS = ../../inc/mpviewbase.h \
        inc/mpdetailsviewplugin.h \
    	  inc/mpdetailsview.h \
    	  inc/mpmpxdetailsframeworkwrapper.h \
    	  inc/mpmpxdetailsframeworkwrapper_p.h \
    	  inc/mpsongdata.h
    	  
RESOURCES += resources/mpdetailsviewresources.qrc
