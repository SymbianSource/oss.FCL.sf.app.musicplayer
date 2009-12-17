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
* Description:  Helper class for sending response back to RemCon Framework.
*
*/



#ifndef C_CMPXREMCONKEYRESPONSE_H
#define C_CMPXREMCONKEYRESPONSE_H


// INCLUDE FILES
#include <remconcoreapitarget.h>


// CLASS DECLARATION

/**
 *  Helper class for sending response back to 
 *  Remote Controller Framework.
 *  All events must be completed.
 *
 *  @lib mpxmediakeyhandler.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXRemConKeyResponse ) : public CActive
    {
public:
        
    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @param aRemConCoreApiTarget RemCon core target object.
     * @return Pointer to newly created object.
     */
    static CMPXRemConKeyResponse* NewL( 
        CRemConCoreApiTarget& aRemConCoreApiTarget );
        
    /**
     * Destructor.
     */
    virtual ~CMPXRemConKeyResponse();
   
    /**
     * Send the any key response back to Remcon server
     *
     * @since 3.0
     * @param aOperationId RemCon operation Id.
     */
    void CompleteAnyKey( TRemConCoreApiOperationId aOperationId );
    
private:

    /**
     * C++ default constructor.
     */
    CMPXRemConKeyResponse(
        CRemConCoreApiTarget& aRemConCoreApiTarget );

// from base class CActive

    /**
     * From CActive
     * Handles an active object's request completion event.
     */
    void RunL();

    /**
     * From CActive
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();

private:    // Data

    /**
     * Response array.
     * Own.
     */
    RArray<TRemConCoreApiOperationId> iResponseArray;
    
    /**
     * Remote controller.
     * Not own.
     */
    CRemConCoreApiTarget& iRemConCoreApiTarget;
    }; 

#endif  // C_CMPXREMCONKEYRESPONSE_H

// End of File
