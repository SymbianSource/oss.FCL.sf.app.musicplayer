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
* Description:  Definition of UI helper utility for DRM functions
*
*/



#ifndef C_CMPXDRMUIHELPER_H
#define C_CMPXDRMUIHELPER_H

#ifdef CAMESE_IN_DRM_UTILITY
#include <drmuicheckrightsobserver.h>
#endif

// FORWARD DECLARATIONS
class CMPXMedia;
#ifdef CAMESE_IN_DRM_UTILITY
namespace DRM
    {
    class CDrmUiHandling;
    }
#else
class CDRMHelper;
#endif

class CMPXCommonUiHelper;

#ifdef __CAMESE_SUPERDISTRIBUTION_SUPPORT  
class CCameseUiHelper;
#endif

// CLASS DECLARATION

/**
 *  DRM UI helper utility class.
 *
 *  @lib mpxcommonui.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXDrmUiHelper ) : public CBase
#ifdef CAMESE_IN_DRM_UTILITY
,public DRM::MDrmUiCheckRightsObserver
#endif
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXDrmUiHelper* NewL( CMPXCommonUiHelper* aHelper );

    /**
     * Destructor.
     */
    virtual ~CMPXDrmUiHelper();
    
    /**
     * Handles displaying the error message for given error code
     * according to the media object provided
     *
     * @since 3.1
     * @param aError Error code
     * @param aMedia MPXMedia object
     * @param aFile Handle to file
     * @return KErrNone if no error, or else if a dialog is displayed will
     *         return the Button ID used to dismiss dialog.
     */
    TInt HandleDrmErrorL( TInt aError, 
                          const CMPXMedia* aMedia=NULL,
                          RFile* aFile=NULL );

private:

    /**
     * C++ default constructor.
     */
    CMPXDrmUiHelper( CMPXCommonUiHelper* aHelper );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // New methods

    /**
     * Handles OMA specific error messages
     * @param aMedia MPXMedia object
     * @param aFile Handle to file
     * @return KErrNone if no error, or else if a dialog is displayed will
     *         return the Button ID used to dismiss dialog.
     */
    TInt HandleOMAErrorL( TInt aError, 
            const CMPXMedia* aMedia, 
            RFile* aFile );

    /**
     * Handles WMA specific error messages
     * @param aMedia MPXMedia object
     * @param aFile Handle to file
     * @return KErrNone if no error, or else if a dialog is displayed will
     *         return the Button ID used to dismiss dialog.
     */
    TInt HandleWMAErrorL( TInt aError, 
            const CMPXMedia* aMedia,
            RFile* aFile );

#ifdef CAMESE_IN_DRM_UTILITY
private:	// From MDrmUiCheckRightsObserver

    DRM::TCheckRightsAction RightsLeft(
            TInt aOperationId,
            TBool aUnconstrained,
            TTimeIntervalSeconds aTime,
            TInt aCounts,
            TTimeIntervalSeconds aAccumulated );

    DRM::TCheckRightsAction RightsNotValid(
            TInt aOperationId,
            DRM::TCheckRightsStatus aRightsStatus,
            TInt aReason );

    // Unused observation methods

    DRM::TEmbeddedPreviewAction EmbeddedPreviewAvailable(
            TInt aOperationId,
            const TDesC& aUniqueId,
            TInt aRightsStatus,
            TInt aReason );

    DRM::TPreviewRightsAction PreviewRightsUrlAvailable(
            TInt aOperationId,
            const TDesC& aPreviewRightsUrl,
            TInt aRightsStatus,
            TInt aReason );

    DRM::TSilentRightsAction SilentRightsUrlAvailable(
            TInt aOperationId,
            const TDesC& aSilentRightsUrl,
            TInt aRightsStatus,
            TInt aReason );

    DRM::TRightsAction RightsUrlAvailable(
            TInt aOperationId,
            const TDesC& aRightsUrl,
            TInt aRightsStatus,
            TInt aReason,
            TInt aUrlType );

    void PreviewRightsAvailable(
            TInt aOperationId,
            TInt aError );

    void SilentRightsAvailable(
            TInt aOperationId,
            TInt aError );

    void RightsAvailable(
            TInt aOperationId,
            TInt aError );

    void PlayEmbeddedPreviewSelected(
            TInt aOperationId,
            const TDesC& aUniqueId );

    void OperationCompleted( TInt aOperationId, TInt aOperationStatus );
#endif

private:    // Data
#ifdef CAMESE_IN_DRM_UTILITY
    DRM::CDrmUiHandling* iDrmUiHandler; // owned

    TBool iRightsValid;
#else
    CDRMHelper* iDrmHelper; // owned
#ifdef __CAMESE_SUPERDISTRIBUTION_SUPPORT 
    CCameseUiHelper* iCameseUiHelper;       // own
#endif
#endif 
    CMPXCommonUiHelper* iCommonUiHelper;    // Not Owned
    };

#endif  // C_CMPXDRMUIHELPER_H

// End of File
