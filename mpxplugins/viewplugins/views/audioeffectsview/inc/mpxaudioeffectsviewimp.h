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
* Description:  Audio Effect view class.
*
*/



#ifndef C_CMPXAUDIOEFFECTSVIEWIMP_H
#define C_CMPXAUDIOEFFECTSVIEWIMP_H


//INCLUDES
#include <mpxplaybackframeworkdefs.h>
#include <mpxviewactivationobserver.h>
#include "mpxaudioeffectsview.h"


// FORWARD DECLARATIONS
class CMPXAudioEffectsModel;
class CMPXAudioEffectsViewContainer;
class MMPXPlaybackUtility;
class CMPXAudioEffectEngine;
class CCustomCommandUtility;
class MMPXViewUtility;

/**
*  CMPXAudioEffectsViewImp, View class for audio settings 
*  
*  @lib mpxaudioeffectsview.dll
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS( CMPXAudioEffectsViewImp ) : public CMPXAudioEffectsView, public MMPXViewActivationObserver
    {
public:

    /**
    * Two-phased constructor
    *
    * @return object of constructed
    */
    static CMPXAudioEffectsViewImp* NewL();

    /**
    * Two-phased constructor
    *
    * @return object of constructed
    */
    static CMPXAudioEffectsViewImp* NewLC();

    /**
    * Destructor
    *
    */
    virtual ~CMPXAudioEffectsViewImp();

protected:
    /**
     * From CAknView
     * Processes user commands.
     *
     * @since 3.0
     * @param aCommand ID of the command to respond to.
     */
    virtual void ProcessCommandL( TInt aCommand );

private:

    /*
    *  Second-phased Constructor
    */
    void ConstructL();
    
    /*
    *  Privatre Constructor
    */
    CMPXAudioEffectsViewImp();

    /**
    * Change selected item
    */
    void ChangeSelectedItemL();

// from CAknView

    /**
    * From CAknView
    */
    TUid Id() const;
    
    /**
    * From CAknView
    */
    void HandleCommandL( TInt aCommand );
    
    /**
    * From CAknView
    */    
    void DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage );
    
    /**
    * From CAknView
    */
    void DoDeactivate();
    
    // from MMPXViewActivationObserver

    /**
     * From MMPXViewActivationObserver
     * Handle view activation.
     *
     * @param aCurrentViewType Current view type Uid.
     * @param aPreviousViewType Previous view type Uid.
     */
    void HandleViewActivation(
        const TUid& aCurrentViewType,
        const TUid& aPreviousViewType );
    /**
     * @see MMPXViewActivationObserver
     */
    void HandleViewUpdate(
        TUid aViewUid,
        MMPXViewActivationObserver::TViewUpdateEvent aUpdateEvent,
        TBool aLoaded,
        TInt aData);

private: // data

    CMPXAudioEffectsViewContainer* iContainer;
    CMPXAudioEffectsModel* iModel;
    MMPXPlaybackUtility* iPlaybackUtility;  // not own
    
    TVwsViewId iPreviousView;
    TInt iResourceOffset;
    MMPXViewUtility* iViewUtility;
    TBool iGoPreviousView;
    };

#endif    // C_CMPXAUDIOEFFECTSVIEWIMP_H

// End of File
