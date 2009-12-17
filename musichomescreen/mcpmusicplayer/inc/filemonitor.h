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
* Description:  Monitors the file system for changes in a file.
*
*/


#ifndef C_FILEMONITOR_H
#define C_FILEMONITOR_H

#include <e32base.h>
#include <f32file.h>//??? remove???

class MFileMonitorObserver;

/**
 *  File monitor class
 *
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CFileMonitor ) : public CActive
    {

public:

    /**
    * Two-phased constructor
    */
    static CFileMonitor* NewL( MFileMonitorObserver& aObserver);

    /**
    * Virtual destructor
    */
    virtual ~CFileMonitor();

    /**
    * Start monitoring a particular file
    * @param aFile file to monitor
    */
    void StartL( const TDesC& aFile );
   
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
    CFileMonitor( MFileMonitorObserver& aObserver);

    /**
    * 2nd phased constructor
    */
    void ConstructL();

private: // data
    MFileMonitorObserver&  iObserver;
    RFs iFs; 
    HBufC* iFilePath;
    };

#endif // C_FILEMONITOR_H
