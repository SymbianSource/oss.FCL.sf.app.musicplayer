/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Responsible for interation with the category tables:
*                Artist, Album, Genre, Composer and AbstractAlbum
*
*/


// INCLUDE FILES
#include <mpxlog.h>
#include <bautils.h>
#include <mpxmediamusicdefs.h>
#include "mpxdbabstractalbum.h"
#include "mpxdbpluginqueries.h"
#include "mpxdbmanager.h"
#include "mpxdbcommonutil.h"
#include "mpxcollectiondbstd.h"
#include "mpxdbcommondef.h"
#include "mpxcollectiondbdef.h"

#ifdef RD_MPX_TNM_INTEGRATION
_LIT( KImageFileType, "image/jpeg" );
#endif //RD_MPX_TNM_INTEGRATION


// CONSTANTS




// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbAbstractAlbum* CMPXDbAbstractAlbum::NewL(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::NewL");

    CMPXDbAbstractAlbum* self = CMPXDbAbstractAlbum::NewLC(aDbManager, aCategory);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbAbstractAlbum* CMPXDbAbstractAlbum::NewLC(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::NewLC");

    CMPXDbAbstractAlbum* self = new (ELeave) CMPXDbAbstractAlbum(aDbManager, aCategory);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbAbstractAlbum::~CMPXDbAbstractAlbum()
    {
    MPX_FUNC("CMPXDbAbstractAlbum::~CMPXDbAbstractAlbum");
#ifdef RD_MPX_TNM_INTEGRATION
    delete iTNManager;
#endif //RD_MPX_TNM_INTEGRATION
    }

// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::GetDriveIdL
// ----------------------------------------------------------------------------
//
TInt CMPXDbAbstractAlbum::GetDriveIdL(
    TUint32 aAbstractAlbumId)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::GetDriveIdL");
    return MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
        ExecuteIntQueryL(KCriterionCategoryVolumeId, aAbstractAlbumId));
    }


// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::AddItemL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbAbstractAlbum::AddItemL(
    const TDesC& aName,
    const TDesC& aAlbumArtist,
    const TDesC& aGenre,
    TInt aDriveId,
    TBool& aNewRecord,
    TBool aCaseSensitive)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::AddItemL");

    TUint32 rowId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXAbstractAlbum,
        aName, aCaseSensitive));
  
    if ( aNewRecord )
        {
        HBufC* name = MPXDbCommonUtil::ProcessSingleQuotesLC(aName);
        HBufC* albumartist = MPXDbCommonUtil::ProcessSingleQuotesLC(aAlbumArtist);
        HBufC* genre = MPXDbCommonUtil::ProcessSingleQuotesLC(aGenre);
        
        iDbManager.ExecuteQueryL(aDriveId, KQueryAbstractAlbumInsert, rowId, name, albumartist, genre, 0, MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), aDriveId));
        CleanupStack::PopAndDestroy(3, name);        
        }
    else
        {
        // increment the number of songs for the category
        HBufC* query = PreProcessStringLC(KQueryCategoryIncrementSongCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, rowId);
        CleanupStack::PopAndDestroy(query);
        }

    return rowId;
    }



// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::DecrementSongsForCategoryL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::DecrementSongsForCategoryL(
    const TUint32 aId,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages,
    TBool& aItemExist,
    TBool aMtpInUse)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::DecrementSongsForCategoryL");
    
    TInt songCount = GetSongsCountL(aDriveId, aId);
       // if just one song uses this category. Use <= just in case
       //while MTP connection, just decrease songCount in DB, do not do deletion.
    if ( songCount > 0 )
        {
        aItemExist = ETrue;
        // decrement the number of songs for the category
        HBufC* query = PreProcessStringLC(KQueryCategoryDecrementSongCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, aId);
        CleanupStack::PopAndDestroy(query);
        songCount--;
        }
    
    if ((songCount == 0) && !aMtpInUse)
        {
        HBufC* uri = DeleteAbstractAlbumL(aId, aDriveId);
        MPX_DEBUG1("CMPXDbAbstractAlbum::DeleteAbstractAlbumL,implicitly delete AbstractAlbum Object when it has 0 references");  
        aItemExist = EFalse;
        if (aItemChangedMessages)
            {
            // add the item changed message
            MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aId, EMPXItemDeleted,
                EMPXAbstractAlbum, KDBPluginUid);   
            }
        delete uri;
        }
    }


// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::DeleteAbstractAlbumL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbAbstractAlbum::DeleteAbstractAlbumL(
    TUint32 aAbstractAlbumId, TInt aDriveId)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::DeleteAbstractAlbumL");

    //before delete category, get the abstract album path
    HBufC* uri = GetNameL(aAbstractAlbumId);
    // delete the category
    if (uri)
        {
        CleanupStack::PushL(uri);
         
    if (aDriveId)
        DeleteCategoryL(aAbstractAlbumId, aDriveId);
    else
        {
        TInt drive = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
            ExecuteIntQueryL(KCriterionCategoryVolumeId, aAbstractAlbumId));
        DeleteCategoryL(aAbstractAlbumId, drive);
        }
//delete abstract album .alb from TN table
#ifdef RD_MPX_TNM_INTEGRATION
        // remove from thumbnail database table
        CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(
                *uri, KImageFileType );
        iTNManager->DeleteThumbnails( *source );
        CleanupStack::PopAndDestroy( source );
#endif //RD_MPX_TNM_INTEGRATION

       
        //delete abstract album .alb file from file system
        RFs rFs;
        User::LeaveIfError( rFs.Connect() );
        CleanupClosePushL( rFs );

        TInt err = BaflUtils::DeleteFile(rFs, *uri);
        if(KErrNone != err)
            {
            MPX_DEBUG2("CMPXDbAbstractAlbum::DeleteAbstractAlbumL,File not deleted from file system with err=%d", err);
            }

        CleanupStack::PopAndDestroy( &rFs );
        }

    CleanupStack::Pop(uri);
    return uri;
    }


// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::UpdateItemL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::UpdateItemL(
    TUint32 aId,
    const CMPXMedia& aMedia,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::UpdateItemL");

    CDesCArrayFlat* fields = new (ELeave) CDesCArrayFlat(EAbstractAlbumFieldCount);
    CleanupStack::PushL(fields);
    CDesCArrayFlat* values = new (ELeave) CDesCArrayFlat(EAbstractAlbumFieldCount);
    CleanupStack::PushL(values);

    // process the media parameter and construct the fields and values array
    GenerateAbstractAlbumFieldsValuesL(aMedia, *fields, *values);

    // construct the SET string
    HBufC* setStr = MPXDbCommonUtil::StringFromArraysLC(*fields, *values, KMCEqualSign, KMCCommaSign);

    if (setStr->Length())
        {
        // execute the query
        iDbManager.ExecuteQueryL(aDriveId, KQueryAbstractAlbumUpdate, setStr, aId);
        MPXDbCommonUtil::AddItemAlbumChangedMessageL(*aItemChangedMessages, aId, EMPXItemModified,
                EMPXAbstractAlbum, KDBPluginUid, ETrue, 0 );
        }

    CleanupStack::PopAndDestroy(setStr);
    CleanupStack::PopAndDestroy(values);
    CleanupStack::PopAndDestroy(fields);
    }

// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::UpdateMediaL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::UpdateMediaL(
    RSqlStatement& aRecord,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
     MPX_FUNC("CMPXDbAbstractAlbum::UpdateMediaL");

       TInt count(aAttrs.Count());
       for (TInt i = 0; i < count; ++i)
           {
           TInt contentId(aAttrs[i].ContentId());
           TUint attributeId(aAttrs[i].AttributeId());

           if (contentId == KMPXMediaIdGeneral)
               {
               if (attributeId & EMPXMediaGeneralId)
                   {
                   aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,
                       aRecord.ColumnInt64(EAbstractAlbumUniqueId));
                   }
               if (attributeId & EMPXMediaGeneralTitle)
                   {
                   TPtrC title(MPXDbCommonUtil::GetColumnTextL(aRecord, EAbstractAlbumName));
                   aMedia.SetTextValueL(KMPXMediaGeneralTitle, title);
                   MPX_DEBUG2("    AbstractAlbumName[%S]", &title);
                   }                          
               if (attributeId & EMPXMediaGeneralCount)
                   {
                   TInt songCount = GetSongsCountL(KDbManagerAllDrives,
                                aRecord.ColumnInt64(EAbstractAlbumUniqueId));
                   aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount, songCount );
                   }
               if (attributeId & EMPXMediaGeneralDrive)
                   {
                   TUint32 volId(aRecord.ColumnInt64(EAbstractAlbumVolumeId));
                   TInt driveId = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(), volId);

                   // LTAN-7GH6BZ, crash if eject memory card when adding song to existing AbstractAlbum
                   // due to special timing issue, it is possible drive number is -1 and create a
                   // panic when use for TDriveUnit
                   MPX_DEBUG3("volId = %d, driveId = %d", volId, driveId);

                   // handle possibly delay from framework notification
                   if (driveId < 0)
                       {
                       MPX_DEBUG1("invalid driveId, leave with KErrNotReady");
                       User::Leave(KErrNotReady);
                       }
                   TDriveUnit driveUnit(driveId);

                   aMedia.SetTextValueL(KMPXMediaGeneralDrive, driveUnit.Name());

                   }
               } // end if contentId == KMPXMediaIdGeneral          
           else if ( contentId == KMPXMediaIdMusic)
               {
               if (attributeId & EMPXMediaMusicAlbumArtist)
                   {         
                   TPtrC albumartist(MPXDbCommonUtil::GetColumnTextL(aRecord, EAbstractAlbumArtist));
                   aMedia.SetTextValueL(KMPXMediaMusicAlbumArtist, albumartist);
                   
                  
                   
                   MPX_DEBUG2("    albumartist[%S]", &albumartist);
                   } 
               if (attributeId & EMPXMediaMusicGenre)
                   {                 
                   TPtrC genre(MPXDbCommonUtil::GetColumnTextL(aRecord, EAbstractAlbumGenre));
                   aMedia.SetTextValueL(KMPXMediaMusicGenre, genre);
                   MPX_DEBUG2("    Genre[%S]", &genre);
                   }
               }
               
           } // end for
       aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
       aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAbstractAlbum);
    }


// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::GenerateAlbumFieldsValuesL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::GenerateAbstractAlbumFieldsValuesL(const CMPXMedia& aMedia, CDesCArray& aFields, CDesCArray& aValues)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::GenerateAbstractAlbumFieldsValuesL");
//support Winlogo use case
   if (aMedia.IsSupported(KMPXMediaGeneralTitle))
        {
        TPtrC truncatedName(aMedia.ValueText(KMPXMediaGeneralTitle).Left(KMCMaxTextLen));
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCCategoryName, truncatedName);
        }
//support Winlogo use case
   if (aMedia.IsSupported(KMPXMediaMusicAlbumArtist))
        {
        TPtrC truncatedAlbumArtist(aMedia.ValueText(KMPXMediaMusicAlbumArtist).Left(KMCMaxTextLen));
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicAlbumArtist, truncatedAlbumArtist);
        }
   if (aMedia.IsSupported(KMPXMediaMusicGenre))
        {
        TPtrC truncatedGenre(aMedia.ValueText(KMPXMediaMusicGenre).Left(KMCMaxTextLen));
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicGenre, truncatedGenre);    
        }
    }


// ----------------------------------------------------------------------------
// CMPXDbAlbum::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::CreateTableL(
    RSqlDatabase& aDatabase,
    TBool /* aCorruptTable */)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::CreateTableL");

    // create the table
    HBufC* query = PreProcessStringLC(KAbstractAlbumCreateTable);
    User::LeaveIfError(aDatabase.Exec(*query));
    CleanupStack::PopAndDestroy(query);

    // do not create an index on the Name field
    // as it only slows down the insert/update queries overall
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::CheckTableL
// ----------------------------------------------------------------------------
//
TBool CMPXDbAbstractAlbum::CheckTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::CheckTableL");

    HBufC* query = PreProcessStringLC(KAbstractAlbumCheckTable);
    TBool check(DoCheckTable(aDatabase, *query));
    CleanupStack::PopAndDestroy(query);

    return check;
    }


// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbAbstractAlbum::CMPXDbAbstractAlbum(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory) :
    CMPXDbCategory(aDbManager, aCategory)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::CMPXDbAbstractAlbum");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::ConstructL()
    {
    MPX_FUNC("CMPXDbAbstractAlbum::ConstructL");

    BaseConstructL();
#ifdef RD_MPX_TNM_INTEGRATION
    // Create Thumbnail Manager instance. This object is the observer.
    iTNManager = CThumbnailManager::NewL( *this );
#endif //RD_MPX_TNM_INTEGRATION
    }



// ---------------------------------------------------------------------------
// CMPXDbAbstractAlbum::ThumbnailReady 
// Callback but not used here
// ---------------------------------------------------------------------------
void CMPXDbAbstractAlbum::ThumbnailPreviewReady( 
        MThumbnailData& /*aThumbnail*/, TThumbnailRequestId /*aId*/ )
    {
    }
        

// ---------------------------------------------------------------------------
// CMPXDbAbstractAlbum::ThumbnailReady
// Callback but not used here
// ---------------------------------------------------------------------------
void CMPXDbAbstractAlbum::ThumbnailReady( TInt /*aError*/, 
        MThumbnailData& /*aThumbnail*/, TThumbnailRequestId /*aId*/ )
    {
    }
// End of File
