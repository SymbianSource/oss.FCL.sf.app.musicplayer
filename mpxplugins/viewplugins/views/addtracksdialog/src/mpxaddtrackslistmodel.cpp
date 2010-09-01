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
* Description:  A list model for add tracks dialog
*
*/


// INCLUDE FILES
#include <bamdesca.h>
#include <badesca.h>
#include <f32file.h>

#include <mpxlog.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxcommonuihelper.h>

#include "mpxaddtracksdialog.hrh"
#include "mpxaddtrackslistmodel.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPXAddTracksListModel* CMPXAddTracksListModel::NewL()
    {
    MPX_FUNC( "CMPXAddTracksListModel::NewL" );
    CMPXAddTracksListModel* self =
        new (ELeave) CMPXAddTracksListModel();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::CMPXAddTracksListModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPXAddTracksListModel::CMPXAddTracksListModel()
    :iCurrentHighlightIndex(0),
    iFindSongs( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPXAddTracksListModel::ConstructL()
    {
    MPX_FUNC( "CMPXAddTracksListModel::ConstructL" );
    iTracksList = CMPXMediaArray::NewL();
    iMMCDrive = CMPXCommonUiHelper::MMCDriveNumber();
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CMPXAddTracksListModel::~CMPXAddTracksListModel()
    {
    MPX_DEBUG1( "CMPXAddTracksListModel::~CMPXAddTracksListModel START" );
    if ( iTracksList )
        {
        iTracksList->Reset();
        delete iTracksList;
        }
    iExpandedIndices.Reset();
    iExpandedIndices.Close();
    MPX_DEBUG1( "CMPXAddTracksListModel::~CMPXAddTracksListModel END" );
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::ResetTracksList
// Reset tracks list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksListModel::ResetTracksList()
    {
    MPX_FUNC( "CMPXAddTracksListModel::ResetTracksList" );
    iTracksList->Reset();
    iExpandedIndices.Reset();
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::TrackName
// Return track name.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CMPXAddTracksListModel::TrackName( TInt aIndex ) const
    {
    //MPX_FUNC( "CMPXAddTracksListModel::TrackName" );
    CMPXMedia* item( NULL );
    TInt count( iTracksList->Count() );
    if ( count > 0 && aIndex < count )
        {
        TRAP_IGNORE( item = iTracksList->AtL( aIndex ) );
        }
    return item ? item->ValueText( KMPXMediaGeneralTitle ) : KNullDesC;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::TrackTypeL
// Return track type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXAddTracksListModel::TrackTypeL( TInt aIndex ) const
    {
    //MPX_FUNC( "CMPXAddTracksListModel::TrackTypeL" );
    CMPXMedia* item( NULL );
    TInt count( iTracksList->Count() );
    if ( count > 0 && aIndex < count )
        {
        item = iTracksList->AtL( aIndex );
        User::LeaveIfNull( item );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    return *item->Value<TMPXGeneralType>( KMPXMediaGeneralType );
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::TrackCategoryL
// Return track Category.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXAddTracksListModel::TrackCategoryL( TInt aIndex ) const
    {
    //MPX_FUNC( "CMPXAddTracksListModel::TrackCategoryL" );
    CMPXMedia* item( NULL );
    TInt count( iTracksList->Count() );
    if ( count > 0 && aIndex < count )
        {
        item = iTracksList->AtL( aIndex );
        User::LeaveIfNull( item );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    return *item->Value<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::TrackIdL
// Return track id.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TMPXItemId CMPXAddTracksListModel::TrackIdL( TInt aIndex ) const
    {
    //MPX_FUNC( "CMPXAddTracksListModel::TrackIdL" );
    CMPXMedia* item( NULL );
    TInt count( iTracksList->Count() );
    if ( count > 0 && aIndex < count )
        {
        item = iTracksList->AtL( aIndex );
        User::LeaveIfNull( item );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    return *item->Value<TMPXItemId>( KMPXMediaGeneralId );
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::MediaL
// Return track media.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const CMPXMedia& CMPXAddTracksListModel::MediaL( TInt aIndex ) const
    {
    //MPX_FUNC( "CMPXAddTracksListModel::MediaL" );
    TInt entriesCount( iTracksList->Count() );
    if ( ( entriesCount < aIndex + 1 ) || ( aIndex < 0 ) )
        {
        User::Leave( KErrArgument );
        }
    CMPXMedia* item( iTracksList->AtL( aIndex ) );
    User::LeaveIfNull( item );
    return *item;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::TracksListCount
// Return number of tracks stored in the list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXAddTracksListModel::TracksListCount() const
    {
    MPX_FUNC( "CMPXAddTracksListModel::TracksListCount" );
    return iTracksList->Count();
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::InsertGroupsL
// Insert groups at aPos of the tracks list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksListModel::InsertGroupsL(
    CMPXMediaArray& aMedia, TInt aPos , TInt aCategory)
    {
    MPX_FUNC( "CMPXAddTracksListModel::InsertGroupsL" );
    TInt count = aMedia.Count();
    MPX_DEBUG2( "CMPXAddTracksListModel::InsertGroupsL MdcaCount = %d", count );
    for ( TInt index = 0; index < count; index++ )
        {
        MPX_DEBUG2( "CMPXAddTracksListModel::InsertGroupsL index = %d", index );
        CMPXMedia* origItem( aMedia.AtL( index ) );
        User::LeaveIfNull( origItem );
        CMPXMedia* item( CMPXMedia::NewL( *origItem ) );
        item->SetTObjectValueL( KMPXMediaGeneralType, EMPXGroup );
        item->SetTObjectValueL( KMPXMediaGeneralCategory, aCategory );
        if ( aPos == -1 )
            {
            iTracksList->AppendL( item );
            }
        else
            {
            iTracksList->InsertL( item, aPos + index );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::InsertTrackL
// Insert track at position.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksListModel::InsertTrackL(
    CMPXMedia& aTrack, TInt aIndex )
    {
    MPX_FUNC( "CMPXAddTracksListModel::InsertTrackL" );
    CMPXMedia* media = CMPXMedia::NewL( aTrack );
    iTracksList->Insert( media, aIndex );
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::InsertTracksUnderGroupL
// Insert tracks under the current group. Update the expanded array too.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksListModel::InsertTracksUnderGroupL(
    CMPXMediaArray& aTracks, TInt aIndex )
    {
    MPX_FUNC( "CMPXAddTracksListModel::InsertTracksUnderGroupL" );
    // Make sure the current index is a group
    if ( TrackTypeL( aIndex ) == EMPXGroup )
        {
        TInt count = aTracks.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            CMPXMedia* origItem( aTracks.AtL( i ) );
            User::LeaveIfNull( origItem );
            CMPXMedia* media = CMPXMedia::NewL( *origItem );
            iTracksList->Insert( media, aIndex + 1 + i );
            }
        // Mark as expanded
        MarkExpandedL( aIndex, count );
        }
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::RemoveAllTracksFromGroupL
// Remove all tracks under group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXAddTracksListModel::RemoveAllTracksFromGroupL( TInt aIndex )
    {
    MPX_FUNC( "CMPXAddTracksListModel::RemoveAllTracksFromGroupL" );
    TInt count = 0;
    if ( TrackTypeL( aIndex ) == EMPXGroup )
        {
        TInt pos = aIndex + 1;
        while ( ( pos < TracksListCount() ) &&
            ( TrackTypeL( pos ) != EMPXGroup ) )
            {
            // Remove the track from the list
            iTracksList->Remove( pos );
            count++;
            }
        // Mark as collapsed
        UnMarkExpanded( aIndex, count );
        }
    return count;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::RemoveTracks
// Remove tracks.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksListModel::RemoveTracks( TInt aIndex )
    {
    MPX_FUNC( "CMPXAddTracksListModel::RemoveTracks" );
    iTracksList->Remove( aIndex );
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::ExpandedGroupsCount
// Return expanded groups count.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXAddTracksListModel::ExpandedGroupsCount() const
    {
    MPX_FUNC( "CMPXAddTracksListModel::ExpandedGroupsCount" );
    return iExpandedIndices.Count();
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::IsExpandedIndex
// Check if the index is expanded or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMPXAddTracksListModel::IsExpandedIndex( TInt aIndex ) const
    {
    //MPX_FUNC( "CMPXAddTracksListModel::IsExpandedIndex" );
    TBool result = EFalse;
    if ( iExpandedIndices.Find( aIndex ) != KErrNotFound )
        {
        result = ETrue;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::MarkExpandedL
// Mark the current index as expanded.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksListModel::MarkExpandedL( TInt aIndex, TInt aCount )
    {
    MPX_FUNC( "CMPXAddTracksListModel::MarkExpandedL" );
    // Update the expanded array accordingly.
    TInt count = iExpandedIndices.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TInt& temp = iExpandedIndices[ i ];
        if ( temp > aIndex )
            {
            temp += aCount;
            }
        }

    iExpandedIndices.Append( aIndex );
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::UnMarkExpanded
// Clear the current index as collapsed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksListModel::UnMarkExpanded( TInt aIndex, TInt aCount )
    {
    MPX_FUNC( "CMPXAddTracksListModel::UnMarkExpanded" );
    TInt pos = iExpandedIndices.Find( aIndex );
    if ( pos != KErrNotFound )
        {
        iExpandedIndices.Remove( pos );

        // Update the expanded array accordingly.
        TInt count = iExpandedIndices.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            TInt& temp = iExpandedIndices[ i ];
            if ( temp > aIndex )
                {
                temp -= aCount;
                }
            }

        iExpandedIndices.Compress();
        }
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::OldestExpandedGroupIndex
// Return the oldest expanded group index.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXAddTracksListModel::OldestExpandedGroupIndex()
    {
    MPX_FUNC( "CMPXAddTracksListModel::OldestExpandedGroupIndex" );
    // Oldest item is always located at the top of the list
    return iExpandedIndices[ 0 ];
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::ClosestExpandedGroupIdx
// Return the closest expanded group index.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXAddTracksListModel::ClosestExpandedGroupIdx( TInt aIndex )
    {
    MPX_FUNC( "CMPXAddTracksListModel::ClosestExpandedGroupIdx" );
    TInt count = iExpandedIndices.Count();
    TInt result = KErrNotFound;

    for ( TInt i = 0; i < count; i++ )
        {
        TInt temp = iExpandedIndices[ i ];
        if ( ( temp <= aIndex ) &&
            ( temp > result ) )
            {
            result = temp;
            }
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::AnimationIconNum
// Return animation icon index for highlight item.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXAddTracksListModel::AnimationIconNum( TInt aIndex )
    {
    //MPX_FUNC( "CMPXAddTracksListModel::AnimationIconNum" );
    TInt trackType = EMPXNoType;
    MPX_TRAPD( error, trackType = TrackTypeL( aIndex ) );

    if( ( aIndex == iCurrentHighlightIndex )
        && ( trackType != EMPXGroup )
        && ( error == KErrNone ) )
        {
        return iAnimIconNum;
        }
    else
        {
        return 0;   // no animation icon
        }
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::SetAnimationIconNum
// Set animation icon index number.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksListModel::SetAnimationIconNum(
    TInt aIndex,
    TInt aAnimIconNum )
    {
    MPX_FUNC( "CMPXAddTracksListModel::SetAnimationIconNum" );
    iCurrentHighlightIndex = aIndex;
    iAnimIconNum = aAnimIconNum;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::FindSongs
// Provide info if there is matched songs.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMPXAddTracksListModel::FindSongs()
    {
    MPX_FUNC( "CMPXAddTracksListModel::FindSongs" );
    return iFindSongs;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::SetFindSongsFlag
// Set flag to indicate if there are mathed songs.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksListModel::SetFindSongsFlag( TBool aFindSongs )
    {
    MPX_FUNC( "CMPXAddTracksListModel::SetFindSongsFlag" );
    iFindSongs = aFindSongs;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksListModel::FindSongs
// determines if the current item is on removable drive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMPXAddTracksListModel::IsOnRemovableDrive( TInt aIndex )
    {
    MPX_FUNC( "CMPXAddTracksListModel::IsOnRemovableDrive" );
    CMPXMedia* entry( NULL );
    TInt count( iTracksList->Count() );
    if ( count > 0 && aIndex < count )
        {
        TRAP_IGNORE( entry = iTracksList->AtL( aIndex ) ) ;
        }

    TUint flags(0);
    if ( entry )
        {
        if( entry->IsSupported( KMPXMediaGeneralFlags ) )
            {
            TRAP_IGNORE( flags = entry->ValueTObjectL<TUint>( KMPXMediaGeneralFlags ) );
            }
        }

    TInt driveNum( ( flags ) & ( KMPXMediaGeneralFlagsDriveInfo ) );
    return ( driveNum == iMMCDrive );
    }

//  End of File
