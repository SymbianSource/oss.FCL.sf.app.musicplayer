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
# Description: Project file for Music Player Data.
#

symbian:TARGET.UID3 = 0x10207C95

TEMPLATE = lib
CONFIG += hb
TARGET = mpdata

TARGET.CAPABILITY = CAP_GENERAL_DLL

INCLUDEPATH += . \
    	inc \
    	../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor.dll \
    -lmpxcommon.dll \
    -lthumbnailmanagerqt.dll
    
symbian:TARGET.EPOCALLOWDLLDATA	= 1

HEADERS += ../inc/mpmpxcollectiondata.h \
           inc/mpmpxcollectiondata_p.h \
           ../inc/mpcollectiondatamodel.h \
           ../inc/mpcollectiontbonelistdatamodel.h \
    	   inc/mpcollectionalbumartmanager.h \
    	   ../inc/mpplaybackdata.h \
    	   ../inc/mpsongdata.h

SOURCES += src/mpmpxcollectiondata.cpp \
           src/mpmpxcollectiondata_p.cpp \
           src/mpcollectiondatamodel.cpp \
           src/mpcollectiontbonelistdatamodel.cpp \
    	   src/mpcollectionalbumartmanager.cpp \
           src/mpplaybackdata.cpp \
           src/mpsongdata.cpp
           
DEFINES += BUILD_MPDATA_LIB

myDefInclude = "NOSTRICTDEF" \
"$${LITERAL_HASH}if defined(WINS)"\
"DEFFILE  bwins/mpdatau.def "\
"$${LITERAL_HASH}else "\
"DEFFILE  eabi/mpdatau.def "\
"$${LITERAL_HASH}endif"
MMP_RULES += myDefInclude
