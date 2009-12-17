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
* Description:  This class is responsible for Artist table.
*
*
*/


#ifndef MPXDBARTIST_H
#define MPXDBARTIST_H

// INCLUDES
#include "mpxdbcategory.h"

// CLASS DECLARATION
class MMPXDbArtistObserver
    {
    public:
		virtual TInt HandleGetAlbumsCountForArtistL(TUint32 aId) = 0;
	};

/**
* Responsible for managing all music databases
*
* @lib MPXDbPlugin.lib
*/

class CMPXDbArtist :
    public CMPXDbCategory
    {
    public:
        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbArtist instance.
        */
        static CMPXDbArtist* NewL(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory, MMPXDbArtistObserver& aObserver);

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbArtist instance on the cleanup stack.
        */
        static CMPXDbArtist* NewLC(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory, MMPXDbArtistObserver& aObserver);

        /**
        * Destructor
        */
        virtual ~CMPXDbArtist();

    public:
        /**
        * Add a category item. If the record already exists, its counter will
        * be incremented.
        * @param aName: This is the name to be entered into the row
        * @param aDriveId: The Drive Id the name (category) belongs
        * @param aNewRecord: indicates to the caller if a new record is created.
        *        ETrue if a new row is created in the table; otherwise EFalse.
        * @param aCaseSensitive indicates whether case sensitivity should be taken
        *        into consideration when generating the unique row id
        * @return The unique id of the row added.
        */
        TUint32 AddItemL(const TDesC& aName, const TDesC& aArt, TInt aDriveId, TBool& aNewRecord,
            TBool aCaseSensitive = ETrue);

        /**
        * Gets the details for all category items.
        * @param aAttrs attributes to be returned
        * @param aMediaArray returns the requested attributes for all items
        */
        void GetAllCategoryItemsL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Update a category item.
        * @param aId: The ID of the category to update
        * @param aMedia: The media data
        * @param aDriveId: The Drive Id the name (category) belongs
        * @param aItemChangedMessages: if valid on return contains a updated message if the
        *        category was updated
        */
        void UpdateItemL(TUint32 aId, const CMPXMedia& aMedia, TInt aDriveId, CMPXMessageArray* aItemChangedMessages);

        /*
        * Checks if the specified artist item is unknown
        * @param aId identifies the artist item
        * @return ETrue if it is unknown, EFalse otherwise
        */
        TBool IsUnknownArtistL(TUint32 aId);

    private:

        /**
        * Updates the media with information from the table
        * @param aRecord record containing the source fields
        * @param aAttrs attributes to be returned
        * @param aMedia returns the requested attributes
        */
        void UpdateMediaL(RSqlStatement& aRecord, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Generate searching criteria for artist table from the given criteria
        * @param aMedia media data
        * @param aFields fields of Artist table
        * @param aValues values of each field of Artist table
        * @return a string containing the selection criteria. The ownership is passed to the caller.
        */
        void GenerateArtistFieldsValuesL(const CMPXMedia& aMedia,
            CDesCArray& aFields, CDesCArray& aValues);

        /**
        * Gets the Album counts for a Artist item
        * @param aId identifies the Artist item
        * @return value of the Album counts
        */
        TInt GetAlbumsCountL(TUint32 aId);

    private:    // from MMPXTable

        /**
        * @see MMPXTable
        */
        virtual void CreateTableL(RSqlDatabase& aDatabase, TBool aCorruptTable);

        /**
        * @see MMPXTable
        */
        virtual TBool CheckTableL(RSqlDatabase& aDatabase);

    private:

        /**
        * C++ constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        */
        CMPXDbArtist(CMPXDbManager& aDbManager, TMPXGeneralCategory aCategory, MMPXDbArtistObserver& aObserver);

        /**
        * Second phase constructor.
        */
        void ConstructL();

    private:

        /**
        * Column indexes in the category tables
        */
        enum TArtistColumns
            {
            EArtistUniqueId = KMPXTableDefaultIndex,
            EArtistName,
            EArtistSongCount,
            EArtistArt,
            EArtistFieldCount
            };

    private:    // Data
        MMPXDbArtistObserver& iObserver;
    };
#endif // MPXDBARTIST_H

// End of File
