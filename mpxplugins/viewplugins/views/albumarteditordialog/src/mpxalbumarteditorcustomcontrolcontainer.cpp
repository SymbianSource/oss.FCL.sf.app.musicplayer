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
* Description:  Music Player album art editor dialog implementation
*
*/


// INCLUDE FILES

#include "mpxalbumarteditorcustomcontrolcontainer.h"
#include "mpxalbumarteditordialogcustomcontrol.h"

// Destructor
CMPXAlbumArtEditorCustomControlContainer::~CMPXAlbumArtEditorCustomControlContainer() 
    { 
    delete iDialogCustomControl; 
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorCustomControlContainer::Draw
// Removes flicker from CEikDialog's Draw().
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorCustomControlContainer::Draw( const TRect & ) const
    {
    }
    
// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//    
void CMPXAlbumArtEditorCustomControlContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );    
    }

//  End of File  
