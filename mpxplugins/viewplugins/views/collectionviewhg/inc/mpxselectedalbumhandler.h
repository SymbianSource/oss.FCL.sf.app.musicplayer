/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Selected album handler
*
*/

#ifndef MPXSELECTEDALBUMHANDLER_H_
#define MPXSELECTEDALBUMHANDLER_H_

class CMPXMedia;


NONSHARABLE_CLASS (MMPXSelectedAlbumHandler)
    {
public:

    /**
    *  Saves selected album.
    *
    *  @since S60 5.2
    *  @param aMedia Album to be stored
    */
    virtual void SaveSelectedAlbumL (CMPXMedia &aMedia) = 0;

    /**
    *  Restores selected album.
    *
    *  @since S60 5.2
    *  @param aMedia Album to be restored
    */
    virtual const CMPXMedia* RestoreSelectedAlbum () = 0;
    };


#endif // MPXSELECTEDALBUMHANDLER_H_

// End of File