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
# Description: Project file for Music Fetcher.
#

symbian:TARGET.UID3 = 0x10207C69

TEMPLATE = lib
CONFIG += hb
TARGET = musicfetcher

TARGET.CAPABILITY = CAP_GENERAL_DLL

INCLUDEPATH += . \
    	inc \
    	../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lxqservice

symbian:TARGET.EPOCALLOWDLLDATA	= 1

HEADERS += inc/musicfetcher.h

SOURCES += src/musicfetcher.cpp
    
DEFINES += BUILD_MUSICFETCHER_LIB

myDefInclude = "NOSTRICTDEF" \
"$${LITERAL_HASH}if defined(WINS)"\
"DEFFILE  bwins/musicfetcheru.def "\
"$${LITERAL_HASH}else "\
"DEFFILE  eabi/musicfetcheru.def "\
"$${LITERAL_HASH}endif"
MMP_RULES += myDefInclude
