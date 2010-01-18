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
* Description:  Helper utility for Common UI
*
*/



// INCLUDE FILES
#include <eikenv.h>
#include <mpxmedia.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediageneraldefs.h>
#include <caf/caferr.h>
#include <mpxcommonui.rsg>
#include <StringLoader.h>
// S60 Specific

#include <mpxconstants.h>
#include "mpxdrmuihelper.h"
#include "mpxcommonuihelper.h"
#include "mpxlog.h"

#ifdef CAMESE_IN_DRM_UTILITY
#include <drmutility.h>
#include <drmuihandling.h>
#include <drmerrorhandling.h>
#include <caf/caf.h>
#include <caf/data.h>
#include <caf/caftypes.h>
#else
#include <drmhelper.h>
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXDrmUiHelper* CMPXDrmUiHelper::NewL( CMPXCommonUiHelper* aHelper )
    {
    CMPXDrmUiHelper* self = new ( ELeave ) CMPXDrmUiHelper( aHelper );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXDrmUiHelper::ConstructL()
    {
    CCoeEnv* coeEnv = CEikonEnv::Static();
#ifdef CAMESE_IN_DRM_UTILITY	
    iDrmUiHandler = DRM::CDrmUiHandling::NewL();
#else
    iDrmHelper = CDRMHelper::NewL( *coeEnv );
#endif
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXDrmUiHelper::CMPXDrmUiHelper( CMPXCommonUiHelper* aHelper )
    : iCommonUiHelper( aHelper )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXDrmUiHelper::~CMPXDrmUiHelper()
    {
#ifdef CAMESE_IN_DRM_UTILITY	
    delete iDrmUiHandler;
#else
    delete iDrmHelper;
#endif
    }

// -----------------------------------------------------------------------------
// Handles displaying the error message for given error code
// according to the media object provided
// -----------------------------------------------------------------------------
//
TInt CMPXDrmUiHelper::HandleDrmErrorL(
    TInt aError,
    const CMPXMedia* aMedia, /*=NULL*/
    RFile* aFile /*=NULL*/ )
    {
    MPX_DEBUG2("CMPXDrmUiHelper::HandleErrorL(%d) entering", aError);

    TInt ret( KErrNone );
    if ( aMedia )
        {
        if ( aMedia->IsSupported( KMPXMediaDrmType ) )
            {
#ifdef CAMESE_IN_DRM_UTILITY
            // all cases will be handled with same logic now
            ret = HandleOMAErrorL( aError, aMedia, aFile );
#else
            TMPXMediaDrmType type(
                    aMedia->ValueTObjectL<TMPXMediaDrmType>( KMPXMediaDrmType ));

            if ( EMPXDrmTypeOMA == type )
                {
                ret = HandleOMAErrorL( aError, aMedia, aFile );
                }
            else if ( EMPXDrmTypeWMA == type )
                {
                ret = HandleWMAErrorL( aError, aMedia, aFile );
                }
#endif
            }
        }

    MPX_DEBUG2("CMPXDrmUiHelper::HandleErrorL() exiting, returning %d", ret);
    return ret;
    }


// -----------------------------------------------------------------------------
// Handles OMA specific error messages
// -----------------------------------------------------------------------------
//
TInt CMPXDrmUiHelper::HandleOMAErrorL(
    TInt aError,
    const CMPXMedia* aMedia,
    RFile* aFile )
    {
    MPX_DEBUG2("CMPXDrmUiHelper::HandleOMAErrorL(%d) entering", aError);
    HBufC8* previewUrl = NULL;

    // Get filename of media
    TInt ret( KErrNone );
    TFileName filename( KNullDesC );
    if ( aMedia->IsSupported( KMPXMediaGeneralUri ) )
        {
        filename = aMedia->ValueText( KMPXMediaGeneralUri );
        }

    TBool useFile( EFalse );
    if ( aFile )
        {
        if ( aFile->SubSessionHandle() )
            {
            useFile = ETrue;
            }
        }
#ifdef CAMESE_IN_DRM_UTILITY
    switch ( aError )
        {
        case KMPXRightsAboutToExpire:
        case KErrCANoRights:
            {
            iRightsValid = EFalse;
            
            // This will ask from user if new rights are acquired
            if ( useFile )
                {
                iDrmUiHandler->CheckRightsAmountL( *aFile,
                		ContentAccess::EPlay, this );
                }
            else if ( filename.CompareF( KNullDesC ) != 0 )
                {
                ContentAccess::TVirtualPathPtr virtualPath( filename, ContentAccess::KDefaultContentObject );
                ContentAccess::CData* content( NULL );
                TRAPD( r, content = ContentAccess::CData::NewL( 
                		virtualPath, ContentAccess::EPeek, 
                		ContentAccess::EContentShareReadWrite ) );
                if ( r == KErrInUse )
                    {
                    content = ContentAccess::CData::NewL( virtualPath, ContentAccess::EPeek, 
                    		ContentAccess::EContentShareReadOnly );
                    }
                else
                    {
                    User::LeaveIfError( r );
                    }
                CleanupStack::PushL( content );
                
                iDrmUiHandler->CheckRightsAmountL( *content,
                	ContentAccess::EPlay, this );
                
                CleanupStack::PopAndDestroy( content );
                }
            if( !iRightsValid )
            	{
            	// Rights still not valid. Cancel the operation.
            	ret = KErrCancel;
            	}
            break;
            }
        default:
            {
			if ( useFile )
                {
                DRM::MDrmErrorHandling& errorHandler = iDrmUiHandler->GetErrorHandler();
                errorHandler.HandleErrorL( *aFile, ContentAccess::EPlay, aError, NULL );
                }
            else if ( filename.CompareF( KNullDesC ) != 0 )
                {
				ContentAccess::TVirtualPathPtr virtualPath( filename, ContentAccess::KDefaultContentObject );
                ContentAccess::CData* content( NULL );
                TRAPD( r, content = ContentAccess::CData::NewL( 
                		virtualPath, ContentAccess::EPeek, 
                		ContentAccess::EContentShareReadWrite ) );
                if ( r == KErrInUse )
                    {
                    content = ContentAccess::CData::NewL( virtualPath, ContentAccess::EPeek, 
                    		ContentAccess::EContentShareReadOnly );
                    }
                else
                    {
                    User::LeaveIfError( r );
                    }
                CleanupStack::PushL( content );
                
                DRM::MDrmErrorHandling& errorHandler = iDrmUiHandler->GetErrorHandler();
                errorHandler.HandleErrorL( *content, ContentAccess::EPlay, aError, NULL );

                CleanupStack::PopAndDestroy( content );
                }
            break;
            }
#else
            switch ( aError )
                {
                case KMPXRightsAboutToExpire:
                    {
                    if ( useFile )
                        {
                        ret = iDrmHelper->CheckRightsAmountL( *aFile );
                        }
                    else if ( filename.CompareF( KNullDesC ) != 0 )
                        {
                        ret = iDrmHelper->CheckRightsAmountL( filename );
                        }
                    break;
                    }
                default:
                    {
                    if ( useFile )
                        {
                        ret = iDrmHelper->HandleErrorOrPreviewL( aError, *aFile, previewUrl );
                        if( previewUrl )
                            {
                            delete previewUrl;
                            }
                        }
                    else if ( filename.CompareF( KNullDesC ) != 0 )
                        {
                        ret = iDrmHelper->HandleErrorOrPreviewL( aError, filename, previewUrl );
                        if( previewUrl )
                            {
                            delete previewUrl;
                            }
                        }
                    break;
                    }

#endif			
                }

    MPX_DEBUG2("CMPXDrmUiHelper::HandleOMAErrorL(%d) exiting, returning %d", ret);
    return ret;
    }

// -----------------------------------------------------------------------------
// Handles WMA specific error messages
// -----------------------------------------------------------------------------
//
TInt CMPXDrmUiHelper::HandleWMAErrorL(
    TInt aError,
    const CMPXMedia* aMedia,
    RFile* /*aFile*/ )
    {
    MPX_DEBUG2("CMPXDrmUiHelper::HandleWMAErrorL(%d) entering", aError);

    TInt ret( KErrNone );
    HBufC* notificationText( NULL );

    // Get filename of media
    TFileName filename( KNullDesC );
    if ( aMedia->IsSupported( KMPXMediaGeneralUri ) )
        {
        filename = aMedia->ValueText( KMPXMediaGeneralUri );
        }

    switch ( aError )
        {
        case KMPXRightsAboutToExpire:
            {
/*
            if ( aMedia->IsSupported( EMPXMediaDrmRightsType ))
                {
                TMPXMediaDrmRightsType type =
                    aMedia->ValueTObject<TMPXMediaDrmRightsType>(
                        EMPXMediaDrmRightsType );
                switch ( type )
                    {
                    case EMPXDrmRightsTypeTime:
                        {
                        TTime endTime = *aMedia->Value<TTime>(
                                            EMPXMediaDrmEndTime );
                        TTime currentTime;
                        currentTime.HomeTime();
                        TTimeIntervalDays daysRemain =
                            endTime.DaysFrom( currentTime );
                        TInt days( daysRemain.Int() );
                        if ( days < 1 )
                            {
                            notificationText = StringLoader::LoadLC(
                                R_MPX_CUI_WMDRM_RIGHTS_ABOUT_EXPIRE_NOTIFICATION );
                            }
                        else if ( days == 1 )
                            {
                            notificationText = StringLoader::LoadLC(
                                R_MPX_CUI_WMDRM_RIGHTS_ABOUT_EXPIRE_ONE_DAY_NOTIFICATION );
                            }
                        else
                            {
                            notificationText = StringLoader::LoadLC(
                                R_MUSICPLAYER_CUI_WMDRM_RIGHTS_ABOUT_EXPIRE_MULTIPLE_DAY_NOTIFICATION );
                            }
                        break;
                        }   // end case EMPXDrmRightsTypeTime
                    case EMPXDrmRightsTypeCount:
                    case EMPXDrmRightsTypeTimeCount:
                        {
                        TUint32 count(
                            aMedia->ValueTObject<TUint32>(
                                EMPXMediaDrmCount ));
                        if ( count <= KMPXWmaDrmAboutToExpireCount )
                            {
                            if ( count == 1 )
                                {
                                notificationText = StringLoader::LoadLC(
                                    R_MUSICPLAYER_CUI_WMDRM_RIGHTS_ACTIVATION_KEY_ONE_COUNT_NOTIFICATION );
                                }
                            else if ( count > 1 )
                                {
                                notificationText = StringLoader::LoadLC(
                                    R_MUSICPLAYER_CUI_WMDRM_RIGHTS_ACTIVATION_KEY_MORE_THAN_ONE_COUNT_NOTIFICATION );
                                }
                            }
                        }   // end case EMPXDrmRightsTypeCount, EMPXDrmRightsTypeTimeCount
                    default:
                        {
                        // do nothing
                        break;
                        }
                    }
                }
*/
            }   // end case KMPXRightsAboutToExpire
        case KErrCANotSupported:
        case KErrCANoPermission:
        case KErrCANoRights:
//        case KErrCANoAgent:
//        case KErrCAOutOfRange:
//        case KErrCAPendingRights:
//        case KErrCASizeNotDetermined:
//        case KErrCANewFileHandleRequired:
            {
            if ( filename.CompareF( KNullDesC ) != 0 )
                {
                TParsePtrC parse( filename );
                notificationText = StringLoader::LoadLC(
                    R_MPX_DRM_EXPIRED_PLAYBACK, parse.Name() );
                iCommonUiHelper->DisplayInfoNoteL( *notificationText );
                CleanupStack::PopAndDestroy( notificationText );
                notificationText = NULL;
                }
            break;
            }
        default:
            {
            break;
            }
        }

    MPX_DEBUG2("CMPXDrmUiHelper::HandleWMAErrorL(%d) exiting, returning %d", ret);
    return ret;
    }
#ifdef CAMESE_IN_DRM_UTILITY
// ----------------------------------------------------------------------------
// Called when valid rights are found
// ----------------------------------------------------------------------------
//
DRM::TCheckRightsAction CMPXDrmUiHelper::RightsLeft(
            TInt /*aOperationId*/,
            TBool /*aUnconstrained*/,
            TTimeIntervalSeconds /*aTime*/,
            TInt /*aCounts*/,
            TTimeIntervalSeconds /*aAccumulated*/ )
	{
	iRightsValid = ETrue;
	return DRM::EUHCheckRightsActionIgnore;
	}
 
// ----------------------------------------------------------------------------
// Called when there is no valid rights
// ----------------------------------------------------------------------------
//
DRM::TCheckRightsAction CMPXDrmUiHelper::RightsNotValid(
            TInt /*aOperationId*/,
            DRM::TCheckRightsStatus /*aRightsStatus*/,
            TInt /*aReason*/ )
	{
	iRightsValid = EFalse;

	// DrmUiHandling will ask for more rights
	return DRM::EUHCheckRightsActionDefault;
	}

// ----------------------------------------------------------------------------
// Unused observation method
// ----------------------------------------------------------------------------
//
DRM::TEmbeddedPreviewAction CMPXDrmUiHelper::EmbeddedPreviewAvailable(
            TInt /*aOperationId*/,
            const TDesC& /*aUniqueId*/,
            TInt /*aRightsStatus*/,
            TInt /*aReason*/ )
	{
	return DRM::EUEmbeddedPreviewActionIgnore;
	}

// ----------------------------------------------------------------------------
// Unused observation method
// ----------------------------------------------------------------------------
//
DRM::TPreviewRightsAction CMPXDrmUiHelper::PreviewRightsUrlAvailable(
            TInt /*aOperationId*/,
            const TDesC& /*aPreviewRightsUrl*/,
            TInt /*aRightsStatus*/,
            TInt /*aReason*/ )
	{
	return DRM::EUPreviewRightsActionIgnore;
	}

// ----------------------------------------------------------------------------
// Unused observation method
// ----------------------------------------------------------------------------
//
DRM::TSilentRightsAction CMPXDrmUiHelper::SilentRightsUrlAvailable(
            TInt /*aOperationId*/,
            const TDesC& /*aSilentRightsUrl*/,
            TInt /*aRightsStatus*/,
            TInt /*aReason*/ )
	{
	return DRM::EUSilentRightsActionDefault;
	}

// ----------------------------------------------------------------------------
// Unused observation method
// ----------------------------------------------------------------------------
//
DRM::TRightsAction CMPXDrmUiHelper::RightsUrlAvailable(
            TInt /*aOperationId*/,
            const TDesC& /*aRightsUrl*/,
            TInt /*aRightsStatus*/,
            TInt /*aReason*/,
            TInt /*aUrlType*/ )
	{
	return DRM::EURightsActionDefault;
	}

// ----------------------------------------------------------------------------
// Unused observation method
// ----------------------------------------------------------------------------
//
void CMPXDrmUiHelper::PreviewRightsAvailable(
            TInt /*aOperationId*/,
            TInt /*aError*/ )
	{
	}

// ----------------------------------------------------------------------------
// Unused observation method
// ----------------------------------------------------------------------------
//
void CMPXDrmUiHelper::SilentRightsAvailable(
            TInt /*aOperationId*/,
            TInt /*aError*/ )
	{
    iRightsValid = ETrue;
    }

// ----------------------------------------------------------------------------
// Unused observation method
// ----------------------------------------------------------------------------
//
void CMPXDrmUiHelper::RightsAvailable(
            TInt /*aOperationId*/,
            TInt /*aError*/ )
	{
    iRightsValid = ETrue;
	}

// ----------------------------------------------------------------------------
// Unused observation method
// ----------------------------------------------------------------------------
//
void CMPXDrmUiHelper::PlayEmbeddedPreviewSelected(
        TInt /*aOperationId*/,
        const TDesC& /*aUniqueId*/ )
	{
	}

// ----------------------------------------------------------------------------
// Unused observation method
// ----------------------------------------------------------------------------
//
void CMPXDrmUiHelper::OperationCompleted( 
		TInt /*aOperationId*/, TInt /*aOperationStatus*/ )
	{
	}
#endif		
//  End of File
