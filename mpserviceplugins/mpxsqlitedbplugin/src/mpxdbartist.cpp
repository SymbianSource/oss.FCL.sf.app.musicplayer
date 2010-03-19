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
* Description:  Responsible for interation with the Artist table
*
*
*/


// INCLUDE FILES
#include <sqldb.h>

#include <mpxlog.h>

#include "mpxdbcommonutil.h"
#include "mpxdbcommondef.h"
#include "mpxmediamusicdefs.h"
#include "mpxdbmanager.h"

#include "mpxcollectiondbdef.h"
#include "mpxdbpluginqueries.h"
#include "mpxdbutil.h"
#include "mpxdbartist.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbArtist* CMPXDbArtist::NewL(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory,
    MMPXDbArtistObserver& aObserver)
    {
    MPX_FUNC("CMPXDbArtist::NewL");

    CMPXDbArtist* self = CMPXDbArtist::NewLC(aDbManager, aCategory, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbArtist* CMPXDbArtist::NewLC(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory,
    MMPXDbArtistObserver& aObserver)
    {
    MPX_FUNC("CMPXDbArtist::NewLC");

    CMPXDbArtist* self = new (ELeave) CMPXDbArtist(aDbManager, aCategory, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbArtist::~CMPXDbArtist()
    {
    MPX_FUNC("CMPXDbArtist::~CMPXDbArtist");
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbArtist::CMPXDbArtist(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory,
    MMPXDbArtistObserver& aObserver) :
    CMPXDbCategory(aDbManager, aCategory), 
    iObserver(aObserver)
    {
    MPX_FUNC("CMPXDbArtist::CMPXDbArtist");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::ConstructL()
    {
    MPX_FUNC("CMPXDbArtist::ConstructL");

    BaseConstructL();
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::AddItemL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbArtist::AddItemL(
    const TDesC& aName,
    const TDesC& aArt,
    TInt aDriveId,
    TBool& aNewRecord,
    TBool aCaseSensitive)
    {
    MPX_FUNC("CMPXDbAlbum::AddItemL");
    
    // try to find the item first
    TUint32 rowId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), iCategory,
        aName, aCaseSensitive));
    aNewRecord = !CategoryItemExistsL(aDriveId, rowId);

    if (aNewRecord)
        {      
        // insert new
        HBufC* query = PreProcessStringLC(KQueryArtistInsert);
        HBufC* name = MPXDbCommonUtil::ProcessSingleQuotesLC(aName);
        HBufC* art = MPXDbCommonUtil::ProcessSingleQuotesLC(aArt);

        iDbManager.ExecuteQueryL(aDriveId, *query, rowId, name, 1, art);

        CleanupStack::PopAndDestroy(art);
        CleanupStack::PopAndDestroy(name);
        CleanupStack::PopAndDestroy(query);
        }
    else
        {
        // retrieve the existing record
        HBufC* query = NULL;
        query = PreProcessStringLC(KQueryCategoryItem);
        RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(*query, rowId));
        CleanupStack::PopAndDestroy(query);
        
        CleanupClosePushL(recordset);
        
        if (recordset.Next() != KSqlAtRow)
            {
            User::Leave(KErrNotFound);
        }
            
        // Album Art
        TPtrC art(KNullDesC);
        art.Set(MPXDbCommonUtil::GetColumnTextL(recordset, EArtistArt));
    
        // the current one is Unknown and the new one is Not Unknown
        if ( art == KNullDesC && aArt != KNullDesC )
            {
            _LIT( KFormatArt, "Art=\'%S\'" );
            HBufC* setStr = HBufC::NewLC(256);
            setStr->Des().Format( KFormatArt, &aArt );
            
            iDbManager.ExecuteQueryL(aDriveId, KQueryArtistUpdate, setStr, rowId);
            CleanupStack::PopAndDestroy(setStr);            
            }
        
        CleanupStack::PopAndDestroy(&recordset);
        
        // increment the number of songs for the category
        query = PreProcessStringLC(KQueryCategoryIncrementSongCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, rowId);
        CleanupStack::PopAndDestroy(query);
        }

    return rowId;
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::IsUnknownArtistL
// ----------------------------------------------------------------------------
//
TBool CMPXDbArtist::IsUnknownArtistL(TUint32 aId)
    {
	MPX_FUNC("CMPXDbArtist::IsUnknownArtistL");
	
    HBufC* name = GetNameL(aId);
	
	TInt ret = EFalse;
	if (*name == KNullDesC)
		{
		ret = ETrue;
		}
	delete name;
	
	return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::UpdateMediaL
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::UpdateMediaL(
    RSqlStatement& aRecord,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbArtist::UpdateMediaL");

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        TInt contentId(aAttrs[i].ContentId());
        TUint attributeId(aAttrs[i].AttributeId());

        if (contentId == KMPXMediaIdGeneral)
            {
            if (attributeId & EMPXMediaGeneralId)
                {
                MPX_DEBUG1("	EMPXMediaGeneralId");
				
                aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,
                    aRecord.ColumnInt64(EArtistUniqueId));
				MPX_DEBUG2("	Id[%d]", aRecord.ColumnInt64(EArtistUniqueId));
                }
            if (attributeId & EMPXMediaGeneralTitle)
                {
                MPX_DEBUG1("	EMPXMediaGeneralTitle");
				
                TPtrC artist( MPXDbCommonUtil::GetColumnTextL(aRecord, EArtistName) );
                aMedia.SetTextValueL(KMPXMediaGeneralTitle,
                    MPXDbCommonUtil::GetColumnTextL(aRecord, EArtistName));

				MPX_DEBUG2("	Artist[%S]", &artist);
                }
            if (attributeId & EMPXMediaGeneralCount)
                {
//				TInt albumCount = GetAlbumsCountL(aRecord.ColumnInt64(EArtistUniqueId));
//              aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount,albumCount);
//              MPX_DEBUG1("	EMPXMediaGeneralCount");
//				MPX_DEBUG2("	AlbumCount[%d]", albumCount);
				TInt songCount = aRecord.ColumnInt64(EArtistSongCount);
				aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount, songCount); // mod by anjokela
				
                MPX_DEBUG1("	EMPXMediaGeneralCount");
				MPX_DEBUG2("	SongCount[%d]", songCount);
                }
            } // end if contentId == KMPXMediaIdGeneral
        else if ( contentId == KMPXMediaIdMusic )
            {
            if (attributeId & EMPXMediaMusicAlbumArtFileName)
                {
                MPX_DEBUG1("    EMPXMediaMusicAlbumArtFileName");
                TPtrC art(KNullDesC);                
                art.Set(MPXDbCommonUtil::GetColumnTextL(aRecord, EArtistArt));
                aMedia.SetTextValueL(KMPXMediaMusicAlbumArtFileName, art);
                MPX_DEBUG2("    Art[%S]", &art);
                }
            }
		} // end for

    aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, iCategory);
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::GenerateArtistFieldsValuesL
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::GenerateArtistFieldsValuesL(const CMPXMedia& aMedia, CDesCArray& aFields, CDesCArray& aValues)
    {
    if (aMedia.IsSupported(KMPXMediaMusicAlbumArtFileName))
        {
        const TDesC& albumArtFilename = aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Left(KMCMaxTextLen);
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicArt, albumArtFilename);
        }
    }
	
// ----------------------------------------------------------------------------
// CMPXDbArtist::GetAlbumsCountL
// ----------------------------------------------------------------------------
//
TInt CMPXDbArtist::GetAlbumsCountL(TUint32 aId)
    {
    MPX_FUNC("CMPXDbArtist::GetAlbumsCountL");
	
	return iObserver.HandleGetAlbumsCountForArtistL(aId);
    }
	
// ----------------------------------------------------------------------------
// CMPXDbArtist::GetAllCategoryItemsL
// ----------------------------------------------------------------------------
//	
void CMPXDbArtist::GetAllCategoryItemsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbArtist::GetAllCategoryItemsL");
    HBufC* query = PreProcessStringLC(KQueryArtistAll);
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(*query));
    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(recordset);
    ProcessRecordsetL(aAttrs, recordset, aMediaArray);
    CleanupStack::PopAndDestroy(&recordset);
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::UpdateItemL
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::UpdateItemL(
    TUint32 aId, 
    const CMPXMedia& aMedia, 
    TInt aDriveId, 
    CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbAlbum::UpdateItemL");

    CDesCArrayFlat* fields = new (ELeave) CDesCArrayFlat(EArtistFieldCount);
    CleanupStack::PushL(fields);
    CDesCArrayFlat* values = new (ELeave) CDesCArrayFlat(EArtistFieldCount);
    CleanupStack::PushL(values);

    // process the media parameter and construct the fields and values array
    GenerateArtistFieldsValuesL(aMedia, *fields, *values);

    // construct the SET string
    HBufC* setStr = MPXDbCommonUtil::StringFromArraysLC(*fields, *values, KMCEqualSign, KMCCommaSign);

    if (setStr->Length())
        {
        // execute the query
        iDbManager.ExecuteQueryL(aDriveId, KQueryArtistUpdate, setStr, aId);
        TInt oldSongId = (aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2;   
        MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aId, EMPXItemModified,
            EMPXArtist, KDBPluginUid, oldSongId );
        }

    CleanupStack::PopAndDestroy(setStr);
    CleanupStack::PopAndDestroy(values);
    CleanupStack::PopAndDestroy(fields);
    }
// ----------------------------------------------------------------------------
// CMPXDbAlbum::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::CreateTableL(
    RSqlDatabase& aDatabase,
    TBool /* aCorruptTable */)
    {
    MPX_FUNC("CMPXDbCategory::CreateTableL");

    // create the table
    HBufC* query = PreProcessStringLC(KArtistCreateTable);
    User::LeaveIfError(aDatabase.Exec(*query));
    CleanupStack::PopAndDestroy(query);

    // do not create an index on the Name field
    // as it only slows down the insert/update queries overall
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::CheckTableL
// ----------------------------------------------------------------------------
//
TBool CMPXDbArtist::CheckTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbCategory::CheckTableL");

    HBufC* query = PreProcessStringLC(KArtistCheckTable);
    TBool check(DoCheckTable(aDatabase, *query));
    CleanupStack::PopAndDestroy(query);

    return check;
    }

// End of File
