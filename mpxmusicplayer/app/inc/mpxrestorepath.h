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
* Description:  Saves/restores current playback path
*
*/


#ifndef CMPXRESTOREPATH_H
#define CMPXRESTOREPATH_H

// INCLUDES
#include <e32base.h>
#include <mpxcollectionobserver.h>
#include <mpxplaybackobserver.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CMPXAppUi;
class CMPXCollectionPath;
class MMPXCollectionUtility;
class MMPXPlaybackUtility;
class MMPXCollectionUiHelper;
class CMPXCollectionOpenUtility;

// CLASS DECLARATION
/**
 * Saves/restores current playback path.
 *
 *  @lib mpx.exe
 *  @since 3.1
 */
class CMPXRestorePath : public CBase,
                        public MMPXCollectionObserver,
                        public MMPXPlaybackObserver
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @since 3.1
         * @param aPlaybackUtility Playback utility to use.
         * @param aCollectionUiHelper UI helper to use
         * @return Pointer to newly created object.
         */
        static CMPXRestorePath * NewL(MMPXPlaybackUtility* aPlaybackUtility,
                                      MMPXCollectionUiHelper* aCollectionUiHelper);

        /**
         * Two-phased constructor.
         *
         * @since 3.1
         * @param aPlaybackUtility Playback utility to use.
         * @param aCollectionUiHelper UI helper to use
         * @return Pointer to newly created object.
         */
        static CMPXRestorePath * NewLC(MMPXPlaybackUtility* aPlaybackUtility,
                                       MMPXCollectionUiHelper* aCollectionUiHelper);

        /**
         * Destructor.
         */
        virtual ~CMPXRestorePath();

    public: // New funtions

        /**
         *  Loads current collection path
         */
        void LoadPathL();

        /**
         * Loads the default collection path
         * @param some delay to load the path (in ms)
         */
        void LoadDefaultPathL( TInt aDelay = 0 );

        /**
         * Save the current collection Path
         */
        void SavePathL();

        /**
         * Return if the default playlist has been initialized
         *
         * @return ETrue if the path has been initialized, EFalse otherwise;
         */
        TBool Initialized();

        /**
         * Return if the playlist is currently initializing
         *
         * @return ETrue it's currently initializing, EFalse otherwise;
         */
        TBool IsInitializing();

        /**
         * Set the initialized state
         *
         * @param aInit The current state of initialization.
         */
        void SetInitialized( TBool aInit );

        /**
         * Cancels the initialize operation
         */
        void CancelInit();
        
        /**
         * Get the Uid of the root path 
         */
        TUid RooPathId();

    private:

        /**
         * C++ Constructor
         *
         * @param aPlaybackUtility Playback utility to use.
         * @param aCollectionUiHelper collection ui helper to use
         */
        CMPXRestorePath (MMPXPlaybackUtility* aPlaybackUtility,
                         MMPXCollectionUiHelper* aCollectionUiHelper);

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    // from base class MMPXCollectionObserver

        /**
         * From MMPXCollectionObserver
         * Handle media properties.
         *
         * @since 3.1
         * @param aMedia  media properties.
         * @param aError Error code.
         */
        void HandleCollectionMediaL(
            const CMPXMedia& aMedia,
            TInt aError );

        /**
         * From MMPXCollectionObserver
         * Handle collection message.
         *
         * @since 3.1
         * @param aMessage Collection message
         * @param aErr system error code.
         */
        void HandleCollectionMessage(
            CMPXMessage* aMessage, TInt aError );

        /**
         * From MMPXCollectionObserver
         * Handles the collection entries being opened.
         *
         * @since 3.1
         * @param aEntries Collection entries opened.
         * @param aIndex Focused entry.
         * @param aComplete ETrue no more entries. EFalse more entries
         *                  expected.
         * @param aError Error code.
         */
        void HandleOpenL(
            const CMPXMedia& aEntries,
            TInt aIndex,
            TBool aComplete,
            TInt aError );

        /**
         * From MMPXCollectionObserver
         * Handles the item being opened.
         *
         * @since 3.1
         * @param aPlaylist Collection playlist, owner ship is transfered.
         * @param aError Error code.
         */
        void HandleOpenL(
            const CMPXCollectionPlaylist& aPlaylist,
            TInt aError );

        /**
         * From MMPXPlaybackObserver
         * Handle playback message
         *
         * @param aMessage playback message
         * @param aErr system error code.
         */
        void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError );

    private:  // New functions

        /**
         * Initialize playback engine with the path provided
         *
         * @param aPath The collection path to initialize playback engine with.
         */
        void InitPlaybackEngineL( CMPXCollectionPath& aPath );

        /**
         * Handle playback message
         *
         * @param aMessage playback message
         */
        void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );

        /**
         * Handle collection message
         *
         * @param aMessage collection message
         */
        void DoHandleCollectionMessageL( const CMPXMessage& aMessage );

        /**
        * Start the incremental open operation at a particular index
        * @param aPath path to open
        * @param aIndex index to start reading from
        * @param aDelay delay to start the operation (in ms)
        */
        void DoIncrementalOpenL( CMPXCollectionPath& aPath, TInt aIndex,
                                 TInt aDelay = 0 );

    private:

        MMPXCollectionUtility* iCollectionUtility; // owned
        MMPXPlaybackUtility* iPlaybackUtility;  // not owned
        MMPXCollectionUiHelper* iCollectionUiHelper;  // not owned
        CMPXCollectionOpenUtility* iIncOpenUtil;

        TBool iPathInitialized;
        TBool iIsInitializing;
        TBool iInitCanceled;
        TBool iDefaultPath;      // Are we initializing the default path?
        TMPXItemId iSelectionId; // Item we want to play
        TInt  iSelectionIndex; // Index we are restoring to
        TBool iLoadSavedPath;    // Load a saved path or load a default path?
        TBool iUSBOngoing;       // is usb ongoing
        TUid iRootPathId;
        TBool iMTPStarted;      // is MTP started
    };

#endif // CMPXRESTOREPATH_H

// End of File

