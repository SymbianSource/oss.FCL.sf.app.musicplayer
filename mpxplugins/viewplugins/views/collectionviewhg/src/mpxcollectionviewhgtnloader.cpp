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
* Description:  Thumbnail loader implementation
*
*/


#include <thumbnaildata.h>
#include <AknIconUtils.h>
#include "mpxcollectionviewhgtnloader.h"
#include "mpxlog.h"

_LIT( KMPXAlbumMimeType, "audio/mpeg3" );


// -----------------------------------------------------------------------------
// CODEScrollerTNLoader::NewL()
// -----------------------------------------------------------------------------
//
CMpxTNLoader* CMpxTNLoader::NewL (
        MMpxTNLoaderObserver& aObserver, TThumbnailSize aSize )
    {
    CMpxTNLoader* self = new (ELeave) CMpxTNLoader(aObserver, aSize );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CMpxTNLoader:~CMpxTNLoader()
// -----------------------------------------------------------------------------
//
CMpxTNLoader::~CMpxTNLoader ( )
    {
    // cancel outstanding requests
    if(iTnEngine)
        {
//        CancelAll();
        }

    iLoading.ResetAndDestroy();
    delete iTnEngine;

    if( iAsyncCallBack )
        iAsyncCallBack->Cancel();

    delete iAsyncCallBack;
    }

TInt CMpxTNLoader::LoadThumbnail( TAny* aSelf )
    {
    CMpxTNLoader* self = (CMpxTNLoader*)aSelf;
    self->LoadNextTN();
    return KErrNone;
    }

void CMpxTNLoader::LoadNextTN()
    {
    if( iLoading.Count() > 0 )
        {
        TInt index = iLoading[0]->iIndex;
        if( index >= 0 && iLoading[0]->iId == 0)
            {
            CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(iLoading[0]->iFileName, KMPXAlbumMimeType);
            TRAPD(err, iLoading[0]->iId = iTnEngine->GetThumbnailL( *source, NULL, 0 ); )
        	MPX_DEBUG4( "GetThumbnailL: %d [%d,%d]", err, index, iLoading[0]->iId);
			CleanupStack::PopAndDestroy(source);
            }
        }
    }

// -----------------------------------------------------------------------------
// CMpxTNLoader::ThumbnailPreviewReady()
// -----------------------------------------------------------------------------
//
void CMpxTNLoader::ThumbnailPreviewReady (MThumbnailData& /*aThumbnail*/,
        TThumbnailRequestId /*aId*/ )
    {
    // Previews not currently used
    }

// -----------------------------------------------------------------------------
// CMpxTNLoader::ThumbnailReady()
// -----------------------------------------------------------------------------
//
void CMpxTNLoader::ThumbnailReady (TInt aError,
        MThumbnailData& aThumbnail, TThumbnailRequestId aId )
    {
    TInt index = FindLoadingById(aId, ETrue);
	MPX_DEBUG4( "ThumbnailReady: %d [%d,%d]", aError, index, aId );

    if( index != KErrNotFound )
        {
        CFbsBitmap* bitmap = (aError == KErrNone ? aThumbnail.DetachBitmap() : NULL);

        TRAP_IGNORE(iObserver.TNReadyL(aError, bitmap, NULL, index));

        if( iLoading.Count() > 0 )
            {
            if(!iAsyncCallBack->IsActive())
                iAsyncCallBack->CallBack();
            }
        }
    }

// -----------------------------------------------------------------------------
// CMpxTNLoader::CMpxTNLoader()
// -----------------------------------------------------------------------------
//
CMpxTNLoader::CMpxTNLoader (MMpxTNLoaderObserver& aObserver, TThumbnailSize aSize )
: iObserver( aObserver ), iSize(aSize)
    {

    }

// -----------------------------------------------------------------------------
// CMpxTNLoader::ConstructL()
// -----------------------------------------------------------------------------
//
void CMpxTNLoader::ConstructL ()
    {
    iTnEngine = CThumbnailManager::NewL( *this );
    iTnEngine->SetFlagsL( CThumbnailManager::EDefaultFlags );
    iTnEngine->SetQualityPreferenceL( CThumbnailManager::EOptimizeForQuality );
    iTnEngine->SetThumbnailSizeL( iSize );
    TCallBack callback(CMpxTNLoader::LoadThumbnail, this);
    iAsyncCallBack = new (ELeave) CAsyncCallBack( CActive::EPriorityStandard );
    iAsyncCallBack->Set(callback);
    }

// -----------------------------------------------------------------------------
// CMpxTNLoader::FindLoading()
// -----------------------------------------------------------------------------
//
TInt CMpxTNLoader::FindLoadingById(TThumbnailRequestId aId, TBool aRemove)
    {
    TInt index = KErrNotFound;
    for(TInt i = 0; i < iLoading.Count(); ++i)
        {
        if(iLoading[i]->iId == aId)
            {
            index = iLoading[i]->iIndex;
            if(aRemove)
                {
                delete iLoading[i];
                iLoading.Remove(i);
                }
            break;
            }
        }
    return index;
    }

// -----------------------------------------------------------------------------
// CMpxTNLoader::FindLoading()
// -----------------------------------------------------------------------------
//
TInt CMpxTNLoader::FindLoadingByIndex(TInt aIndex, TBool aRemove)
    {
    TInt index = KErrNotFound;
    for(TInt i = 0; i < iLoading.Count(); ++i)
        {
        if(iLoading[i]->iIndex == aIndex)
            {
            index = i;
            if(aRemove)
                {
                delete iLoading[i];
                iLoading.Remove(i);
                }
            break;
            }
        }
    return index;
    }

// -----------------------------------------------------------------------------
// CMpxTNLoader::LoadThumbL()
// -----------------------------------------------------------------------------
//
void CMpxTNLoader::LoadThumbL( TInt aIndex, const TDesC& aFileName )
    {
    if( FindLoadingByIndex(aIndex) == KErrNotFound )
        {
        iLoading.Append( new (ELeave) TLoadingTN( 0, aIndex, aFileName ) );
        if( !iAsyncCallBack->IsActive() )
            iAsyncCallBack->CallBack();
        }
    }

// -----------------------------------------------------------------------------
// CMpxTNLoader::CancelThumb()
// -----------------------------------------------------------------------------
//
void CMpxTNLoader::CancelThumb( TInt aIndex )
    {
    TInt loadingIndex = FindLoadingByIndex( aIndex );
    if(loadingIndex != KErrNotFound)
        {
        // Current thumbnail manager has bugs in thumbnail request cancelation.
        // There is a fix coming in near future but until that we cannot cancel requests
        // since after that we dont get any thumbnails from server.
//        if( iLoading[loadingIndex]->iId != 0 )
//            {
//            RDebug::Print(_L("!!CANCEL REQUEST!!"));
//            iTnEngine->CancelRequest( iLoading[loadingIndex]->iId );
//            }
        delete iLoading[loadingIndex];
        iLoading.Remove(loadingIndex);
        }
    }

void CMpxTNLoader::CancelAll()
    {
    iLoading.ResetAndDestroy();
    return;
    // Current thumbnail manager has bugs in thumbnail request cancelation.
    // There is a fix coming in near future but until that we cannot cancel requests
    // since after that we dont get any thumbnails from server.
    while ( iLoading.Count() > 0 )
        {
        if( iLoading[0]->iId != 0 )
            iTnEngine->CancelRequest( iLoading[0]->iId );
        delete iLoading[0];
        iLoading.Remove(0);
        }
    }

void CMpxTNLoader::SetSizeL( TThumbnailSize aSize )
    {
    iTnEngine->SetThumbnailSizeL( aSize );
    }
