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
* Description:  album art events observer
*
*/

 

#ifndef MMPXALBUMARTEVENTOBSERVER_H
#define MMPXALBUMARTEVENTOBSERVER_H


enum TMPXAlbumArtEvents
    {
    EMPXAlbumArtEventExtractCompleted
    };

/**
*  Interface for getting event callbacks for album art events 
*/
NONSHARABLE_CLASS( MMPXAlbumArtEventObserver )
    {
public:
    /**
    * Handle album art events
    *
    * @param aEvent album art event
    * @param aError error code
    */
    virtual void HandleAlbumArtEventL( 
        TMPXAlbumArtEvents aEvent, TInt aError ) = 0;   
    };    
    
#endif // MMPXALBUMARTEVENTOBSERVER_H
