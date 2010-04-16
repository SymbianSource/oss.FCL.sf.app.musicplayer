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

TEMPLATE = app
CONFIG += qtestlib hb
TARGET = 

DEPENDPATH += .
INCLUDEPATH += . \
    stub/inc \
    ../../../../inc

    
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

HEADERS += inc/unittest_mpcollectiondocumentloader.h \
           ../../inc/mpcollectiondocumentloader.h \
           stub/inc/mpnowplayingwidget.h
               
SOURCES += src/unittest_mpcollectiondocumentloader.cpp \
           ../../src/mpcollectiondocumentloader.cpp \
           stub/src/mpnowplayingwidget.cpp
