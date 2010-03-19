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
* Description:  Responsible for interation with the category tables:
*                Artist, Album, Genre and Composer
*
*/


// INCLUDE FILES
#include <sqldb.h>

#include <mpxlog.h>

#include "mpxdbcommonutil.h"
#include "mpxdbcommondef.h"
#include "mpxdbmanager.h"

#include "mpxcollectiondbdef.h"
#include "mpxmediamusicdefs.h"
#include "mpxdbpluginqueries.h"
#include "mpxdbutil.h"
#include "mpxdbalbum.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbAlbum* CMPXDbAlbum::NewL(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory,
    MMPXDbAlbumObserver& aObserver)
    {
    MPX_FUNC("CMPXDbAlbum::NewL");

    CMPXDbAlbum* self = CMPXDbAlbum::NewLC(aDbManager, aCategory, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbAlbum* CMPXDbAlbum::NewLC(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory,
    MMPXDbAlbumObserver& aObserver)
    {
    MPX_FUNC("CMPXDbAlbum::NewLC");

    CMPXDbAlbum* self = new (ELeave) CMPXDbAlbum(aDbManager, aCategory, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbAlbum::~CMPXDbAlbum()
    {
    MPX_FUNC("CMPXDbAlbum::~CMPXDbAlbum");
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbAlbum::CMPXDbAlbum(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory,
    MMPXDbAlbumObserver& aObserver) :
    CMPXDbCategory(aDbManager, aCategory),
    iObserver(aObserver)
    {
    MPX_FUNC("CMPXDbAlbum::CMPXDbAlbum");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::ConstructL()
    {
    MPX_FUNC("CMPXDbAlbum::ConstructL");

    BaseConstructL();
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::AddItemL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbAlbum::AddItemL(
    const TDesC& aName,
    TUint32 aArtist,
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
        HBufC* query = PreProcessStringLC(KQueryAlbumInsert);
        HBufC* name = MPXDbCommonUtil::ProcessSingleQuotesLC(aName);
		HBufC* art = MPXDbCommonUtil::ProcessSingleQuotesLC(aArt);

        iDbManager.ExecuteQueryL(aDriveId, *query, rowId, name, 1, aArtist, art);

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

        // Artist
        TUint32 artistId = recordset.ColumnInt64(EAlbumArtist);
	    
        // the current one is Unknown and the new one is Not Unknown.
        if ( IsUnknownArtistL( artistId ) && !IsUnknownArtistL( aArtist ) )
            {
            _LIT( KFormatArtistId, "Artist=%d" );
            HBufC* setStr = HBufC::NewLC(KFormatArtistId().Length() + KMCIntegerLen);
            setStr->Des().Format( KFormatArtistId, aArtist );
        
            iDbManager.ExecuteQueryL(aDriveId, KQueryAlbumUpdate, setStr, rowId);
            CleanupStack::PopAndDestroy(setStr);            
            }
	        
        // Album Art
        TPtrC art(KNullDesC);
        art.Set(MPXDbCommonUtil::GetColumnTextL(recordset, EAlbumArt));
	
        // the current one is Unknown and the new one is Not Unknown
        if ( art == KNullDesC && aArt != KNullDesC )
            {
            _LIT( KFormatArt, "Art=\'%S\'" );
            HBufC* setStr = HBufC::NewLC(256);
            setStr->Des().Format( KFormatArt, &aArt );
            
            iDbManager.ExecuteQueryL(aDriveId, KQueryAlbumUpdate, setStr, rowId);
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
// CMPXDbAlbum::DecrementSongsForCategoryL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::DecrementSongsForCategoryL(
    const TUint32 aId,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages,
    TBool& aItemExist,
    const TUint32 aArtist)
    {
    MPX_FUNC("CMPXDbAlbum::DecrementSongsForCategoryL");

    // if just one song uses this category. Use <= just in case
    if (GetSongsCountL(aDriveId, aId) <= 1)
        {
        aItemExist = EFalse;
        // delete the category
        DeleteCategoryL(aId, aDriveId);

        if (aItemChangedMessages)
            {
            // add the item changed message
            MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aId, EMPXItemDeleted,
                iCategory, KDBPluginUid);
            }
        }
    else
        {
        aItemExist = ETrue;
        
        // retrieve the existing record
        HBufC* query = PreProcessStringLC(KQueryCategoryItem);
        RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(*query, aId));
        CleanupStack::PopAndDestroy(query);
        
        CleanupClosePushL(recordset);
        
        if (recordset.Next() != KSqlAtRow)
            {
            User::Leave(KErrNotFound);
        }

        TUint32 artistId = recordset.ColumnInt64(EAlbumArtist);
        
        CleanupStack::PopAndDestroy(&recordset);
        
        // the current artist is equal to deleted song's artist
        if ( artistId == aArtist )
            {
            TUint32 newArtistId = ArtistForAlbumL(aId);
            
            _LIT( KFormatArtistId, "Artist=%d" );
            HBufC* setStr = HBufC::NewLC(KFormatArtistId().Length() + KMCIntegerLen);
            setStr->Des().Format(KFormatArtistId, newArtistId);
        
            iDbManager.ExecuteQueryL(aDriveId, KQueryAlbumUpdate, setStr, aId);
            CleanupStack::PopAndDestroy(setStr);
            
            if (aItemChangedMessages)
                {
                // add the item changed message
                MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aId, EMPXItemModified,
                    iCategory, KDBPluginUid);
                }
            }

        // decrement the number of songs for the category
        query = PreProcessStringLC(KQueryCategoryDecrementSongCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, aId);
        CleanupStack::PopAndDestroy(query);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::GetAllCategoryItemsL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::GetAllCategoryItemsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbAlbum::GetAllCategoryItemsL");

    HBufC* query = PreProcessStringLC(KQueryAlbumAll);
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(*query));
    CleanupStack::PopAndDestroy(query);

    CleanupClosePushL(recordset);
    ProcessRecordsetL(aAttrs, recordset, aMediaArray);
    CleanupStack::PopAndDestroy(&recordset);
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::UpdateItemL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::UpdateItemL(
    TUint32 aId, 
    const CMPXMedia& aMedia, 
    TInt aDriveId, 
    CMPXMessageArray* aItemChangedMessages)
	{
	MPX_FUNC("CMPXDbAlbum::UpdateItemL");

	CDesCArrayFlat* fields = new (ELeave) CDesCArrayFlat(EAlbumFieldCount);
	CleanupStack::PushL(fields);
	CDesCArrayFlat* values = new (ELeave) CDesCArrayFlat(EAlbumFieldCount);
	CleanupStack::PushL(values);

	// process the media parameter and construct the fields and values array
	GenerateAlbumFieldsValuesL(aMedia, *fields, *values);

	// construct the SET string
	HBufC* setStr = MPXDbCommonUtil::StringFromArraysLC(*fields, *values, KMCEqualSign, KMCCommaSign);

    if (setStr->Length())
        {
        // execute the query
        iDbManager.ExecuteQueryL(aDriveId, KQueryAlbumUpdate, setStr, aId);
	    TInt oldSongId = (aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2;	
	    MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aId, EMPXItemModified,
            EMPXAlbum, KDBPluginUid, oldSongId );
        }

	CleanupStack::PopAndDestroy(setStr);
	CleanupStack::PopAndDestroy(values);
	CleanupStack::PopAndDestroy(fields);
	}

// ----------------------------------------------------------------------------
// CMPXDbAlbum::GetAlbumsCountForArtistL
// ----------------------------------------------------------------------------
//
TInt CMPXDbAlbum::GetAlbumsCountForArtistL(TUint32 aArtistId)
	{
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryCategorySubcategoryItems, aArtistId));

    TInt prevId(0);
    TInt count(0);
	TInt err(KErrNone);
	
    while ((err = recordset.Next()) == KSqlAtRow)
		{
		TUint32 rowId(recordset.ColumnInt64(EAlbumUniqueId));
        if (prevId == rowId)
            {
            continue;
            }
		
		count++;
		prevId = rowId;
		}

    if (err != KSqlAtEnd)
        {
        User::Leave(err);
        }

	return count;

	}

// ----------------------------------------------------------------------------
// CMPXDbAlbum::GetSongsCountInAlbumMatchingArtistL
// ----------------------------------------------------------------------------
//
TInt CMPXDbAlbum::GetSongsCountInAlbumMatchingArtistL(TUint32 aArtistId, TUint32 aAlbumId)
	{
	MPX_FUNC("CMPXDbAlbum::GetSongsCountInAlbumMatchingArtistL");
	
	return ExecuteSumQueryL(KQuerySongsInArtistAlbum, aArtistId, aAlbumId);
	}

// ----------------------------------------------------------------------------
// CMPXDbAlbum::UpdateMediaL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::UpdateMediaL(
    RSqlStatement& aRecord,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbAlbum::UpdateMediaL");

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        TInt contentId(aAttrs[i].ContentId());
        TUint attributeId(aAttrs[i].AttributeId());

        if (contentId == KMPXMediaIdGeneral)
            {
            if (attributeId & EMPXMediaGeneralId)
                {
                MPX_DEBUG1("    EMPXMediaGeneralId");
                
                aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,
                    aRecord.ColumnInt64(EAlbumUniqueId));
                }
            if (attributeId & EMPXMediaGeneralTitle)
                {
                MPX_DEBUG1("    EMPXMediaGeneralTitle");
                
                TPtrC album( MPXDbCommonUtil::GetColumnTextL(aRecord, EAlbumName) );
                aMedia.SetTextValueL(KMPXMediaGeneralTitle,
                    MPXDbCommonUtil::GetColumnTextL(aRecord, EAlbumName));
                MPX_DEBUG2("	Album[%S]", &album );
                }
            if (attributeId & EMPXMediaGeneralCount)
                {
                MPX_DEBUG1("    EMPXMediaGeneralCount");

				TInt songCount = GetSongsCountL(KDbManagerAllDrives,
					aRecord.ColumnInt64(EAlbumUniqueId));
                aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount, songCount );
				MPX_DEBUG2("	SongCount[%d]", songCount );
                }
            } // end if contentId == KMPXMediaIdGeneral
		else if ( contentId == KMPXMediaIdMusic )
			{
			if (attributeId & EMPXMediaMusicArtist)
				{				
				MPX_DEBUG1("	EMPXMediaMusicArtist");

				TPtrC artistName(KNullDesC);
				
				// if album is unknown, ignore arist name
				if (MPXDbCommonUtil::GetColumnTextL(aRecord, EAlbumName) != KNullDesC)
					{
					artistName.Set(MPXDbCommonUtil::GetColumnTextL(aRecord, EAlbumArtistName));
					}
				
				aMedia.SetTextValueL(KMPXMediaMusicArtist, artistName);
				MPX_DEBUG2("	Artist[%S]", &artistName);
				}
			if (attributeId & EMPXMediaMusicAlbumArtFileName)
				{
				MPX_DEBUG1("	EMPXMediaMusicAlbumArtFileName");

				TPtrC art(KNullDesC);
				
				// if album is unknown, ignore album art name
				if (MPXDbCommonUtil::GetColumnTextL(aRecord, EAlbumName) != KNullDesC)
					{
					art.Set(MPXDbCommonUtil::GetColumnTextL(aRecord, EAlbumArt));
					}
				
				aMedia.SetTextValueL(KMPXMediaMusicAlbumArtFileName, art);
				MPX_DEBUG2("	Art[%S]", &art);
				}
			}
		} // end for

    aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, iCategory);
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::GenerateAlbumFieldsValuesL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::GenerateAlbumFieldsValuesL(const CMPXMedia& aMedia, CDesCArray& aFields, CDesCArray& aValues)
	{
	if (aMedia.IsSupported(KMPXMediaMusicAlbumArtFileName))
		{
		const TDesC& albumArtFilename = aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Left(KMCMaxTextLen);
		MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicArt, albumArtFilename);
		}
		
	if (aMedia.IsSupported(KMPXMediaMusicArtist))
	    {
	    const TDesC& artistName = aMedia.ValueText(KMPXMediaMusicArtist).Left(KMCMaxTextLen);
	    TUint32 artistId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXArtist,
	            artistName, ETrue);
	    MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicArtist, artistId);	    
	    }
	}

// ----------------------------------------------------------------------------
// CMPXDbAlbum::IsUnknownArtistL
// ----------------------------------------------------------------------------
//
TBool CMPXDbAlbum::IsUnknownArtistL(TUint32 aId)
	{
	return iObserver.HandleIsUnknownArtistL(aId);
	}

// ----------------------------------------------------------------------------
// CMPXDbAlbum::ArtistForAlbumL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbAlbum::ArtistForAlbumL(const TUint32 aId)
    {
    return iObserver.HandleArtistForAlbumL(aId);
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::CreateTableL(
    RSqlDatabase& aDatabase,
    TBool /* aCorruptTable */)
    {
    MPX_FUNC("CMPXDbCategory::CreateTableL");

    // create the table
    HBufC* query = PreProcessStringLC(KAlbumCreateTable);
    User::LeaveIfError(aDatabase.Exec(*query));
    CleanupStack::PopAndDestroy(query);

    // do not create an index on the Name field
    // as it only slows down the insert/update queries overall
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::CheckTableL
// ----------------------------------------------------------------------------
//
TBool CMPXDbAlbum::CheckTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbCategory::CheckTableL");

    HBufC* query = PreProcessStringLC(KAlbumCheckTable);
    TBool check(DoCheckTable(aDatabase, *query));
    CleanupStack::PopAndDestroy(query);

    return check;
    }

// End of File
