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


#ifndef CMPXPDCOMMANDHANDLER_H
#define CMPXPDCOMMANDHANDLER_H

// INCLUDES
#include <e32base.h>
#include <mpxplaybackobserver.h>
#include <mpxplaybackframeworkdefs.h>
#include <mpxplaybackcommanddefs.h>

// FORWARD DECLARATIONS
class MMPXPlaybackUtility;
class MMPXViewUtility;
class CMPXParameter;
class CMPXDrmMediaUtility;
class CMPXCommonUiHelper;

// CLASS DECLARATION
/**
 * Process commands passed in by other applications.
 *
 *  @lib mpx.exe
 *  @since 3.1
 */
class CMPXPdCommandHandler : public CBase,
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
        static CMPXPdCommandHandler * NewL( MMPXPlaybackUtility* aPlaybackUtility );

        /**
         * Two-phased constructor.
         *
         * @since 3.1
         * @param aPlaybackUtility Playback utility to use.
         * @param aCollectionMode The collection mode for current music player
         * @param aParameter The command to process.
         * @return Pointer to newly created object.
         */
        static CMPXPdCommandHandler * NewLC( MMPXPlaybackUtility* aPlaybackUtility );

        /**
         * Destructor.
         */
        virtual ~CMPXPdCommandHandler();


        TInt HandleCommandParametersL( const TDesC8& aParameter );

	    /**
	     * Handle media key commands.
	     *
	     * @param aCommand Media key commands. see mpxappui.hrh
	     */
	    void HandleCommandL( TMPXPlaybackCommand aCommand );

        /**
         *  Current state of player
         * 
         *  @since 3.1
         *  @return current PdPlaybackUtility state
         */
        TMPXPlaybackState PdStateL() const;
        
        TBool IsPlaybackCompleted();
        
    private:

        /**
         * C++ Constructor
         *
         */
        CMPXPdCommandHandler(MMPXPlaybackUtility* aPlaybackUtility);

        /**
         * By default Symbian 2nd phase constructor is private.
         *
         */
        void ConstructL();


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
         * Start processing the message
         *
         */
        void DoHandleMessageL();

        /**
         * Handle playback message
         *
         * @param aMessage playback message
         */
        void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );
        
	    /**
	     *  Handle playback state changed.
	     *
	     *  @param aState New Playback state
	     */
	    void DoHandleStateChangedL(
	        TMPXPlaybackState aState );
        

        void ActivatePlaybackViewL();

        void SendCustomCommandToMPXL( TUint aTransactionID, TMPXPlaybackPdCommand aCustomCmd );


    private:

        MMPXPlaybackUtility* iPdPlaybackUtility; // owned
        MMPXPlaybackUtility* iPlaybackUtility; // owned

        MMPXViewUtility* iViewUtility; // owned
		CMPXDrmMediaUtility* iDrmMediaUtility; // owned
		CMPXCommonUiHelper* iCommonUiHelper; // owned
		TBool iPlaybackCompleted;
		TBufC<KMaxFileName> iCurrentFileName;
		TMPXPlaybackState iPrevousPlaybackState;

    };

#endif // CMPXPDCOMMANDHANDLER_H

// End of File

