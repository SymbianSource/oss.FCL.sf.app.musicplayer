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
# Description: Unit test for mpdetailsviewplugin
#


TEMPLATE = subdirs

## TODO: fix this pro file, unit tests in their own root compile, but not within this pro file
##SUBDIRS += ./unittest_mpdetailsview
##SUBDIRS += ./unittest_mpdetailsviewplugin
##SUBDIRS += ./unittest_mpmpxdetailsframeworkwrapper
##SUBDIRS += ./unittest_mpsongdata

test.depends = first
test.CONFIG += recursive
autotest.depends = first
autotest.CONFIG += recursive
QMAKE_EXTRA_TARGETS += test autotest


