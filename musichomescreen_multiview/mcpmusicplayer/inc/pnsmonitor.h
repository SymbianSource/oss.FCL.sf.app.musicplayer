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
* Description:  Monitors for publish and subscribe property.
*
*/


#ifndef C_PNSMONITOR_H
#define C_PNSMONITOR_H

#include <e32base.h>

class MPNSMonitorObserver;

/**
 *  publish and subscribe monitor class
 *
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CPNSMonitor ) : public CActive
    {

public:

    /**
    * Two-phased constructor
    */
    static CPNSMonitor* NewL( MPNSMonitorObserver& aObserver );

    /**
    * Virtual destructor
    */
    virtual ~CPNSMonitor();

    /**
    * Start monitoring a particular publish and subscribe property
    */
    void Start();
   
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
    TInt RunError( TInt aError );    
            
private:
    
    /**
    * Private constructor
    */
    CPNSMonitor( MPNSMonitorObserver& aObserver );

    /**
    * 2nd phased constructor
    */
    void ConstructL();

private: // data
    MPNSMonitorObserver& iObserver;
    RProperty iPropertyHandler;
    };

#endif // C_PNSMONITOR_H
