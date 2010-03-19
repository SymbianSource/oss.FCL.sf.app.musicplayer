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

symbian:TARGET.UID3 = 0x10207C63

TEMPLATE = lib
CONFIG += hb qt ecomplugin
TARGET = mpcollectionviewplugin

SERVICE.INTERFACE_NAME = org.nokia.mmdt.MpxViewPlugin/1.0
SERVICE.CONFIGURATION = "<t>0x101FFCA1</t><p>0x101FFC3A;0xE419BEEE</p><i>EMPXViewPluginPriorityNormal</i><f>0x00000001</f>"

TARGET.CAPABILITY = All -TCB

DEPENDPATH += .
INCLUDEPATH += . \
     inc \
     ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor.dll \
    -lthumbnailmanagerqt.dll \
    -lmpxplaybackutility.dll \
    -lmpxcollectionutility.dll \
    -lmpxharvesterutility.dll \
    -lmpxcommon.dll \
    -lmpxviewframeworkqt.dll \
    -lmpxcollectionhelper.dll \
    -lmpnowplayingbanner.dll  \
    -lhbcore \
    -lhbwidgets \
    -lhbtools \
    -lmpsettingsmanager
    
symbian:TARGET.EPOCALLOWDLLDATA	= 1

# Input
SOURCES += src/mpcollectionviewplugin.cpp \
    	   src/mpcollectionview.cpp \
    	   src/mpcollectiondocumentloader.cpp \
    	   src/mpcollectioncontainerfactory.cpp \
    	   src/mpcollectioncontainer.cpp \
    	   src/mpcollectionlistcontainer.cpp \
    	   src/mpcollectioncontainerallsongs.cpp \
    	   src/mpcollectioncontainerartists.cpp \
    	   src/mpcollectioncontainerplaylists.cpp \
    	   src/mpcollectioncontainergenres.cpp \
    	   src/mpcollectiondatamodel.cpp \
    	   src/mpcollectionalbumartmanager.cpp \
    	   src/mpcollectioninfobar.cpp \
           src/mpcollectionsongscanner.cpp \
           src/mpmpxframeworkwrapper.cpp \
           src/mpmpxframeworkwrapper_p.cpp \
           src/mpmpxcollectiondata.cpp \
           src/mpmpxcollectiondata_p.cpp \
           src/mpsnapshotwidget.cpp

HEADERS = ../../inc/mpviewbase.h \
          inc/mpcollectionviewplugin.h \
    	  inc/mpcollectionview.h \
    	  inc/mpcollectiondocumentloader.h \
    	  inc/mpcollectioncontainerfactory.h \
    	  inc/mpcollectioncontainer.h \
    	  inc/mpcollectionlistcontainer.h \
    	  inc/mpcollectioncontainerallsongs.h \
    	  inc/mpcollectioncontainerartists.h \
    	  inc/mpcollectioncontainerplaylists.h  \
    	  inc/mpcollectioncontainergenres.h  \
    	  inc/mpcollectiondatamodel.h \
    	  inc/mpcollectionalbumartmanager.h \
    	  inc/mpcollectioninfobar.h \
          inc/mpcollectionsongscanner.h \
          inc/mpmpxcollectionviewdefs.h \
          inc/mpmpxframeworkwrapper.h \
          inc/mpmpxframeworkwrapper_p.h \
          inc/mpmpxcollectiondata.h \
          inc/mpmpxcollectiondata_p.h  \
          inc/mpsnapshotwidget.h
       
RESOURCES += resources/mpcollectionviewresources.qrc
