#
# Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  scalable icons makefile for project mpx
#


ifeq (WINS,$(findstring WINS,$(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32/release/$(PLATFORM)/$(CFG)/z
else
ZDIR=$(EPOCROOT)epoc32/data/z
endif

TARGETDIR=$(ZDIR)/resource/apps
HEADERDIR=$(EPOCROOT)epoc32/include
ICONTARGETFILENAME=$(TARGETDIR)/mpx_aif.mbm
HEADERFILENAME=$(HEADERDIR)/mpx.mbg

MAKMAKE : 
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
		/c8,8 qgn_menu_mp_lst.bmp \
		/c8,8 qgn_menu_mp_cxt.bmp

BLD : ;

CLEAN : ;

LIB : ;

CLEANLIB : ;

RESOURCE : ;

FREEZE : ;

SAVESPACE : ;

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : ;
