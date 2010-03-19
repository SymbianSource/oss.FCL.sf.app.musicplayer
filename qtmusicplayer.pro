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

TEMPLATE = subdirs

symbian {
    include(app/rom/rom.pri)
    include(musicfetcher/rom/rom.pri)
    include(utilities/mpsettingsmanager/rom/rom.pri)
    include(utilities/mpnowplayingbanner/rom/rom.pri)
    include(mpviewplugins/mpcollectionviewplugin/rom/rom.pri)
    include(mpviewplugins/mpplaybackviewplugin/rom/rom.pri)
    include(mpserviceplugins/audioeffects/rom/rom.pri)
    include(mpserviceplugins/localaudio/rom/rom.pri)
    include(mpserviceplugins/mpxsqlitedbcommon/rom/rom.pri)
    include(mpserviceplugins/mpxsqlitedbplugin/rom/rom.pri)
    include(mpserviceplugins/mpxsqlitepodcastdbplugin/rom/rom.pri)
    include(mpserviceplugins/m3uplaylistplugin/rom/rom.pri)
}

SUBDIRS = utilities \
           musicfetcher \
           app \
           mpviewplugins 

CONFIG += ordered

BLD_INF_RULES.prj_mmpfiles += "./mpserviceplugins/audioeffects/group/mpxaudioeffectengine.mmp" \
"./mpserviceplugins/localaudio/group/mpxlocalaudioplayback.mmp" \
"./mpserviceplugins/mpxsqlitedbcommon/group/mpxsqlitedbcommon.mmp" \
"./mpserviceplugins/mpxsqlitedbplugin/group/mpxsqlitedbplugin.mmp" \
"./mpserviceplugins/mpxsqlitepodcastdbplugin/group/mpxsqlitepodcastdbplugin.mmp" \
"./mpserviceplugins/m3uplaylistplugin/group/mpxm3uplaylistparsers.mmp" \
"./mpserviceplugins/m3uplaylistplugin/group/mpxm3uplaylistplugin.mmp"

