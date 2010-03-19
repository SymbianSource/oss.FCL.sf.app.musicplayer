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
* Description:  This class is responsible for all category tables (Artist,
*                Album, Genre, Composer).
*
*
*/


#ifndef MPXDBALBUM_H
#define MPXDBALBUM_H

// INCLUDES
#include "mpxdbcategory.h"

// CLASS DECLARATION

class MMPXDbAlbumObserver
    {
    public:
        virtual TInt HandleIsUnknownArtistL(TUint32 aId) = 0;    
        virtual TUint32 HandleArtistForAlbumL( const TUint32 aId ) = 0;
    };
/**
* Responsible for managing all music databases
*
* @lib MPXDbPlugin.lib
*/

class CMPXDbAlbum :
    public CMPXDbCategory
    {
    public:

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbCategory instance.
        */
        static CMPXDbAlbum* NewL(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory, MMPXDbAlbumObserver& aObserver);

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbCategory instance on the cleanup stack.
        */
        static CMPXDbAlbum* NewLC(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory, MMPXDbAlbumObserver& aObserver);

        /**
        * Destructor
        */
        virtual ~CMPXDbAlbum();

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
        TUint32 AddItemL(const TDesC& aName, TUint32 aArtist, const TDesC& aArt, TInt aDriveId, TBool& aNewRecord,
            TBool aCaseSensitive = ETrue);

        /**
        * Decrement the number of songs for the item. If the count gets to 0, remove
        * the item.
        * @param aId: The ID of the category to delete.
        * @param aDriveId: The drive Id the name (category) belongs to. Songs on different
        *                  drives may belong to the same album or artist; consequently, one
        *                  row for each artist/album id and drive ID pair will exist in the
        *                  lookup table with a count existing for each row that tracks the
        *                  number number of songs on that drive with that album/artist.
        * @param aItemChangedMessages if valid on return contains a deleted message if the
        *                  category was deleted
        * @param aItemExist Out parameter, ETrue if the category is not deleted after the delete,
        *        EFalse otherwise
		* @param aArtist: The ID of the artist
        */
        void DecrementSongsForCategoryL(TUint32 aId, TInt aDriveId,
            CMPXMessageArray* aItemChangedMessages, TBool& aItemExist, const TUint32 aArtist);
        
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

	public:
		/**
	* Get albums count for a specified artist
	* @param aId: The ID of the artist
	*/		
	    TInt GetAlbumsCountForArtistL(TUint32 aArtistId);

		/**
	* Get songs count for a specified album and a specified artist
	* @param aArtistId: The ID of the artist
	* @param aAlbumId: The ID of the album
	*/		
		TInt GetSongsCountInAlbumMatchingArtistL(TUint32 aArtistId, TUint32 aAlbumId);
		
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
        * Generate searching criteria for album table from the given criteria
        * @param aMedia media data
        * @param aFields fields of Album table
        * @param aValues values of each field of Album table
        * @return a string containing the selection criteria. The ownership is passed to the caller.
        */
		void GenerateAlbumFieldsValuesL(const CMPXMedia& aMedia, 
			CDesCArray& aFields, CDesCArray& aValues);


		TBool IsUnknownArtistL(TUint32 aId);
		
		TUint32 ArtistForAlbumL(const TUint32 aId);

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
        CMPXDbAlbum(CMPXDbManager& aDbManager, TMPXGeneralCategory aCategory, MMPXDbAlbumObserver& aObserver);

        /**
        * Second phase constructor.
        */
        void ConstructL();

    public:

        /**
        * Column indexes in the category tables
        */
        enum TAlbumColumns
            {
            EAlbumUniqueId = KMPXTableDefaultIndex,
            EAlbumName,
            EAlbumSongCount,
            EAlbumArtist,
            EAlbumArt,
            EAlbumArtistName,
            EAlbumFieldCount
            };

    private:    // Data
        MMPXDbAlbumObserver& iObserver;
    };
#endif // MPXDBALBUM_H

// End of File
