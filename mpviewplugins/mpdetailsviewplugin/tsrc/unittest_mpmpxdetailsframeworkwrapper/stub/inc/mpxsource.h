/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: mpxplaybackutility stub for testing mpmpxframeworkwrapper
*
*/


#ifndef MPXSOURCE_H
#define MPXSOURCE_H

#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>

class CMPXCollectionPlaylist;
class MMPXPlaybackCallback;
class TMPXAttribute;
class CMPXAttributeSpecs;

class MMPXSource
{
public:
    MMPXSource();
    ~MMPXSource();
    
public: // MMPXSource    
    CMPXCollectionPlaylist* PlaylistL();
    RFile* FileL();
    HBufC* UriL();
    void MediaL(const TArray<TMPXAttribute>& aAttrs, MMPXPlaybackCallback& aCallback);
    void MediaL(const TArray<TMPXAttribute>& aAttrs, MMPXPlaybackCallback& aCallback,
                            CMPXAttributeSpecs* aSpecs);    
};
    
#endif // MPXSOURCE_H
