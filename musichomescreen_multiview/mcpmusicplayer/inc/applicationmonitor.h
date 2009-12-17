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
* Description:  Monitors an application for exit event.
*
*/


#ifndef C_APPLICATIONMONITOR_H
#define C_APPLICATIONMONITOR_H

#include <e32base.h>

class MApplicationMonitorObserver;
class RThread;

/**
 *  Application monitor class
 *
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CApplicationMonitor ) : public CActive
    {

public:

    /**
    * Two-phased constructor
    */
    static CApplicationMonitor* NewL( MApplicationMonitorObserver& aObserver);

    /**
    * Virtual destructor
    */
    virtual ~CApplicationMonitor();

    /**
    * Start monitoring a particular Application
    * @param aApplication to monitor
    * @param aRootAppIndication ETrue for RootApp and EFalse EmbeddedApp
    */
    void StartL(  TUid aAppUid , TBool aRootAppIndication = ETrue);
   
protected:
        
    /**
    * From CActive
    */
    void RunL();
    
    /**
    * From CActive
    */
    void DoCancel();

    /**
    *  From CActive
    */
    TInt RunError(TInt aError);    
            
private:
    
    /**
    * Private constructor
    */
    CApplicationMonitor( MApplicationMonitorObserver& aObserver);

    /**
    * 2nd phased constructor
    */
    void ConstructL();

private: // data
    MApplicationMonitorObserver&  iObserver;
    RThread iThread; 
    };

#endif // C_APPLICATIONMONITOR_H
