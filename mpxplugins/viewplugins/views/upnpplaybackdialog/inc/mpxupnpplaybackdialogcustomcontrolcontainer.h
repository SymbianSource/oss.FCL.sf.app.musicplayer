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
* Description:  Music Player UPnP Playback dialog declaration
*
*/


#ifndef MPXUPNPPLAYBACKDIALOGCUSTOMCONTROLCONTAINER_H
#define MPXUPNPPLAYBACKDIALOGCUSTOMCONTROLCONTAINER_H

// INCLUDE FILES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CMPXUPnPPlaybackDialogCustomControl;

// CLASS DECLARATION

/**
 *  The container for the custom control in UPnP Playback Dialog
 */
NONSHARABLE_CLASS(CMPXUPnPPlaybackDialogCustomControlContainer) : public CCoeControl 
    {
public:  // Constructors and destructor
    
    /**
     * Destructor.
     */
    ~CMPXUPnPPlaybackDialogCustomControlContainer(); 

public: // New functions
    
    /**
     * Draw function removes flicker from CEikDialog's Draw()..
     * @param TRect The container's extent
     */
    void Draw( const TRect & ) const;
    
public: // from base classes
    
    /**
     * From CoeControl.
     * Handles a change to the control's resources.
     *
     * @param aType A message UID value.
     */
    void HandleResourceChange( TInt aType );     

public:     // Data

    CMPXUPnPPlaybackDialogCustomControl* iDialogCustomControl; // Custom Control
    
    };

#endif      // MPXUPNPPLAYBACKDIALOGCUSTOMCONTROLCONTAINER_H 
            
// End of File