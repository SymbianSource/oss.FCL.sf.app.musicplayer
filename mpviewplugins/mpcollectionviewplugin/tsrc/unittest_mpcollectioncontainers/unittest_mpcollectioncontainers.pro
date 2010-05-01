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

LIBS += -lestor.dll \
    	-lmpnowplayingbanner.dll


HEADERS += inc/unittest_mpcollectioncontainers.h \
           ../../inc/mpcollectioncontainerfactory.h \
           ../../inc/mpcollectioncontainer.h \
           ../../inc/mpcollectionlistcontainer.h \
           ../../inc/mpcollectioncontainerallsongs.h \
           ../../inc/mpcollectioncontainerartists.h \
           ../../inc/mpcollectioncontaineralbums.h \
           ../../inc/mpcollectioncontainerplaylists.h \
           ../../inc/mpcollectioncontainergenres.h \
           ../../inc/mpcollectiondocumentloader.h \
           stub/inc/mpcollectiondatamodel.h \
           stub/inc/mpmpxcollectiondata.h \
           stub/inc/mpcollectionview.h
               
SOURCES += src/unittest_mpcollectioncontainers.cpp \
           ../../src/mpcollectioncontainerfactory.cpp \
           ../../src/mpcollectioncontainer.cpp \
           ../../src/mpcollectionlistcontainer.cpp \
           ../../src/mpcollectioncontainerallsongs.cpp \
           ../../src/mpcollectioncontainerartists.cpp \
           ../../src/mpcollectioncontaineralbums.cpp \
           ../../src/mpcollectioncontainerplaylists.cpp \
           ../../src/mpcollectioncontainergenres.cpp \ 
           ../../src/mpcollectiondocumentloader.cpp \
           stub/src/mpcollectiondatamodel.cpp \
           stub/src/mpmpxcollectiondata.cpp \
           stub/src/mpcollectionview.cpp 
           
RESOURCES += ../../resources/mpcollectionviewresources.qrc