

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
* Description:  Thumbnail loader
*
*/

#ifndef MPXCOLLECTIONVIEWHGTNLOADER_H_
#define MPXCOLLECTIONVIEWHGTNLOADER_H_

#include <e32base.h>
#include <thumbnailmanager.h> // needs to be included here because of a bug in thumbnailmanagerobserver.h
#include <thumbnailmanagerobserver.h>

class CThumbnailManager;
class CFbsBitmap;
class CAsyncCallBack;

/**
 * Observer iface
 */
class MMpxTNLoaderObserver
    {
public:
    virtual void TNReadyL(TInt aError, CFbsBitmap* aBitmap, CFbsBitmap* aMask, TInt aIndex) = 0;
    };

/**
 * TN Loader
 */
NONSHARABLE_CLASS( CMpxTNLoader ) : public CBase, public MThumbnailManagerObserver
    {
public: // CONSTRUCTORS & DESCTRUCTOR

    static CMpxTNLoader* NewL(MMpxTNLoaderObserver& aObserver, TThumbnailSize aSize );
    virtual ~CMpxTNLoader();

public: // NEW FUNCTIONS

    void LoadThumbL(TInt aIndex, const TDesC& aFileName );
    void CancelThumb( TInt aIndex );
    void CancelAll();
    void SetSizeL( TThumbnailSize aSize );

private: // From MThumbnailManagerObserver

    void ThumbnailPreviewReady( MThumbnailData& aThumbnail,
        TThumbnailRequestId aId );
    void ThumbnailReady( TInt aError, MThumbnailData& aThumbnail,
        TThumbnailRequestId aId );

private: // CONSTRUCTORS

    CMpxTNLoader(MMpxTNLoaderObserver& aObserver,
            TThumbnailSize aSize );
    void ConstructL();

private: // NEW FUNCTIONS

    TInt FindLoadingById(TThumbnailRequestId aId, TBool aRemove = EFalse);
    TInt FindLoadingByIndex(TInt aIndex, TBool aRemove = EFalse);
    static TInt LoadThumbnail( TAny* aSelf );
    void LoadNextTN();

private: // DATA

    MMpxTNLoaderObserver& iObserver;
    CThumbnailManager* iTnEngine; // Own

    // Loading information
    class TLoadingTN
        {
    public:
        TLoadingTN(TThumbnailRequestId aId, TInt aIndex, const TDesC& aFileName )
        : iId(aId), iIndex(aIndex), iFileName( aFileName )
            {
            }

        TThumbnailRequestId iId;
        TInt iIndex;
        TFileName iFileName;
        };

    RPointerArray<TLoadingTN> iLoading; // Loading Thubmbs
    CAsyncCallBack* iAsyncCallBack;
    TThumbnailSize iSize;
    };

#endif // MPXCOLLECTIONVIEWHGTNLOADER_H_
