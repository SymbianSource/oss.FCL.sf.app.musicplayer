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
# Description: Project file for Music Player Engine.
#

symbian:TARGET.UID3 = 0x10207C93

TEMPLATE = lib
CONFIG += hb
TARGET = mpengine

TARGET.CAPABILITY = CAP_GENERAL_DLL

INCLUDEPATH += . \
    	inc \
    	../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor.dll \
    -lmpxplaybackutility.dll \
    -lmpxcollectionutility.dll \
    -lmpxharvesterutility.dll \
    -lmpxcommon.dll \
    -lmpxcollectionhelper.dll \
    -lremconcoreapi.dll \
    -lremconinterfacebase.dll \
    -lmpsettingsmanager \
    -lmpdata.dll \
    -lxqserviceutil
    
symbian:TARGET.EPOCALLOWDLLDATA	= 1

HEADERS += ../inc/mpengine.h \
           inc/mpmpxharvesterframeworkwrapper.h \
           inc/mpmpxharvesterframeworkwrapper_p.h \
           inc/mpmpxplaybackframeworkwrapper.h \
           inc/mpmpxplaybackframeworkwrapper_p.h \
           inc/mpsongscanner.h \
           inc/mpmediakeyhandler.h \
           inc/mpmediakeyhandler_p.h\
           inc/mpmediakeyremconresponse.h \
           inc/mpmpxcollectionframeworkwrapper.h \
           inc/mpmpxisolatedcollectionhelper.h \
           inc/mpmpxcollectionframeworkwrapper_p.h

SOURCES += src/mpengine.cpp \
           src/mpmpxharvesterframeworkwrapper.cpp \
           src/mpmpxharvesterframeworkwrapper_p.cpp \
           src/mpmpxplaybackframeworkwrapper.cpp \
           src/mpmpxplaybackframeworkwrapper_p.cpp \
           src/mpsongscanner.cpp \
           src/mpmediakeyhandler.cpp \
           src/mpmediakeyhandler_p.cpp \
           src/mpmediakeyremconresponse.cpp \
           src/mpmpxcollectionframeworkwrapper.cpp \
           src/mpmpxisolatedcollectionhelper.cpp \
           src/mpmpxcollectionframeworkwrapper_p.cpp       

DEFINES += BUILD_MPENGINE_LIB

myDefInclude = "NOSTRICTDEF" \
"$${LITERAL_HASH}if defined(WINS)"\
"DEFFILE  bwins/mpengineu.def "\
"$${LITERAL_HASH}else "\
"DEFFILE  eabi/mpengineu.def "\
"$${LITERAL_HASH}endif"
MMP_RULES += myDefInclude
