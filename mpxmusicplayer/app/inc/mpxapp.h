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
* Description:  Application class required by AVKON application framework.
*
*/



#ifndef CMPXAPP_H
#define CMPXAPP_H


// INCLUDES
#include <aknapp.h>


// CLASS DECLARATION

/**
 *  CMPXApp application class.
 *
 *  Provides factory to create concrete document object.
 *
 *  @lib mpx.exe
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CMPXApp ) : public CAknApplication
    {
private:

// from base class CApaApplication

    /**
     * From CApaApplication, creates CMPXDocument document object.
     * @return A pointer to the created document object.
     */
    CApaDocument* CreateDocumentL();
    
    /**
     * From CApaApplication, returns application's UID (defined in mpxconstants.h).
     * @return The UID for music player application.
     */
    TUid AppDllUid() const;
    };

#endif // CMPXAPP_H

// End of File
