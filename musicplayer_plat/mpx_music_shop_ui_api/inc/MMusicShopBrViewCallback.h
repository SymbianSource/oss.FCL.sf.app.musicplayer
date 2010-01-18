/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MusicShop Browser  view callback provider.
*
*/


#ifndef M_MUSICSHOP_BR_VIEW_CALLBACK_H
#define M_MUSICSHOP_BR_VIEW_CALLBACK_H

// INCLUDE FILES
#include <MusicShopExternalInterface.h>


// CLASS DECLARATION

/**
* Interface to give callback to browser view.
*
* @lib MusicShopLib.lib
* @since S60 3.2.3
*/
class MMusicShopBrViewCallback
    {

        /**
        * Activate the MusicShop application view based on view id
        *
        * @since S60 3.2.3
        * @param aViewId view id,
        *                KUidMusBrViewId for browser  view,
        *                KUidMusBmViewId for bookmark view.
        */
        virtual void ActivateMusicShopViewL (TUid aViewId) = 0;

    };

#endif //M_MUSICSHOP_BR_VIEW_CALLBACK_H

    // End of File

