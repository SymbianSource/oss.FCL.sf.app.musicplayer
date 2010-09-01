/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manages MCP plugins, and content publishing.
*
*/

#ifndef C_MUSICCONTENTMAP_H
#define C_MUSICCONTENTMAP_H 

#include <liwvariant.h>
#include <e32hashtab.h>

// CLASS DECLARATION

/**
* Data class to construct and hold variant values persistently.
*/
class RMusicContentVariant {
  public:

    /**
     * Constructor
     *
     * Constructs a null variant value.
     */
    RMusicContentVariant();

    /**
     * Reset variant to null value.
     * Frees content.
     */
    void Reset();

    /**
     * Set variant to integer value.
     * @param New value.
     * @return Whether the value was changed.
     * Frees content of previous value.
     */
    TBool SetL( TInt aInt);

    /**
     * Set variant to string value.
     * @param New value. An internal copy of the string is made.
     * @return Whether the value was changed.
     * Frees content of previous value.
     */
    TBool SetL( const TDesC& aDesc );

    /**
     * Set variant to map value.
     * @param New value. Reference counter of the map is increased.
     * @return Whether the value was changed.
     * Frees content of previous value.
     */
    TBool SetL( CLiwMap * aMap );
    
    /**
     * Check whether value has been modified since the last ResetModified.
     * @return ETrue if value has been modified.
     */
    TBool IsModified() const;

    /**
     * Reset modified flag.
     * After calling this method IsModified returns EFalse.
     */
    void ResetModified();
    
    /**
     * Return whether the value is null.
     * @return ETrue if value is null.
     */
    TBool IsNull() const;

    /**
     * Return the value as TLiwVariant
     * @return Value encapsulated in TLiwVariant.
     */
    TLiwVariant LiwVariant() const;
    
  private:
  	
  	enum TType {ETypeNull, ETypeInt, ETypeBufC, ETypeMap};
  	
    TType   iType; // type of the value
    TBool   iModified; // whether value has been modified
    
    // Data area
    union {
    	TInt      iInt;
    	HBufC   * iBufC; // owned
    	CLiwMap * iMap;  // reference counted
    } iValue;
    
};

// CLASS DECLARATION

/**
 *  Persistent map of variable content
 *
 *  @lib musiccontentpublisher.dll
 *  @since S60 S60 v5.0
 */

class RMusicContentMap {
  public:

    /**
     * Constructs an empty map
     */
    RMusicContentMap();

    /**
     * Resets a single value in the map in case it exists
     * @param aKey The key of the value to be reset
     * Does nothing if the value does not exist
     */
    void Reset( TPtrC8 aKey );

    /**
     * Sets a string value in the map
     * @param aKey The key of the value
     * @param aValue The value for the key. The string is copied.
     */
    void SetL( TPtrC8 aKey, const TDesC& aValue );

    /**
     * Sets an integer value in the map
     * @param aKey The key of the value
     * @param aValue The value for the key.
     */
    void SetL( TPtrC8 aKey, const TInt& aValue );

    /**
     * Sets an map value in the map
     * @param aKey The key of the value
     * @param aValue The value for the key. The reference count of the map is increased.
     */
    void SetL( TPtrC8 aKey, CLiwMap * aMap ); 
    
    /**
     * Check whether value has been modified since last reading the values.
     * @return ETrue if value has been modified.
     */
    TBool IsModified() const;
    
    /**
     * Reads all those entries from the map that have been modified
     * since previous reading of values. Null values are not read.
     *
     * @return A freshly allocated map with modified values. Null is returned if there are no modifications.
     */
    CLiwMap * GetModifiedLC ();
   
    /**
     * Reads all non-null entries from the map.
     *
     * @return aMap  A freshly allocated map with entries. Null is returned if map is empty.
     */
    CLiwMap * GetAllLC();
    
    /**
     * Closes the map discarding all its content.
     */
    void Close();
  
  private:
  	
  	RMusicContentVariant& AtL (TPtrC8 aKey);
  	
  	RHashMap<TPtrC8, RMusicContentVariant> iMap;
  	TBool                                  iModified;
};

#endif
