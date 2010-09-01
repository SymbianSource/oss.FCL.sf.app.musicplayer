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


#ifndef C_CMPXADDTRACKSLISTMODEL_H
#define C_CMPXADDTRACKSLISTMODEL_H

// INCLUDES
#include <mpxcollectionpath.h>  // TMPXItemId

// CLASS DECLARATION
class MDesCArray;
class CMPXMedia;
class CMPXMediaArray;

/**
*  List model for add tracks dialog.
*  Keep track of list item .
*/
NONSHARABLE_CLASS( CMPXAddTracksListModel ) : public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     * @return A pointer to a newly created object
     */
    static CMPXAddTracksListModel* NewL();

    /**
     * Destructor.
     */
    virtual ~CMPXAddTracksListModel();

public: // New functions

    /**
     * Reset tracks list.
     */
    void ResetTracksList();

    /**
     * Return track name.
     * @param aIndex Track's index.
     * @return Track name.
     */
    const TDesC& TrackName( TInt aIndex ) const;

    /**
     * Return track type.
     * @param aIndex Track's index.
     * @return Track type.
     */
    TInt TrackTypeL( TInt aIndex ) const;

    /**
     * Return track Category.
     * @param aIndex Track's index.
     * @return Track category.
     */
    TInt TrackCategoryL( TInt aIndex ) const;

    /**
     * Return track id.
     * @param aIndex Track's index.
     * @return Track id.
     */
    TMPXItemId TrackIdL( TInt aIndex ) const;

    /**
     * Return media at a given index
     *
     * @param aIndex index to get
     */
    const CMPXMedia& MediaL( TInt aIndex ) const;

    /**
     * Return number of tracks stored in the list.
     * @return Number of tracks stored in the list.
     */
    TInt TracksListCount() const;

    /**
     * Insert groups at aPos of the tracks list.
     * @param aMedia media array containing the track info.
     * @param aPos Index for the groups to be added to. Ignore this field
     *             if groups are appended to the end of the list.
     * @param aCategory category of the group could be either EMPXSong or EMPXArtist
     */
    void InsertGroupsL( CMPXMediaArray& aMedia, TInt aPos = -1 , TInt aCategory = EMPXArtist);

    /**
     * Insert tracks under the current group.
     * @param aTrack track with song ids.
     * @param aIndex Index to insert.
     */
    void InsertTrackL( CMPXMedia& aTrack, TInt aIndex );

    /**
     * Insert tracks under the current group. Update the expanded array too.
     * @param aTracks List of tracks with song ids.
     * @param aIndex Index of the group.
     */
    void InsertTracksUnderGroupL(
        CMPXMediaArray& aTracks,
        TInt aIndex );

    /**
     * Remove all tracks under group.
     * @param aIndex Index of the group which tracks will be removed.
     * @return Number of tracks removed.
     */
    TInt RemoveAllTracksFromGroupL( TInt aIndex );

    /**
     * Remove tracks .
     * @param aIndex Index of the track which will be removed.
     */
    void RemoveTracks( TInt aIndex );

    /**
     * Return expanded groups count.
     * @return Expanded groups count.
     */
    TInt ExpandedGroupsCount() const;

    /**
     * Check if the index is expanded or not.
     * @param aIndex Listbox index.
     * @return ETrue if aIndex is expanded. Otherwise, EFalse.
     */
    TBool IsExpandedIndex( TInt aIndex ) const;

    /**
     * Mark the current index as expanded.
     * @param aIndex Index to be marked as expanded.
     * @param aCount Number of tracks added.
     */
    void MarkExpandedL( TInt aIndex, TInt aCount );

    /**
     * Clear the current index as collapsed.
     * @param aIndex Index to be removed from the expanded array.
     * @param aCount Number of tracks removed.
     */
    void UnMarkExpanded( TInt aIndex, TInt aCount );

    /**
     * Return the oldest expanded group index.
     * @return The oldest expanded group index.
     */
    TInt OldestExpandedGroupIndex();

    /**
     * Return the closest expanded group index.
     * @param aIndex Current index
     * @return Closest expanded group index if found. Otherwise, KErrNotFound.
     */
    TInt ClosestExpandedGroupIdx( TInt aIndex );

    /**
     * Return animation icon index for highlight item.
     * @param aIndex current highlight index number.
     * @return highlight item's aniamtion icon's index, KErrNotFound.
     */
    TInt AnimationIconNum( TInt aIndex );

    /**
     * Set animation icon index number.
     * @param aIndex current highlight index number
     * @param aAnimIconNum current animation icon index
     */
    void SetAnimationIconNum( TInt aIndex, TInt aAnimIconNum );

    /**
     * There are mathed songs, it means Songs(All) node is shown.
     * @return ETrue if there are matched songs,
     *         EFalse if there is not.
     */
    TBool FindSongs();

    /**
     * Set flag to indicate if there are mathed songs.
     * @param aFindSongs if there are songs matched
     */
    void SetFindSongsFlag( TBool aFindSongs );

    /**
     * determines if the current item is on removable drive
     *
     * @param aIndex current highlight index number
     * @return ETrue if on removable drive, EFalse otherwise
     */
    TBool IsOnRemovableDrive( TInt aIndex );

private: // Constructors

    /**
     * C++ default constructor.
     */
    CMPXAddTracksListModel();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data
    CMPXMediaArray*                     iTracksList;
    RArray<TInt>                        iExpandedIndices;
    TInt                                iCurrentHighlightIndex; // Current highlite item
    TInt                                iAnimIconNum;           // animation icon number
    TBool                               iFindSongs;             // Find songs flag
    TInt iMMCDrive;
    };

#endif  // C_CMPXADDTRACKSLISTMODEL_H

// End of File
