/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MPX common container Media helper.
*
*/



#ifndef CMPXCOMMONCONTAINERMEDIAHELPER_H
#define CMPXCOMMONCONTAINERMEDIAHELPER_H

#include <e32base.h>
#include <mpxcollectionobserver.h>

// FORWARD DECLARATIONS
class MMPXCommonContainer;
class MMPXCollectionUtility;

/**
*  Helper class to retrieve media for clients
*/
NONSHARABLE_CLASS( CMPXCommonContainerMediaHelper )
    : public CBase
    , public MMPXCollectionObserver
    {
public:

    /**
    * Two-phased constructor
    */
    static CMPXCommonContainerMediaHelper* NewL(
        MMPXCommonContainer* aCommonContainer );

    /**
    * Destructor.
    */
    virtual ~CMPXCommonContainerMediaHelper();

private: // constructors

    /**
    * Constructor.
    */
    CMPXCommonContainerMediaHelper( MMPXCommonContainer* aCommonContainer );

    /**
    * Symbian OS constructor.
    */
    void ConstructL();

public: // New Methods

    /**
    * Retrieves the media given the current path, and upon return,
    * either calls back the observer, or broadcasts the message to
    * the given client list
    *
    * @param aCmd CMPXCommand containing the attributes and attribute spec
    *             Ownership of aAttrs is passed to this method
    */
    void MediaL(
        const TMPXItemId& aItemId,
        RArray<TMPXAttribute> aAttrs,
        CMPXMedia* aAttrSpecs );

    /**
    * Updates the specified media with the results from the search
    * criteria
    *
    * @param aMedia Media to be updated
    * @param aSearchAttrs Media search attributes
    * @param aSearchAttrSpecs Media search attribute specs
    */
    void UpdateMediaL(
        CMPXMedia* aMedia,
        RArray<TMPXAttribute> aSearchAttrs,
        CMPXMedia* aSearchAttrSpecs );

    /**
    * Cancels all outstanding requests
    */
    void CancelRequests();

private: // From base classes

	/**
    *  From MMPXCollectionObserver
    */
    void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr );

    /**
    *  From MMPXCollectionObserver
    */
    void HandleOpenL(
        const CMPXMedia& aEntries,
        TInt aIndex,
        TBool aComplete,
        TInt aError );

    /**
    *  From MMPXCollectionObserver
    */
    void HandleOpenL(
        const CMPXCollectionPlaylist& aPlaylist,
        TInt aError );

    /**
     *  From MMPXCollectionObserver
     */
    void HandleCommandComplete(
        CMPXCommand* aCommandResult,
        TInt aError );

    /**
    *  From MMPXCollectionMediaObserver
    */
    void HandleCollectionMediaL(
        const CMPXMedia& aMedia,
        TInt aError);

private: // data

    MMPXCommonContainer*    iCommonContainer;
    MMPXCollectionUtility*  iCollectionUtility;
    };

#endif // CMPXCOMMONCONTAINERMEDIAHELPER_H

//End of file
