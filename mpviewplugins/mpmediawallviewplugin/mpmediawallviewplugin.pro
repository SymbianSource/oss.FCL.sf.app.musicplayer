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

symbian:TARGET.UID3 = 0x10207C94

TEMPLATE = lib
CONFIG += hb qt ecomplugin
HB += hbfeedback
TARGET = mpmediawallviewplugin

SERVICE.INTERFACE_NAME = org.nokia.mmdt.MpxViewPlugin/1.0
SERVICE.CONFIGURATION = "<i>EMPXViewPluginPriorityNormal</i>"

TARGET.CAPABILITY = All -TCB

DEPENDPATH += .
INCLUDEPATH += . \
     inc \
     ../../inc \
     ../../mpserviceplugins/inc
     
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$MW_LAYER_PUBLIC_EXPORT_PATH(hgwidgets)
     
LIBS += -lestor.dll \
				-lmpxviewframeworkqt.dll \
				-lganeswidgets.dll \
				-lmpengine \
				-lmpdata.dll
	
symbian:TARGET.EPOCALLOWDLLDATA = 1

HEADERS += ../../inc/mpviewbase.h \
	         inc/mpmediawallviewplugin.h \
           inc/mpmediawallview.h \
    inc/mpmediawalldocumentloader.h \
    inc/mpalbumcoverwidget.h \
    inc/mptracklistwidget.h
SOURCES += src/mpmediawallviewplugin.cpp \
           src/mpmediawallview.cpp \
    src/mpmediawalldocumentloader.cpp \
    src/mpalbumcoverwidget.cpp \
    src/mptracklistwidget.cpp
RESOURCES += resources/mpmediawallviewresources.qrc
