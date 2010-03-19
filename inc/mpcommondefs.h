/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common definitios.
*
*/



#ifndef MPCOMMONDEFS_H
#define MPCOMMONDEFS_H

namespace MpCommon
{
    // UID definition
    const long int KMusicPlayerUid      = {0x10207C62};
    const long int KCollectionViewUid   = {0x10207C63};
    const long int KPlaybackViewUid     = {0x10207C64};

    /*!
     Command code sent from the views to the application main window
     through command signal.
     */
    enum MpCommandCode {
        Exit,
        ActivateCollectionView,
        ActivatePlaybackView
    };

    /*!
     View's mode.
     DefaultView - Default mode; normal Music Player operation.
     FetchView - Music fetcher mode; launched through QtHighway interface.
     */
    enum MpViewMode {
        DefaultView,
        FetchView
    };
}

#endif	// MPCOMMONDEFS_H
