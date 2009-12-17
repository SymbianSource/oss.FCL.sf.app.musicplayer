#
# Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  rsg export makefile for project
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=\epoc32\data\z
endif

FINAL :
	echo Exporting .rsg files...
	copy \epoc32\include\musichomescreen.rsg $(ZDIR)\resource\apps\musichomescreen.rsg
    
MAKMAKE FREEZE LIB CLEANLIB RESOURCE RELEASABLES CLEAN BLD SAVESPACE :
