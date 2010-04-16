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

#include "mpxsource.h"
#include "mptrace.h"

MMPXSource::MMPXSource()
{
    TX_ENTRY_ARGS("STUB") 
}

MMPXSource::~MMPXSource()
{
    TX_ENTRY_ARGS("STUB")
}

CMPXCollectionPlaylist* MMPXSource::PlaylistL()
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
    return 0;
}

RFile* MMPXSource::FileL()
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
    return 0;
}

HBufC* MMPXSource::UriL()
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
    return 0;
}

void MMPXSource::MediaL(const TArray<TMPXAttribute>& /*aAttrs*/, MMPXPlaybackCallback& /*aCallback*/)
{
    TX_ENTRY_ARGS("STUB")    
}

void MMPXSource::MediaL(const TArray<TMPXAttribute>& /*aAttrs*/, MMPXPlaybackCallback& /*aCallback*/,
                            CMPXAttributeSpecs* /*aSpecs*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}
