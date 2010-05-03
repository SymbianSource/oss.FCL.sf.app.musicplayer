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

symbian:TARGET.UID3 = 0x10207C64

TEMPLATE = lib
CONFIG += hb qt ecomplugin
TARGET = mpplaybackviewplugin

SERVICE.INTERFACE_NAME = org.nokia.mmdt.MpxViewPlugin/1.0
SERVICE.CONFIGURATION = "<t>0x101FFCA0</t><p>0x101FFC06;0x101FFC3A;0xE419BEEE</p><i>EMPXViewPluginPriorityNormal</i><f>0x00000002</f>"

TARGET.CAPABILITY = All -TCB

DEPENDPATH += .
INCLUDEPATH += . \
     inc \
     ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
     
LIBS += -lmpxplaybackutility.dll \
        -lmpxcommon.dll \
        -lestor.dll \
		-lmpxviewframeworkqt.dll \
		-laudioequalizerutility.dll \
		-lmpsettingsmanager \
		-lmpengine \
		-lmpdata.dll
	
symbian:
{
TARGET.EPOCALLOWDLLDATA = 1
    
BLD_INF_RULES.prj_exports += \
    "resources/nullsound.mp3 /epoc32/release/winscw/udeb/Z/nullsound.mp3" \
    "resources/nullsound.mp3 /epoc32/data/Z/system/data/nullsound.mp3"
}

HEADERS = ../../inc/mpviewbase.h \
	  inc/mpplaybackviewplugin.h \
      inc/mpplaybackview.h \
      inc/mpplaybackwidget.h \
      inc/mpequalizerwrapper.h \
      inc/mpequalizerwrapper_p.h \
      inc/mpequalizerwidget.h

SOURCES += src/mpplaybackviewplugin.cpp \
       src/mpplaybackview.cpp \
       src/mpplaybackwidget.cpp \
       src/mpequalizerwrapper.cpp \
       src/mpequalizerwrapper_p.cpp \
       src/mpequalizerwidget.cpp
       
RESOURCES += resources/mpplaybackviewpluginresources.qrc

