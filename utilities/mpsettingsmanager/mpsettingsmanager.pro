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

symbian:TARGET.UID3 = 0x10207C68
    
TEMPLATE = lib
CONFIG += hb qt
TARGET = mpsettingsmanager

TARGET.CAPABILITY = All -TCB

INCLUDEPATH += . \
               inc \
               ../../inc
               
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS+= -lxqsettingsmanager

symbian:TARGET.EPOCALLOWDLLDATA	= 1

HEADERS = ../../inc/mpsettingsmanager.h 
    
SOURCES += src/mpsettingsmanager.cpp
DEFINES += BUILD_MPSETTINGSMANAGER


myDefInclude = "NOSTRICTDEF" \
"$${LITERAL_HASH}if defined(WINS)"\
"DEFFILE  bwins/mpsettingsmanageru.def "\
"$${LITERAL_HASH}else "\
"DEFFILE  eabi/mpsettingsmanageru.def "\
"$${LITERAL_HASH}endif"
MMP_RULES += myDefInclude

BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "conf/musicplayer.confml                   APP_LAYER_CONFML(musicplayer.confml)" \
     "conf/musicplayer_10207C92.crml        APP_LAYER_CRML(musicplayer_10207C92.crml)"
