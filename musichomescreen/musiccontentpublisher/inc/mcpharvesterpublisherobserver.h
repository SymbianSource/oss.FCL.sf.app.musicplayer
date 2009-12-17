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
* Description:  Updates applications and icons in Operator Tile.
*
*/


#ifndef C_MCPHARVESTERPUBLISHEROBSERVER_H
#define C_MCPHARVESTERPUBLISHEROBSERVER_H 

#include <e32base.h>
#include <LiwCommon.h>

// FORWARD DECLARATIONS
class CMusicContentPublisher;
class CLiwServiceHandler;
// CONSTANTS

// CLASS DECLARATION
/**
 */

class CMCPHarvesterPublisherObserver : public CBase, public MLiwNotifyCallback
	{
	public: // Constructors and destructor
		/**
		 * Two-phased constructor.
		 */
		static CMCPHarvesterPublisherObserver* NewL( CMusicContentPublisher* aHarvester );

		/**
		 * Two-phased constructor.
		 */
		static CMCPHarvesterPublisherObserver* NewLC( CMusicContentPublisher* aHarvester );
		
		/**
		 * Destructor.
		 */
		virtual ~CMCPHarvesterPublisherObserver();



	public:  //from MLiwNotifyCallback
	    
	    /**
	    * Handles notifications caused by an asynchronous Execute*CmdL call
	    * or an event.
	    *
	    * @param aCmdId The service command associated to the event.
	    * @param aEventId Occured event, see LiwCommon.hrh.
	    * @param aEventParamList Event parameters, if any, as defined per
	    *        each event.
	    * @param aInParamList Input parameters, if any, given in the
	    *        related HandleCommmandL.
	    * @return Error code for the callback.
	    */
	    virtual TInt HandleNotifyL(
	        TInt aCmdId,
	        TInt aEventId,
	        CLiwGenericParamList& aEventParamList,
	        const CLiwGenericParamList& aInParamList);
	
	public:
	    
	    /**
	    * Registers to CPS for add, delete , update and execute notifications
	    * @aFilter - filter for input parameter list
	    * @return void.
	    */
	    void RegisterL( CLiwDefaultMap* aFilter );
	    
	    
	private:
	    /**
        * Perform the second phase construction 
        */      
        void ConstructL();
        
        /**
		* Default constructor.
		*/
		CMCPHarvesterPublisherObserver( CMusicContentPublisher* aHarvester );
		
		// Prhohibited
		CMCPHarvesterPublisherObserver();

	    /**
	    * Cancel all the registered notifications. 
	    * @return void.
	    */
	    void ReleaseL();
	    
	    void InitLiwL();
	    
	    void ReleaseLiw();
		
	private: // data
	        
	    // Reference of 
	    // Owned
	    MLiwInterface* iInterface;
	    
	    // Reference of 
	    // Owned
	    CLiwServiceHandler* iLiwHandler;	            
	            
	    // Reference of the sapi data
	    // Not owned
	    CMusicContentPublisher* iHarvester;
	    
	    // Call back error code
	    TInt iError;
    };

#endif // C_MCPHARVESTERPUBLISHEROBSERVER_H 
