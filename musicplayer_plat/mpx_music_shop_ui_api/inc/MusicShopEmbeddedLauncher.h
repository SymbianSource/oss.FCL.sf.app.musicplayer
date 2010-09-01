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
* Description: MusicShop embedded launcher
*
*/



#ifndef MUSICSHOP_EMBEDDED_LAUNCHER_H
#define MUSICSHOP_EMBEDDED_LAUNCHER_H

#include <MusicShopExternalInterface.h>
#include <Avkon.rsg>
#include <AknServerApp.h>

class CAknNullService;
class CAknLaunchAppService;
class CGenericParamList;

const TInt KMusDefaultIap = 0; // 0 means application will take the default
                               // internet access point from comms db.

/**
* Launches MusicShop in Embedded mode.
*
* @lib MusicShopLib.lib
* @since S60 3.2.3
*/

NONSHARABLE_CLASS ( CMusicShopEmbeddedLauncher )
    : public CBase,
      public MAknServerAppExitObserver
    {
    public:

        /**
        * First phase constructor,
        *
        * @since S60 v3.2.3
        * @param aCoeEnv Eikon environment
        */
        static IMPORT_C CMusicShopEmbeddedLauncher* NewL(CEikonEnv* aCoeEnv);

        /**
        * Destructor
        *
        * @since S60 v3.2.3
        */
        IMPORT_C virtual ~CMusicShopEmbeddedLauncher();

        /**
        * Activate MusicShop browser view, this API can also
        * be used to send DNL message to already launched MS
        * application.
        *
        * @since S60 v3.2.3
        * @param aUrlLink name of the url
        * @param aDefaultAccessPoint URL access point
        *                            0 to use default AP
        * @param aRskId Right softkey id
        */
        static IMPORT_C void ActivateMusBrViewL(
                        const TDesC& aUrlLink,
                        const TUint aDefaultAccessPoint = KMusDefaultIap,
                        const TInt aRskId = R_AVKON_SOFTKEYS_OPTIONS_BACK);

        /**
        * This will launch the music shop application based on view id
        *
        * @since S60 v3.2.3
        * @param aUrlLink If Url is set to NULL then Bookmark view
        *                 is launched else browser view.
        * @param aDefaultAccessPoint URL access point
        *                            0 to use default AP
        * @param aRskId Right softkey id
        */
        IMPORT_C void LaunchMusicShopL(
                        const TDesC& aUrlLink,
                        const TUint aDefaultAccessPoint = KMusDefaultIap,
                        const TInt aRskId = R_AVKON_SOFTKEYS_OPTIONS_BACK);
    private:
        /**
        * Default Contructor
        *
        * @since S60 v3.2.3
        * @param aCoeEnv Eikon environment
        */
        CMusicShopEmbeddedLauncher(CEikonEnv* aCoeEnv);

        /**
        * Second phase constructor
        *
        * @since S60 v3.2.3
        */
        void ConstructL();

        /**
        * This will launch the music shop application based on view id and URL value
        *
        * @since S60 v3.2.3
        * @param aViewId KUidMusBmViewId for launching Bookmark view else
        *                KUidMusBrViewId for browser view
        * @param aUrlLink Link to be loaded by the browser view.
        * @param aDefaultAccessPoint URL access point
        * @param aRskId Right softkey id
        */
        void LaunchMusicShopL(
                        const TUid& aViewId,
                        const TDesC& aUrlLink,
                        const TUint aDefaultAccessPoint,
                        const TInt aRskId
                        );


    private:
        CEikonEnv* iCoeEnv;
        CAknNullService*        iNullService;
        CAknLaunchAppService*   iLaunchAppService;
    };

#endif // MUSICSHOP_EMBEDDED_LAUNCHER_H

// End of File
