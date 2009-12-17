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


#ifndef C_MUSICCONTENTPUBLISHER_H
#define C_MUSICCONTENTPUBLISHER_H 

#include <contentharvesterplugin.h>
#include <mpxcollectionobserver.h>

class MLiwInterface;
class CLiwGenericParamList;

// FORWARD DECLARATIONS
class MMPXCollectionUtility;
class MMPXCollectionUiHelper;
class CMPXCollectionPath;

/**
 *  Music Content Publisher (a Content Harvester plugin)
 *
 *  @lib musiccontentpublisher.dll
 *  @since S60 S60 v5.0
 */
class CMpxCollectionPublisher : public CContentHarvesterPlugin
                               ,public MMPXCollectionObserver
    {

public:

    /**
     * Two-phased constructor.
     */
    static CMpxCollectionPublisher* NewL( MLiwInterface* aCPSInterface );

    /**
     * Destructor.
     */
    virtual ~CMpxCollectionPublisher();

    // from base class CContentHarvesterPlugin
    /**
     * From CContentHarvesterPlugin
     * This is called by the Content Harvester Server to signal an update. 
     */
    void UpdateL();
    
    //From Base class MMPXCollectionMediaObserver
    /**
     * From MMPXCollectionMediaObserver 
     * It handles open collection event.
     */
    void HandleOpenL( const CMPXMedia& aEntries, TInt aIndex, 
    		 TBool aComplete, TInt  aError );
    
    /**
     * From MMPXCollectionMediaObserver 
     * It handles open collection event.
     */                
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist,
    		 TInt aError ); 
    
    /**
     * From MMPXCollectionMediaObserver 
     * It handles MediaL event.
     *
     * @param aMedia object containing collection's informations
     * @param aError error code
     */
    void HandleCollectionMediaL( const CMPXMedia& aMedia,
    		TInt aError );
    
    /** 
    *  Handle collection message
    * 
    *  @param aMsg collection message, ownership not transferred. 
    *         Please check aMsg is not NULL before using it. 
    *         If aErr is not KErrNone, plugin might still call back with more
    *         info in the aMsg.
    *  @param aErr system error code.
    */
    void HandleCollectionMessage( CMPXMessage* aMsg, TInt /*aErr*/);
        


private:

    /**
     * Default constructor.
     */
    CMpxCollectionPublisher( MLiwInterface* aCPSInterface );

    /**
     * Perform the second phase construction 
     */
    void ConstructL();
    
    /**
	 * Logic of HandleCollectionMessage, to avoid the function leaves
	 */
	void DoHandleGeneralMessageL(const CMPXMessage& aMsg);

	/**
	 * Logic of HandleCollectionMessage, to avoid the function leaves
	 * it handles item change operation
	 */
	void DoHandleItemChangedMessageL(const CMPXMessage& aMsg);   
    
	/**
	 * Logic of HandleCollectionMessageL, to avoid the function leaves
	 */
	void HandleCollectionMessageL(CMPXMessage* aMsg);  		
	
    /**
     * Called when a number needs to be updated.
	 *
	 * @since S60 5.0
	 * @param aNum containing the number.
	 */
	void PublishNumberL( const TInt aNumb );

private: // data

    /**
     * Instance of CPS interface used for update with CPS.
     * Not own
     */
    MLiwInterface* iCPSInterface;
    
    /**
    * MPX Collection utility
    * Own.
    */
    MMPXCollectionUtility* iCollectionUtility;
    
    /**
    * MPX Collection helper
    * Own.
    */
    MMPXCollectionUiHelper* iCollectionUiHelper;
    
    /**
     * Path to Music collection
     * Own.
     */
    CMPXCollectionPath* iAllSongsPath;

	/**
	* Indicates whether we're in a massive refresh operation
	*/
	TBool iRefreshOperation;
	
    /**
    * Array containig wanted collection attributes
    * Own.
    */	    
	RArray<TMPXAttribute> iDefaultAttrs;

    };

#endif // C_MUSICCONTENTPUBLISHER_H

//  End of File
