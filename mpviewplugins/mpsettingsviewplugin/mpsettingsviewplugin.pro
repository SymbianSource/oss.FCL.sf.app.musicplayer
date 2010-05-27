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

symbian:TARGET.UID3 = 0x10207C66

TEMPLATE = lib
CONFIG += hb qt ecomplugin
TARGET = mpsettingsviewplugin

SERVICE.INTERFACE_NAME = org.nokia.mmdt.MpxViewPlugin/1.0
SERVICE.CONFIGURATION = "<t>0x101FFCA2</t><i>EMPXViewPluginPriorityLowest</i>"

TARGET.CAPABILITY = All -TCB

DEPENDPATH += .
INCLUDEPATH += . \
     inc \
     ../../inc \
     ../../mpserviceplugins/inc
     
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
     
LIBS += -lmpxplaybackutility.dll \
        -lestor.dll \
	-lmpxviewframeworkqt.dll \
	-lmpengine.dll
	
symbian:TARGET.EPOCALLOWDLLDATA = 1

HEADERS += ../../inc/mpviewbase.h \
      inc/mpsettingsviewplugin.h \
      inc/mpsettingsview.h \
      inc/mpsettingsaudioeffectswidget.h

SOURCES += src/mpsettingsviewplugin.cpp \
       src/mpsettingsview.cpp \
       src/mpsettingsaudioeffectswidget.cpp 
       
RESOURCES += resources/mpsettingsview.qrc

