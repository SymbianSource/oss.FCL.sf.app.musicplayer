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
* Description:  MPX PodcastCollection view container implementation
*
*/



#ifndef C_CMPXPODCASTCOLLECTIONVIEWCONTAINER_H
#define C_CMPXPODCASTCOLLECTIONVIEWCONTAINER_H


// INCLUDES
#include <mpxcommonframeworkdefs.h> // TMPXItemId
#include <mpxpodcastdefs.h> // Podcast specific
#include "mpxviewcontainer.h"
#include "mpxcommoncontainer.h"

// FORWARD DECLARATION
class TAknsItemID;
class CAknContextPane;
class CMPXCommonContainerFactory;
class MMPXCommonContainer;

// CLASS DECLARATION

/**
 *  MPX Podcast Collection view container.
 *
 *  @lib mpxpodcastcollectionview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXPodcastCollectionViewContainer )
    : public CBase
    , public MMPXViewContainer
    , public MMPXCommonListBoxArrayObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 5.1
     * @param aView Command observer.
     * @param aObserver MEikListBoxObserver
     * @return Pointer to newly created object.
     */
    static CMPXPodcastCollectionViewContainer* NewL(
        MEikCommandObserver* aView,
        MEikListBoxObserver* aObserver );

    /**
     * Destructor.
     */
    virtual ~CMPXPodcastCollectionViewContainer();

    /**
     * Sets playback status with item id
     *
     * @param aId item id of the episode
     * @param aStatus status of the episode
     */
    void SetPlaybackStatusByIdL( TMPXItemId aId, TMPXPlaybackState aStatus );

    /**
     * Get current playback index
     *
     * @return Current playback index, KErrNotFound if nothing is playing
     */
    TInt PlaybackIndex();

    /**
     * Called by podcast collection view to set current view
     * to be used to determine the correct help context
     *
     * @param: the current view
     */
    void SetCurrentView( const TMPXPodcastView& currentView );

    /**
     * Return common container
     *
     * @since 3.2
     */
    MMPXCommonContainer* Common();

// from base class MMPXViewContainer

    /**
     * Handles key events.
     *
     * @param aKeyEvent The key event.
     * @param aType The type of key event.
     */
    TKeyResponse HandleKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );

    /**
     * Handles the retrieval of the view container's help context.
     *
     * @param aContext The control's help context.
     */
    void HandleHelpContext( TCoeHelpContext& aContext ) const;

    /**
     * Returns the indicators for the specified item within the view container
     *
     * @param aIndex specified array index
     * @return Indicator icon indices
     */
    RArray<TInt> IndicatorsL( TInt aIndex );
    
    /**
     * Hides container controls.
     */
    void HideContainerWindow();

private:

    /**
     * C++ default constructor.
     */
    CMPXPodcastCollectionViewContainer(
        MEikCommandObserver* aView,
        MEikListBoxObserver* aObserver);

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Creates listbox array
     *
     */
    CMPXCommonListBoxArrayBase* CreateListBoxArrayL();

// from base class MMPXCommonListBoxArrayObserver

    /**
     * From MMPXCommonListBoxArrayObserver.
     * Handle listbox array events
     *
     *  @param aEvent list box event
     */
    void HandleListBoxArrayEventL(
        MMPXCommonListBoxArrayObserver::TMPXCommonListBoxArrayEvents aEvent );

private: // data member

    MMPXCommonContainer*                   iCommonContainer; // Owned
    MEikCommandObserver*                   iView; // not owned
    MEikListBoxObserver*                   iListBoxObserver; // not owned
    CAknContextPane*                       iContextPane; // Not owned
    CMPXPodcastCollectionViewListBoxArray* iLbxArray; // Not owned
    CEikImage*                             iNewIcon;  // New context icon, owned
    CEikImage*                             iOrigIcon; // New context icon, not owned
    TMPXPodcastView                        iCurrentView; // stores the current view to help with setting
                                                         // help context
    };

#endif  // C_CMPXPODCASTCOLLECTIONVIEWCONTAINER_H

// End of File
