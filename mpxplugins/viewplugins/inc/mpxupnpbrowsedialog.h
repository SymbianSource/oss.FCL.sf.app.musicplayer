/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of UPnP players browse Dialog
*
*/


#ifndef C_CMPXUPNPBROWSEDIALOG_H__
#define C_CMPXUPNPBROWSEDIALOG_H__

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <AknDialog.h>
#include <mpxplaybackobserver.h>
#include <AknsItemID.h>

// FORWARD DECLARATIONS
class MMPXPlaybackUtility;
class CAknPopupList;
class CAknSinglePopupMenuStyleListBox;
class CMPXPlayerManagerStub;
class CAknIconArray;
class MAknsSkinInstance;
class CMPXMedia;
class MMPXViewUtility;

/**
* Device selection dialog class of Common UI library
*/
class CMPXUPnPBrowseDialog: public CAknDialog, public MMPXPlaybackCallback,
                            public MMPXPlaybackObserver
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMPXUPnPBrowseDialog* NewL(TInt aPlayerUid);
        
        /**
        * Destructor.
        */
        virtual ~CMPXUPnPBrowseDialog();

    public: // Methods from MMPXPlaybackCallback

        /**
        *  Handle playback property
        *
        *  @param aProperty the property
        *  @param aValue the value of the property
        *  @param aError error code
        */
        void HandlePropertyL(TMPXPlaybackProperty aProperty,
                                     TInt aValue, 
                                     TInt aError);
        
        /**
        *  Method is called continously until aComplete=ETrue, signifying that 
        *  it is done and there will be no more callbacks
        *  Only new items are passed each time
        *
        *  @param aPlayer UID of the subplayer
        *  @param aSubPlayers a list of sub players
        *  @param aComplete ETrue no more sub players. EFalse more subplayer
        *                   expected
        *  @param aError error code
        */
        void HandleSubPlayerNamesL(TUid aPlayer, 
                                           const MDesCArray* aSubPlayers,
                                           TBool aComplete,
                                           TInt aError);
        
        /**
        *  Call back of media request
        *  Notes: The client is responsible for delete the object of aProperties
        *
        *  @param aMedia media 
        *  @param aError error code    
        */
        void HandleMediaL( const CMPXMedia& aProperties,
                                               TInt aError);   
                                               
        /**
         * From MMPXPlaybackObserver
         * Handle playback message
         * 
         * @param aMessage playback message
         * @param aErr system error code.
         */
        void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError );

    public: // New functions

        /**
         * Displays selection pop up
         *
         * @since Series 60 3.1
         * @param aPlayerUid, selected player Uid
         */
        IMPORT_C TInt ExecuteLD(TInt aPlayerUid);

    private:

        /**
        * C++ default constructor.
        */
        CMPXUPnPBrowseDialog(TInt aPlayerUid);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:    // New Functions

        /**
        * Loads a possibly skinned icon and adds it to icon array
        * @since Series 60 3.0
        * @param CAknIconArray, array of icons
        * @param MAknsSkinInstance, skin instance
        * @param TDesC, reference to icon file
        * @param TAknsItemID, skinned icon id
        * @param TInt, bitmap id
        * @param TInt, bitmap mask id
        */
        void AppendIconToArrayL(CAknIconArray* aArray,
                                MAknsSkinInstance* aSkin,
                                const TDesC& aMbmFile,
                                const TAknsItemID& aID,
                                TInt aBitmapId,
                                TInt aMaskId);

        /**
        * Loads listbox items
        * @since Series 60 3.0
        */
        void LoadListItemsL(void);
        
        /**
        * Loads listbox items
        * @since Series 60 3.0
        */
        void DisplaySubPlayersNamesL( const MDesCArray* aSubPlayers,
                                      const TBool aComplete );

        /**
        * From CAknDialog, handles layout initialization
        * @param const TDesC&, dialog title
        */
        void PreLayoutDynInitL();

        /**
         * Starts selection popup
         *
         * @since Series 60 3.1
         * @param CUPnPDevice, selected device
         */
        TInt StartPopupL();

        /**
         * Creates a selection popup.
         *
         * @since Series 60 3.1
         * @param const TDesC&, popup title
         * @return None
         */
        void CreatePopupL();

        /**
         * Retrieves the name and other info of the currently
         * used subplayer
         */
        void RetrieveCurrentlySelectSubplayerNameL();    

        /**
         * Display an error note after the selected player was not valid
         * @param aResourceId ID of the text to be displayed on the error note
         */
        void DisplayErrorNoteL( TInt aResourceId );     
        
        /**
         * Displays the waiting text during retrieval of subplayer names
         */
        void DisplayWaitingTextL();
        
        /**
         *  Handle playback message
         *
         *  @param aMessage playback message
         */
        void DoHandlePlaybackMessageL( 
            const CMPXMessage& aMessage );

    private:

        CAknPopupList*                   iPopup;
        CAknSinglePopupMenuStyleListBox* iListBox;
        TInt                             iPlayerUid;
        MMPXPlaybackUtility* iPlaybackUtility;  // not owned
        TInt                 iResourceOffset;
        TInt                 iErrorResourceOffset;
        CMPXPlayerManagerStub* iPlayerManagerStub;
        HBufC* iCurrentlySelectedSubPlayerName;
        TUid iCurrentlyUsedPlayer;
        
        CDesCArrayFlat* iSubPlayersCachedArray;
        
        // View utility.
        MMPXViewUtility* iViewUtility;
        
        // ETrue if PlayerUnavailable event was received
        TBool iPlayerUnavailableError;
    };

#endif // C_CMPXUPNPBROWSEDIALOG_H__

// End of file
