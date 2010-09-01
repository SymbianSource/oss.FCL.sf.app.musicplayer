/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Music Player domain CenRep keys.
*
*/



#ifndef MUSICPLAYERDOMAINCRKEYS_H
#define MUSICPLAYERDOMAINCRKEYS_H

// Audio ApplicationFeatures API

const TUid KCRUidMusicPlayerFeatures = {0x101F880D};

// Block non-DRM protected content playback
const TUint32 KRequireDRMInPlayback        = 0x00000002;

// Space separated list of audio formats (MIME-types) to block,
// when KRequireDRMInPlayback is enabled
const TUint32 KPlaybackRestrictedMimeTypes = 0x00000003;

#endif      // MUSICPLAYERDOMAINCRKEYS_H


// End of File
