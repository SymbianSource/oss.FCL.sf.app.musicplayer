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
* Description:  Listbox array for common container.
*
*/


#ifndef CMPXCOMMONLISTBOXARRAY_H
#define CMPXCOMMONLISTBOXARRAY_H

#include <e32base.h>
#include <bamdesca.h>
#include <mpxcommonframeworkdefs.h>
#include <mpxplaybackframeworkdefs.h>

// FORWARD DECLARATIONS
class MMPXCommonListBoxArrayObserver;
class CAknIconArray;
class CMPXMediaArray;
class CMPXMedia;

// CLASS DECLARATION

/**
 *  Listbox array for common container.
 *  Creates item text descriptors according to the list model.
 *
 *  Dervied classes should implement MdcaPoint from MDesCArray
 *
 *  @lib mpxcommoncontainer.lib
 *  @since S60 v3.1
 */
class CMPXCommonListBoxArrayBase : public CBase, 
                                   public MDesCArray
    {
public:  // Constructors and destructor
    
    /**
     * C++ constructor.
     */
    IMPORT_C CMPXCommonListBoxArrayBase();
        
    /**
     * Destructor.
     */
    virtual ~CMPXCommonListBoxArrayBase(); 
    
    /**
     * Construct the array
     */
    IMPORT_C void ConstructListBoxArrayL();   

public: // new functions

    /**
     * Appends media array contained in the media object
     * to the current array
     *
     * @since 3.1
     * @param aMedia Media object containing the media array to be append
     */ 
    IMPORT_C virtual void AppendMediaL( const CMPXMedia& aMedia );

    /**
     * Resets media array
     *
     * @since 3.1
     */
    IMPORT_C void ResetMediaArrayL();
    
    /**
     * Get media array
     *
     * @since 3.1
     * @return Media array used by the listbox
     */
    IMPORT_C const CMPXMediaArray& MediaArray();
    
    /**
     * Get media at an index
     *
     * @since 3.1
     * @param aIndex index for get media
     * @return Media object at the given index
     */
    IMPORT_C const CMPXMedia& MediaL( TInt aIndex );
    
    /**
     * Get the media object for the container of current level
     * It does NOT contain the media array
     *
     * @since 3.1
     * @return Media object for the parent container for the current level
     */
    IMPORT_C const CMPXMedia& ContainerMedia();
    
    /**
     * Set observer
     *
     * @since 3.1
     * @param aObserver array event observer
     */
    IMPORT_C void SetObserver( MMPXCommonListBoxArrayObserver* aObserver);
    
    /**
     * Remove obsever
     *
     * @since 3.1
     */
    IMPORT_C void RemoveObserver();
    
    /**
     * Creates icon array
     *
     * @since 3.1
     * @return Pointer to the newly created icon array
     */   
    IMPORT_C virtual CAknIconArray* CreateIconArrayL();        

protected: // Functions from base classes

    /**
     * From MDesCArray, returns the number of descriptor elements
     * in a descriptor
     *
     * @since 3.1
     * @return The number of descriptor elements in a descriptor array.
     */
    IMPORT_C TInt MdcaCount() const;
    
protected:    // Data

    MMPXCommonListBoxArrayObserver*  iObserver;         // not owned
    CMPXMediaArray*                  iMedia;            // not owned
    CMPXMedia*                       iContainerMedia;   // owned
    HBufC*                           iItem;             // owned

    TMPXPlaybackState                iPbState;
    TInt                             iIndex;            // Now playing item index
    };

#endif  // CMPXCOMMONLISTBOXARRAY_H
            
// End of File
