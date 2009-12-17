/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of metadata handler
*
*/



#ifndef C_CMPXMETADATAHANDLERIMP_H
#define C_CMPXMETADATAHANDLERIMP_H

// INCLUDES
#include <mpxplaybackobserver.h>
#include <mpxplaybackframeworkdefs.h>
#include <mpxcollectionplaylistobserver.h>

#include <remconmediainformationtarget.h>
#include <playerinformationtargetobserver.h>
#include <remcongroupnavigationtargetobserver.h>

// FORWARD DECLARATIONS
class CRemConInterfaceSelector;
class CRepository;
class MMPXPlaybackUtility;
class MPlayerCapabilitiesObserver;
class MPlayerApplicationSettingsObserver;
class MPlayerEventsObserver;
class CRemConGroupNavigationApiTarget;
class CPlayerInfoTarget;
class CEqualizerPresetChangeListener;


/**
 *  Observer class for equalizer preset changes.
 *
 *  @lib mpxmetadatahandler.lib
 *  @since S60 v5.1
 */
class MEqualizerPresetChangeListenerObserver
    {
public:

    /**
     * Callback for receiving changes in the equalizer preset settings.
     *
     * @since S60 v5.1
     * @param aNewPreset The ID of the new equalizer preset
     */
    virtual void EqualizerPresetChangedL(TInt aNewPreset) = 0;

    };


/**
 *  AVRCP 1.3 metadata handler implementation class
 *
 *  @lib mpxmetadatahandler.lib
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CMPXMetaDataHandlerImp ): public CBase,
											 public MMPXMetaDataHandler,
											 public MMPXPlaybackObserver,
											 public MMPXPlaybackCallback,
											 public MPlayerApplicationSettingsNotify,
											 public MRemConMediaInformationTargetObserver,
											 public MRemConGroupNavigationTargetObserver,
											 public MMPXCollectionPlaylistObserver,
											 public MEqualizerPresetChangeListenerObserver
	{
public:

	/**
	 * Two-phased constructor.
	 *
	 * @since v5.1
	 * @param aInterfaceSelector RemCon interface selector.
	 * @return Pointer to newly created object.
	 */
	static MMPXMetaDataHandler* NewL(CRemConInterfaceSelector &aInterfaceSelector);

	/**
	 * Destructor.
	 */
	virtual ~CMPXMetaDataHandlerImp();

private:

	/**
	 * C++ default constructor.
	 *
     * @since S60 v5.1
	 */
	CMPXMetaDataHandlerImp();

	/**
	 * By default Symbian 2nd phase constructor is private.
	 * 
     * @since S60 v5.1
     * @param aInterfaceSelector RemCon interface selector.
	 */
	void ConstructL(CRemConInterfaceSelector &aInterfaceSelector);

	/**
	 * Handle playback message
	 *
     * @since S60 v5.1
	 * @param aMessage playback message
	 */
	void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );

	/**
	 * Handle playback property
	 *
     * @since S60 v5.1
	 * @param aProperty the property
	 * @param aValue the value of the property
	 * @param aError error code
	 */
	void DoHandlePropertyL( TMPXPlaybackProperty aProperty, TInt aValue, TInt aError );

	/**
	 * Handle media properties.
	 *
     * @since S60 v5.1
	 * @param aMedia media properties
	 * @param aError error code
	 */
	void DoHandleMediaL( const CMPXMedia& aMedia, TInt aError );

	/**
	 * Handle playback state changed.
	 *
     * @since S60 v5.1
	 * @param aState New Playback state
	 */
	void DoHandleStateChangedL( TMPXPlaybackState aState );

// from base class MMPXPlaybackObserver

	/**
	 * From MMPXPlaybackObserver
	 * Handle playback message
	 *
     * @since S60 v5.1
	 * @param aMessage playback message
	 * @param aErr system error code.
	 */
	void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError );

// from base class MMPXPlaybackCallback

	/**
	 * From MMPXPlaybackCallback
	 * Handle playback property
	 *
     * @since S60 v5.1
	 * @param aProperty the property
	 * @param aValue the value of the property
	 * @param aError error code
	 */
	void HandlePropertyL( TMPXPlaybackProperty aProperty, TInt aValue, TInt aError );

	/**
	 * From MMPXPlaybackCallback
	 * Method is called continously until aComplete=ETrue, signifying that
	 * it is done and there will be no more callbacks
	 * Only new items are passed each time
	 *
     * @since S60 v5.1
	 * @param aPlayer UID of the subplayer
	 * @param aSubPlayers a list of sub players
	 * @param aComplete ETrue no more sub players. EFalse more subplayer expected
	 * @param aError error code
	 */
	void HandleSubPlayerNamesL( TUid aPlayer, const MDesCArray* aSubPlayers, 
	                            TBool aComplete, TInt aError );

	/**
	 * From MMPXPlaybackCallback
	 * Handle media properties
	 *
     * @since S60 v5.1
	 * @param aMedia media
	 * @param aError error code
	 */
	void HandleMediaL( const CMPXMedia& aMedia, TInt aError );

// From base class MMPXCollectionPlaylistObserver

    /**
     * From MMPXCollectionPlaylistObserver
     * Handle collection playlist change.
     *
     * @since S60 v5.1
     * @param aError KErrNotFound - Playlist is updated, current item removed
     *               KErrNone - Playlist is updated, current item is valid
     *               KErrEof - Playlist is updated, current item removed and
     *                          reached to the end of playlist
     */
	void HandleCollectionPlaylistChange(TInt aError);

// From base class MPlayerApplicationSettingsNotify

	/**
	 * From MPlayerApplicationSettingsNotify
	 * This is called when the controller has changed a setting
	 *
     * @since S60 v5.1
	 * @param aAttributeID A list of attribute IDs whose value has changed.
	 * @param aAttributeValue A list of new values for the attributes listed in aAttributeID.
	 */
	virtual void MpasnSetPlayerApplicationValueL(const RArray<TInt>& aAttributeID, const RArray<TInt>& aAttributeValue);

// From base class MRemConMediaInformationTargetObserver

	/**
	 * From MRemConMediaInformationTargetObserver
	 * For each element in aAttributeList the client should respond by calling
	 * CRemConMediaInformationTarget::AttributeValue(). After all attributes have
	 * been supplied the client should call CRemConMediaInformationTarget::Completed().
	 *
     * @since S60 v5.1
	 * @param aAttributeList A list of TAttributeID requested by the controller
	 */
	virtual void MrcmitoGetCurrentlyPlayingMetadata( TMediaAttributeIter& aAttributeIter );

	/**
	 * From MRemConGroupNavigationTargetObserver
	 * Clients must implement this interface in order to instantiate objects of type
	 * CRemConGroupNavigation. This interface passes incoming commands from RemCon to
	 * the client.
	 * A 'Next Group' has been received.
	 * 
     * @since S60 v5.1
	 * @param aButtonAct The button action associated with the command.
	 */
	virtual void MrcgntoNextGroup(TRemConCoreApiButtonAction aButtonAct);

	/**
	 * From MRemConGroupNavigationTargetObserver
	 * For each element in aAttributeList the client should respond by calling
	 * CRemConMediaInformationTarget::AttributeValue(). After all attributes have
	 * been supplied the client should call CRemConMediaInformationTarget::Completed().
	 * A 'Previous Group' has been received.
	 * 
     * @since S60 v5.1
	 * @param aButtonAct The button action associated with the command.
	 */
	virtual void MrcgntoPreviousGroup(TRemConCoreApiButtonAction aButtonAct);

// From base class MEqualizerPresetChangeListenerObserver

    /**
     * From MEqualizerPresetChangeListenerObserver
     * Callback for receiving changes in the equalizer preset settings.
     *
     * @since S60 v5.1
     * @param aNewPreset The ID of the new equalizer preset
     */
	virtual void EqualizerPresetChangedL(TInt aNewPreset);

private:    // Data

    /**
     * The current player state (playing, paused, stopped).
     */
    TMPXPlaybackState iPlayerState;

    /**
     * Current track's position info (in seconds)
     */
    TInt iPlaybackPosition;

    /**
     * Current track's playing time info (in milliseconds)
     */
    TInt iPlayingTime;

    /**
     * Current track number
     */
    TInt iTrackNumber;

    /**
     * Last selected equalizer preset settings
     */
    TInt iLastEqPresetId;

	/**
	 * Current track's title info
     * Own.
	 */
	HBufC* iTrackTitle;

    /**
     * Current track's artist info
     * Own.
     */
	HBufC* iArtist;

	/**
     * Current track's album info
     * Own.
     */
	HBufC* iAlbum;

	/**
     * Current track's genre info
     * Own.
     */
	HBufC* iGenre;
	
	/**
     * Current track's unique id info
     * Own.
     */
	
	TUid iColId;

    /**
     * Observer for equalizer preset changes
     * Own.
     */
    CEqualizerPresetChangeListener* iEqPresetListener;

    /**
     * Main interface to the player.
     * Not own.
     */
    MMPXPlaybackUtility* iPlaybackUtility;

    /**
     * Container for player information responses
     * Not own.
     */
    CPlayerInfoTarget* iPlayerInformationTarget;

    /**
     * Observer of this player's capabilities.
     * Not own.
     */
	MPlayerCapabilitiesObserver* iPlayerCapabilitiesObserver;

    /**
     * Observer of this player's settings.
     * Not own.
     */
	MPlayerApplicationSettingsObserver* iPlayerApplicationSettingsObserver;

	/**
     * Observer of player events.
     * Not own.
     */
	MPlayerEventsObserver* iPlayerEventsObserver;

	/**
     * Interface for sending media information
     * Not own.
     */
	CRemConMediaInformationTarget* iMediaInfoTarget;

	/**
     * Interface for group navigation responses
     * Not own.
     */
	CRemConGroupNavigationApiTarget* iGroupNavigationTarget;

	};


/**
 *  Helper class for receiving equalizer preset changed events.
 *
 *  @lib mpxmetadatahandler.lib
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS(CEqualizerPresetChangeListener): public CActive
	{
public:

	/**
	 * Two-phased constructor.
	 *
	 * @since v5.1
	 * @param aFlags Flags for creating popups.
	 * @param aObserver Custom command observer for media key handling.
	 * @return Pointer to newly created object.
	 */
	static CEqualizerPresetChangeListener* NewL(MEqualizerPresetChangeListenerObserver& aObserver);

	/**
	 * Destructor.
	 */
	virtual ~CEqualizerPresetChangeListener();

    /**
     * Get the current equalizer preset ID
     *
     * @since S60 v5.1
     * @return The current preset ID
     */
	TInt GetCurrentPresetL();

    /**
     * Set the equalizer preset
     *
     * @since S60 v5.1
     * @param aNewPreset The ID of the preset to be activated
     */
	void ChangePresetL(TInt aNewPreset);

    /**
     * Start listening to equalizer preset changes
     *
     * @since S60 v5.1
     */
	void StartL();

	/**
     * Stop listening to equalizer preset changes
     *
     * @since S60 v5.1
     * @return The current preset ID
     */
	void Stop();

private:

	/**
	 * C++ default constructor.
	 *
     * @since S60 v5.1
	 * @param aObserver Client for receiving equalizer preset changed events
	 */
	CEqualizerPresetChangeListener(MEqualizerPresetChangeListenerObserver& aObserver);

	/**
     * Symbian 2nd-phase constructor
     *
     * @since S60 v5.1
	 */
	void ConstructL();

// from base class CActive

    /**
     * From CActive.
     * Called by the active scheduler when the request has been cancelled.
     *
     * @since S60 v5.1
     */
    virtual void DoCancel();
    
    /**
     * From CActive.
     * Called by the active scheduler when the request has been completed.
     *
     * @since S60 v5.1
     */
    virtual void RunL();

    /**
     * From CActive.
     * Called by the active scheduler when an error in RunL has occurred.
     *
     * @since S60 v5.1
     */
    TInt RunError( TInt aError );

private:    // Data

    /**
     * Handle to Central Repository
     * Own.
     */
    CRepository* iRepository;

    /**
     * Client for receiving equalizer preset changed events
     * Not own.
     */
    MEqualizerPresetChangeListenerObserver& iObserver;

	};

#endif  // C_CMPXMETADATAHANDLERIMP_H

// End of File
