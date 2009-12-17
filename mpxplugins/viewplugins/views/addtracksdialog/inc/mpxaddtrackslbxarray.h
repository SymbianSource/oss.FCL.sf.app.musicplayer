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
* Description:  Listbox array for add tracks dialog.
*
*/


#ifndef C_CMPXADDTRACKSLBXARRAY_H
#define C_CMPXADDTRACKSLBXARRAY_H

// FORWARD DECLARATIONS
class CGulIcon;
class CAknIconArray;
class CMPXAddTracksListModel;

// CLASS DECLARATION

/**
*  Listbox array for add tracks dialog.
*  Creates item text descriptors according to the list model.
*/
NONSHARABLE_CLASS( CMPXAddTracksLbxArray ) : public CBase,
                                             public MDesCArray
    {
public:

    // Listbox icons
    enum TMPXATLbxIcons
        {
        EMPXATLbxIconTrackExpand = 0,
        EMPXATLbxIconTrackCollapse,
        EMPXATLbxIconArtistExpand,
        EMPXATLbxIconArtistCollapse,
        EMPXATLbxIconEmpty,
        EMPXATLbxIconSongAddedAnim1,
        EMPXATLbxIconSongAddedAnim2,
        EMPXATLbxIconSongAddedAnim3,
        EMPXATLbxIconSongAddedAnim4,
        EMPXATLbxIconMMC,
        EMPXATLbxIconsCount
        };

public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aModel List model of Add tracks dialog.
     * @return A pointer to a newly created object
     */
    static CMPXAddTracksLbxArray* NewL(
        CMPXAddTracksListModel* aModel );

    /**
     * Destructor.
     */
    virtual ~CMPXAddTracksLbxArray();

public: // New functions

    /**
     * Creates new icon array. Ownership is transferred to caller.
     * @return New icon array.
     */
    CArrayPtr<CGulIcon>* CreateIconArrayL();

public: // Functions from base classes

    /**
     * From MDesCArray, returns the number of descriptor elements
     * in a descriptor
     * @return The number of descriptor elements in a descriptor array.
     */
    TInt MdcaCount() const;

    /**
     * From MDesCArray, indexes into a descriptor array.
     * @param aIndex The position of the descriptor element within a descriptor array.
     * @return Descriptor element located at position aIndex within a descriptor array
     */
    TPtrC MdcaPoint( TInt aIndex ) const;

private: // Constructors

    /**
     * C++ default constructor.
     * @param aModel List model of Add tracks dialog.
     */
    CMPXAddTracksLbxArray(
        CMPXAddTracksListModel* aModel );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // New functions

    /**
     * Load an icon and append it to an icon array.
     * @param aArray    pointer to the icon array
     * @param aID       skin id of the icon
     * @param aColorId  Item ID of the color table.
     * @param aColorIndex Index in the color table.
     * @param aMbmFile  path to the mbm/mif file containing the icon
     * @param aBitmapId mbm id of the bitmap
     * @param aMaskId   mbm id of the mask
     */
    void AppendColorIconToArrayL(
        CAknIconArray* aArray,
        const TAknsItemID& aID,
        const TAknsItemID& aColorId,
        TInt aColorIndex,
        const TDesC& aMbmFile,
        TInt aBitmapId,
        TInt aMaskId );

private:    // Data

    CMPXAddTracksListModel*  iModel; // Not owned
    HBufC* iItem;
    };

#endif  // C_CMPXADDTRACKSLBXARRAY_H

// End of File
