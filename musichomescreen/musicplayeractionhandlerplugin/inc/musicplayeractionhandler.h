/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Music Player Action Handler Plugin
*
*/


#ifndef C_MUSICPLAYERACTIONHANDLER_H
#define C_MUSICPLAYERACTIONHANDLER_H

#include <ahplugin.h>

// FORWARD DECLARATION
class CLiwMap;
class TLiwVariant;
class MMPXPlaybackUtility;

/**
 *  Music Player Action Handler Plugin.
 *  Class extends the CAHPlugin in order to be used as a plugin for
 *  action handler.
 *  @since S60 S60 v5.0
 */
NONSHARABLE_CLASS( CMusicPlayerActionHandler ) : public CAHPlugin
    {
public:

    /**
     * Standard Symbian 2 phase constructor
     */
    static CMusicPlayerActionHandler* NewL();

    /**
     * Standard Symbian 2 phase constructor
     */
    static CMusicPlayerActionHandler* NewLC();

    /**
     * Standard C++ destructor.
     */
    ~CMusicPlayerActionHandler();

// from base class CAHPlugin

    /**
     * From CAHPlugin
     * This is called by the AH to execute an action 
     * @param aMap an action object to be executed
     * @return Error code
     */
    TInt ExecuteActionL( const CLiwMap* aMap );

private:
    /**
     * Standard Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Executes Playback command on the current player.
     * @param aMap an action object to be executed
     * @return status code
     */
    TInt ExecutePlaybackCommandL( const CLiwMap* aMap );

    /**
     * Sends a message to Music Player.
     * @param aMap an action object to be executed
     * @return status code
     */
    TInt ExecuteMessageToMusicPlayerL( const CLiwMap* aMap );

    /**
     * goes to now-playing view
     * @return status code
     */
    TInt GoToNowPlayingL();
    
    /**
     * goes to The last Played song.
     * @param aMinimized indicates if app should be in the background.
     * @return status code
     */
    TInt GoToLastPlayedL(TBool aMinimazed);
    
    /**
     * goes to library/collection view
     * @return status code
     */
    TInt GoToMusicLibraryL();
    
    /**
     * Gets a descriptor from the map.
     * @param aMap containing the data.
     * @param aString where the data is extracted to.
     * @param aMapName element of the map that is to be extracted.
     * @return status code
     */
    TInt ExtractDesL( const CLiwMap* aMap, RBuf& aString,
            const TDesC8& aMapName );

    /**
     * Gets a variant from the map.
     * @param aMap containing the data.
     * @param aVariant where the data is extracted to.
     * @param aMapName element of the map that is to be extracted.
     * @return status code
     */
    TInt ExtractVariantL( const CLiwMap* aMap, TLiwVariant& aVariant,
            const TDesC8& aMapName );

private: // data

    /**
     * The playback utility where playback operations are performed.
     * Own.
     */
    MMPXPlaybackUtility* iPlaybackUtility;
    };

#endif /* C_MUSICPLAYERACTIONHANDLER_H */

//  End of File
