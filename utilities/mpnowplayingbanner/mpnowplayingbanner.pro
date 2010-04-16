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

symbian:TARGET.UID3 = 0x10207C67
    
TEMPLATE = lib
CONFIG += hb qt
TARGET = mpnowplayingbanner

TARGET.CAPABILITY = All -TCB

INCLUDEPATH += . \
	 inc \
	 ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
     
LIBS += -lmpxplaybackutility.dll \
        -lmpxcommon.dll
        
symbian:TARGET.EPOCALLOWDLLDATA	= 1

HEADERS = ../../inc/mpnowplayingwidget.h \
    inc/mpnowplayingbackend.h \
    inc/mpnowplayingwidget_p.h \
    inc/mpnowplayingbackend_p.h

SOURCES += src/mpnowplayingwidget.cpp \
    src/mpnowplayingwidget_p.cpp \
    src/mpnowplayingbackend.cpp \
    src/mpnowplayingbackend_p.cpp
                
RESOURCES += resources/mpnowplayingbanner.qrc

DEFINES += BUILD_MPNOWPLAYINGBANNER

myDefInclude = "NOSTRICTDEF" \
"$${LITERAL_HASH}if defined(WINS)"\
"DEFFILE  bwins/mpnowplayingbanneru.def "\
"$${LITERAL_HASH}else "\
"DEFFILE  eabi/mpnowplayingbanneru.def "\
"$${LITERAL_HASH}endif"
MMP_RULES += myDefInclude
