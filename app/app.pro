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

SERVICEAPP = app
TARGET = musicplayer
ICON = resources/qgn_menu_mp_qt.svg
DEPENDPATH += .
INCLUDEPATH += . \
	 inc \
	 ../inc \
         ../musicfetcher/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

CONFIG += hb

symbian: {
    TARGET.UID2 = 0x100039CE 
    TARGET.UID3 = 0x10207C62
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x1F00000
    TARGET.CAPABILITY = CAP_APPLICATION NetworkControl DRM
    }

# Service provider specific configuration.
# If in-process plugin specific implementation
# will be possible in future, service configuration
# can be located in musicfetcher library's pro file.
SERVICE.FILE = data/service_conf.xml
SERVICE.OPTIONS = embeddable
CONFIG += service
# Service provider specific configuration ends

# Input
LIBS += -lecom.dll \
    -lestor.dll  \
    -lfbscli.dll \
    -lxqplugins.dll \
    -lmpxviewframeworkqt.dll \
    -lxqserviceutil \
    -lmusicfetcher \
    -lmpengine
    
    
SOURCES += src/main.cpp \
           src/mpmainwindow.cpp
           
HEADERS =  inc/mpmainwindow.h

MMP_RULES+=EXPORTUNFROZEN

symbian: {
    addImages.sources = images/*.png \
                        resources/*.xml
    DEPLOYMENT += addImages
    }
    
RESOURCES += resources/musiplayerapp.qrc

TRANSLATIONS = musicplayer.ts