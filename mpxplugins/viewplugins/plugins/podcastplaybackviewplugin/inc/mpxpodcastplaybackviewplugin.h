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
* Description:  MPX Podcast Playback view plugin definition.
*
*/



#ifndef C_CMPXPODCASTPLAYBACKVIEWPLUGIN_H
#define C_CMPXPODCASTPLAYBACKVIEWPLUGIN_H


// INCLUDES
#include <mpxaknviewplugin.h>


// CLASS DECLARATION

/**
 *  MPX Podcast Playback view plugin definition.
 *
 *  @lib mpxpodcastplaybackviewplugin.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXPodcastPlaybackViewPlugin ) : public CMPXAknViewPlugin
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPodcastPlaybackViewPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CMPXPodcastPlaybackViewPlugin();

private:

    /**
     * From CMPXAknViewPlugin
     * Construct Avkon view.
     *
     * @since S60 v3.0
     * @return Pointer to a newly created Avkon view.
     */
    CAknView* ConstructViewLC();

private:

    /**
     * C++ default constructor.
     */
    CMPXPodcastPlaybackViewPlugin();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif  // C_CMPXPODCASTPLAYBACKVIEWPLUGIN_H

// End of File
