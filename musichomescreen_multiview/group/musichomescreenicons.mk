#
# Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  icons makefile for project MusicHomeScreen
#


ifeq (WINS,$(findstring WINS,$(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32/release/$(PLATFORM)/$(CFG)/z
else
ZDIR=$(EPOCROOT)epoc32/data/z
endif

TARGETDIR=$(ZDIR)/resource/apps
HEADERDIR=$(EPOCROOT)epoc32/include
ICONTARGETFILENAME=$(TARGETDIR)/musichomescreenicons.mif
HEADERFILENAME=$(HEADERDIR)/musichomescreenicons.mbg

MAKMAKE : ;

BLD : ;

CLEAN : ;

LIB : ;

CLEANLIB : ;

RESOURCE : 
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
		/Ficonlist.txt

FREEZE : ;

SAVESPACE : ;

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : ;