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
# Description:  generates stub sis for mpx music player
#


TARGETDIR=$(EPOCROOT)EPOC32\Data\Z\System\Install

SISNAME=mpxmusicplayerstub
PKGNAME=mpxmusicplayerstub

$(TARGETDIR) :
	@perl -S emkdir.pl "$(TARGETDIR)"

do_nothing :
	rem do_nothing

SISFILE=$(TARGETDIR)\$(SISNAME).sis

$(SISFILE) : ..\sis\$(PKGNAME).pkg
	makesis -s $? $@ 

#
# The targets invoked by bld...
#

MAKMAKE : do_nothing

RESOURCE : do_nothing

SAVESPACE : do_nothing

BLD : do_nothing

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FINAL : $(TARGETDIR) $(SISFILE)

CLEAN : 
	-erase $(SISFILE)

RELEASABLES : 
	@echo $(SISFILE)
