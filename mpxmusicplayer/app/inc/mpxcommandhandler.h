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
* Description:  Handles commands passed in by other appilcations
*
*/


#ifndef CMPXCOMMANDHANDLER_H
#define CMPXCOMMANDHANDLER_H

// INCLUDES
#include <e32base.h>
#include <mpxcollectionobserver.h>
#include <mpxplaybackobserver.h>

// FORWARD DECLARATIONS
class CMPXCollectionPath;
class MMPXCollectionUtility;
class MMPXPlaybackUtility;
class MMPXViewUtility;
class CMPXParameter;

// CLASS DECLARATION
/**
 * Process commands passed in by other applications.
 *
 *  @lib mpx.exe
 *  @since 3.1
 */
class CMPXCommandHandler : public CBase,
                           public MMPXCollectionObserver,
                           public MMPXPlaybackObserver
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @since 3.1
         * @param aPlaybackUtility Playback utility to use.
         * @param aCollectionMode The collection mode for current music player
         * @param aParameter The command to process.
         * @return Pointer to newly created object.
         */
        static CMPXCommandHandler * NewL(
            MMPXPlaybackUtility* aPlaybackUtility,
            TUid aCollectionMode,
            const TDesC8& aParameter );

        /**
         * Two-phased constructor.
         *
         * @since 3.1
         * @param aPlaybackUtility Playback utility to use.
         * @param aCollectionMode The collection mode for current music player
         * @param aParameter The command to process.
         * @return Pointer to newly created object.
         */
        static CMPXCommandHandler * NewLC(
            MMPXPlaybackUtility* aPlaybackUtility,
            TUid aCollectionMode,
            const TDesC8& aParameter );

        /**
         * Destructor.
         */
        virtual ~CMPXCommandHandler();

        /**
         * Start processing the command
         *
         * @param aIsUsbInUse Current USB state, ETrue if it's in use,
         *                    EFalse otherwise
         * @param aRefreshing Current refresh state, ETrue if refresh ongoing,
         *                    EFalse otherwise
         */
        void ProcessMessageL( TBool aIsUsbInUse, TBool aRefreshing, TBool aFormattingOnGoing );

        /**
         * Returns if a path has been inited in playback engine
         *
         * @return ETrue if a path has been inited, EFalse otherwise.
         */
        TBool IsPathInited();

        /**
         * Set current USB state
         *
         * @param aIsUsbInUse Current USB state, ETrue if it's in use,
         *                    EFalse otherwise
         */
        void SetUsbInUse( TBool aIsUsbInUse );

        /**
         * Set current refresh state
         *
         * @param aRefreshing Current refresh state, ETrue if refresh ongoing,
         *                    EFalse otherwise
         */
        void SetRefreshState( TBool aRefreshing );

        /**
         * Determines if the command should be handled by standalone mode.
         * This does not forward the command, it just returns if it should
         *
         * @return ETrue if the command should be forwarded, EFalse otherwise
         */
        TBool ForwardCmdToStandAloneMode();

        /**
         * Determines if the command should be handled by now playing instance
         * This does not forward the command, it just returns if it should
         *
         * @return ETrue if the command should be forwarded, EFalse otherwise
         */
        TBool ForwardCmdToNowPlaying();

        /**
         * Checks if initialize playback engine is needed for current command
         *
         * @return ETrue if init is needed, EFalse otherwise
         */
        TBool IsInitPlaybackEngineNeeded();

        /**
         * Extracts the activated view type from the command
         *
         * @return Activated view type ID
         */
        TInt ActivatedViewTypeId();

    private:

        /**
         * C++ Constructor
         *
         * @param aPlaybackUtility Playback utility to use.
         */
        CMPXCommandHandler(
            MMPXPlaybackUtility* aPlaybackUtility );

        /**
         * By default Symbian 2nd phase constructor is private.
         *
         * @param aCollectionMode The collection mode for current music player
         * @param aParameter The command to process.
         */
        void ConstructL(
            TUid aCollectionMode,
            const TDesC8& aParameter );

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
         * Start processing the message
         *
         */
        void DoHandleMessageL();

        /**
         * Extracts the message from the paramter provided.  The message
         * is internalized to iParameter
         *
         * @param aParameter Parameter to extract the message from.
         */
        void ExtractMessageL( const TDesC8& aParameter );

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

    private: // enums

        enum TMPXCollectionPlaylistSetupState
            {
            TMPXCollectionPlaylistSetupStateIdle,
            TMPXCollectionPlaylistSetupStateOpenPath,
            TMPXCollectionPlaylistSetupStateInitPlaybackEngine
            };

    private:

        MMPXPlaybackUtility* iPlaybackUtility; // Not owned

        MMPXCollectionUtility* iMusicPlayerCollectionUtility; //owned

        MMPXViewUtility* iViewUtility; // owned

        CMPXCollectionOpenUtility* iIsolatedIncOpenUtil; // owned

        CMPXParameter* iParameter; //owned

        TBool iUnhandledMessage;

        TBool iPathInited;

        TBool iUsbInUse;

        TBool iRefreshOngoing;
        TBool iLastPlayedAutostart;

        TMPXCollectionPlaylistSetupState iCollectionPlaylistState;
    };

#endif // CMPXCOMMANDHANDLER_H

// End of File

