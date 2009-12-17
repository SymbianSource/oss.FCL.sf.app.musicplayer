/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AI ecom plugin class. Main interface to XML AI framework.
*
*/


#ifndef C_AIPLAYERPLUGIN_H
#define C_AIPLAYERPLUGIN_H

// INCLUDES
#include <aicontentpublisher.h>
#include <aicontentobserver.h>
#include <aipropertyextension.h>
#include <aieventhandlerextension.h>

#include "aiplayerpluginengine.h"
#include "aiplayerpluginengineobserver.h"

class MAiPluginSettings;
class MAiContentObserver;
class MAiContentItemIterator;
class CCoeEnv;


/**
 *  Music Player Player Plugin.
 *  This class implements AI ecom plugin. This is the main interface to
 *  the XML AI framework.
 *
 *  @lib aiplayerplugin2.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CAiPlayerPlugin ) : public CAiContentPublisher,
                                       public MAiPropertyExtension,
                                       public MAiEventHandlerExtension,
                                       public MAiPlayerPluginEngineObserver
    {
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CAiPlayerPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CAiPlayerPlugin();

public:

    /**
     * From CAiContentPublisher
     * See aicontentpublisher.h for detailed description.
     * @since S60 5.0
     */
    void Resume( TAiTransitionReason aReason );
    void Suspend( TAiTransitionReason aReason );
    void Stop( TAiTransitionReason aReason );
    void SubscribeL( MAiContentObserver& aObserver );
    void ConfigureL( RAiSettingsItemArray& aSettings );
    TAny* Extension( TUid aUid );

    /**
     * From MAiPropertyExtension
     * See aipropertyextension.h for detailed description.
     * @since S60 5.0
     */
    TAny* GetPropertyL( TInt aProperty );
    void SetPropertyL( TInt aProperty, TAny* aValue );

    /**
     * From MAiEventHandlerExtension
     * See aieventhandlerextension.h for detailed description.
     * @since S60 5.0
     */
    void HandleEvent( TInt aEvent, const TDesC& aParam );

    /**
     * From MAiPlayerPluginEngineObserver
     * See aiplayerpluginengine.h for detailed description.
     * @since S60 5.0
     */
    void PlayerStateChanged( TMPlayerState aState );
    void TrackInfoChanged( const TDesC& aTitle, const TDesC& aArtist );
    void PlaybackPositionChanged( TInt aPosition );
    void VolumeChanged( TInt aVolume );

private:

    CAiPlayerPlugin();
    void ConstructL();

    /**
     * Helper class for HandleEvent received from MAiEventHandlerExtension
     */
    void DoHandleEventL( TInt aEvent, const TDesC& aParam );

    /**
     * Callback function invoked through iCleanTimer upon timeout.
     */
	static TInt ClearL( TAny* aPtr );

private:     // Data

    RPointerArray<MAiContentObserver>   iObservers;
    TAiPublisherInfo                    iInfo;
    MAiContentItemIterator*             iContent;
    MAiContentItemIterator*             iResources;
    MAiContentItemIterator*             iEvents;

    CAiPlayerPluginEngine*              iEngine;
    TMPlayerState          				iState;

    TBuf<128>                           iDuration;
    TBuf<128>                           iElapsedTime;
    HBufC*                              iLongFormatString;
    HBufC*                              iShortFormatString;
    HBufC*                              iUnknownArtistText;
    TInt                                iResourceOffset;

    CCoeEnv*                            iCoeEnv;                // Not owned
    TBool 								iPlayStarted;
    CPeriodic*  						iCleanTimer;
    TInt                                iLastSetVolume;
    };

#endif  // C_AIPLAYERPLUGIN_H


