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
#include <coeutils.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <AknWaitDialog.h>
#include <AknProgressDialog.h>
#include <AknCommonDialogs.h>
#include <CAknFileNamePromptDialog.h>
#include <centralrepository.h>
#include <AknsUtils.h>
#include <AknIconArray.h>
#include <barsread.h>
#include <StringLoader.h>
#include <AknUtils.h>
#include <bautils.h>
#include <eikapp.h>
#include <cprofiletonehandler.h>
#include <pathinfo.h>
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif // RD_MULTIPLE_DRIVE
#include <MProfileEngine.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <data_caging_path_literals.hrh>
#include <caf/caferr.h>
#include <sysutil.h>
#include <textresolver.h>
#include <aknnotewrappers.h>

#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediaarray.h>
#include <mpxcommonui.rsg>
#include <mpxcommonui.mbg>
#include <mpx.rsg>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionpath.h> // TMPXItemId
#include <mpxcollectionutility.h>
#include <mpxcollectionplugin.hrh>
#include <mpxuser.h>
#include <AknGlobalNote.h>

// Cover UI start
//#ifdef __COVER_DISPLAY
#include <aknSDData.h>
#include <AknMediatorFacade.h>
#include <mplayersecondarydisplayapi.h>
#include <featmgr.h>
//#endif
// Cover UI end

#include <mpxconstants.h>
#include "mpxfilenamequerydlg.h"
#include "mpxcommonui.hrh"
#include "mpxtlshelper.h"
#include "mpxcommonuihelper.h"
#include "mpxdrmuihelper.h"
#include "mpxinternalcrkeys.h"
#include "mpxlog.h"


// CONSTANTS
const TInt KMPXOneSecInMicroSecs( 1000000 );
const TInt KMPXOneMinInSeconds( 60 );
const TInt KMPXOneHourInSeconds( 3600 );
const TInt KMPXOneDayInSeconds( 86400 );
const TInt KMPXOneHundredHoursInSecs( 360000 );

const TInt KMPXDurationDisplayResvLen( 10 );

const TInt KMPXDecimalPlaces ( 1 );
const TInt KMPXWidthOfChar ( 10 );

const TInt KMPXMaxBufferLen ( 160 );

const TInt KMPXMaxPlaylistPathLen ( 200 );
const TInt KMPXPlaylistinfoLen ( 300 );

const TInt KMPXStandAloneProcessIDTop32( 1 );
const TInt KMPXStandAloneProcessIDBottom32( 2 );

const TReal KMPXOneKB ( 1024 );
const TReal KMPXOneMB ( 1048576 );
const TReal KMPXOneGB ( 1073741824 );

const TText KRightToLeftMark = 0x200F;

const TInt KMPXChunkSize = 50;  // number of songs added in each chunk, IncAddL
const TInt KSQLErrGeneral = -311; // SQL General error. Don't want to include sql header here

_LIT( KMPXCommonUiRscPath, "mpxcommonui.rsc" );
_LIT( KMPXAddToPlaylistMemoryCardIcon, "\t0");
_LIT( KMPXCommonUiIconFile, "mpxcommonui.mbm" );
_LIT( KMPXAppUiRscPath, "mpx.rsc" );

// autonaming format
_LIT( KAutoNamingFormat, "%S(%S)");
#ifndef RD_MULTIPLE_DRIVE
_LIT( KDriveFormatString, ":\\" );
#endif // RD_MULTIPLE_DRIVE
_LIT( KAutoNumberFormat, "%u");
_LIT( KAutoNumberPaddedFormat, "%+02u");
_LIT( KWildCardCharacter, "*");

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXCommonUiHelper::CMPXCommonUiHelper(MMPXCollectionUtility* aCollectionUtility)
    : iCollectionUtility(aCollectionUtility)
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXCommonUiHelper::ConstructL()
    {
    CCoeEnv* coeEnv = CEikonEnv::Static();
    TParse parse;
    parse.Set( KMPXCommonUiRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    TParse appUiparse;
    appUiparse.Set( KMPXAppUiRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName appUiResourceFile( appUiparse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( appUiResourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), appUiResourceFile );
    iAppUiResourceOffset = coeEnv->AddResourceFileL( appUiResourceFile );

    iMpxDrmHelper = CMPXDrmUiHelper::NewL( this );

//#ifdef __COVER_DISPLAY
    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
        {
        iCoverDisplay = ETrue;
        }
    else
        {
        iCoverDisplay = EFalse;
        }
//#endif // __COVER_DISPLAY
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXCommonUiHelper* CMPXCommonUiHelper::NewL(MMPXCollectionUtility* aCollectionUtility)
    {
    CMPXCommonUiHelper* self = new ( ELeave ) CMPXCommonUiHelper(aCollectionUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXCommonUiHelper::~CMPXCommonUiHelper()
    {
    if ( iCollectionUiHelper )
        {
        iCollectionUiHelper->Close();
        }

    if ( iWaitDialog )
        {
        iWaitDialog->SetCallback( NULL );
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
        }

    if ( iResourceOffset )
        {
        CEikonEnv::Static()->DeleteResourceFile( iResourceOffset );
        }
    if ( iAppUiResourceOffset )
        {
        CEikonEnv::Static()->DeleteResourceFile( iAppUiResourceOffset );
        }
    delete iMpxDrmHelper;
    }

// ---------------------------------------------------------------------------
// Check if the host application is messaging or browser
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonUiHelper::IsHostMessagingBrowserL()
    {
    return ( IsHostMessagingL() || IsHostBrowserL() );
    }

// ---------------------------------------------------------------------------
// Check if the host application is messaging
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonUiHelper::IsHostMessagingL()
    {
    TBool ret( EFalse );
    TInt hostUid = MPXTlsHelper::HostUid().iUid;
    if ( hostUid == KMPXMmsViewerUid || hostUid == KMPXMmsEditorUid ||
         hostUid == KMPXMailViewerUid || hostUid == KMPXMailEditorUid ||
         hostUid == KMPXMessagingUid || hostUid == KMPXUniEditorUid ||
         hostUid == KMPXCommonMailUid )
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Check if the host application is browser
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonUiHelper::IsHostBrowserL()
    {
    TBool ret( EFalse );
    TInt hostUid = MPXTlsHelper::HostUid().iUid;
    if ( hostUid == KMPXBrowserUid || hostUid == KMPXBrowserUid2 ) // browser
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Check if the host application is Podcasting Application
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonUiHelper::IsHostPodcastingAppL()
    {
    return MPXTlsHelper::HostUid().iUid == KPodcastingAppUid;
    }

// ---------------------------------------------------------------------------
// Convert to displayable duration.
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CMPXCommonUiHelper::DisplayableDurationL(
    TInt64 aDuration,
    TMPXDuratDisplayMode aMode )
    {
    TInt resId;
    HBufC* format = NULL;
    if ( aDuration >= KMPXOneHundredHoursInSecs )
        {
        resId = R_MPX_TIME_DURAT_HHHMMSS_WITH_ZERO;
        format = StringLoader::LoadLC(
            resId, aDuration / KMPXOneHourInSeconds );
        }
    else
        {
        resId = R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO;
        if ( ( aMode == EMPXDuratHMS ) ||
            ( aDuration >= KMPXOneHourInSeconds ) )
            {
            resId = R_QTN_TIME_DURAT_LONG_WITH_ZERO;
            }
        format = StringLoader::LoadLC( resId );
        }

    // Convert total playing time to texts.
    TTime durTime( aDuration * KMPXOneSecInMicroSecs );

    HBufC* buf =
        HBufC::NewLC( format->Length() + KMPXDurationDisplayResvLen );
    TPtr bufTPtr( buf->Des() );

    durTime.FormatL( bufTPtr, *format );
    AknTextUtils::LanguageSpecificNumberConversion( bufTPtr );
    CleanupStack::Pop( buf );
    CleanupStack::PopAndDestroy( format );

    return buf;
    }

// ---------------------------------------------------------------------------
// Convert to displayable duration in text format.
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CMPXCommonUiHelper::DisplayableDurationInTextL(
    TInt64 aDuration )
    {
    HBufC* buf = NULL;
    TInt days = I64INT( aDuration / KMPXOneDayInSeconds );
    TInt hours =
        I64INT( ( aDuration % KMPXOneDayInSeconds ) /
            KMPXOneHourInSeconds );
    TInt mins =
        I64INT( ( aDuration % KMPXOneHourInSeconds ) /
            KMPXOneMinInSeconds );
    TInt secs = I64INT( aDuration % KMPXOneMinInSeconds );

    CArrayFix< TInt >* intArray =
        new ( ELeave ) CArrayFixFlat<TInt>( 2 );  // magic number
    CleanupStack::PushL( intArray );

    if ( days > 1 && ( hours == 0 || hours > 1 ) )
        {
        intArray->AppendL( days );
        intArray->AppendL( hours );
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_DAYS_HRS_TEXT, *intArray );
        }
    else if ( days > 1 && hours == 1 )
        {
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_DAYS_HR_TEXT, days );
        }
    else if ( days == 1 && ( hours == 0 || hours > 1 ) )
        {
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_DAY_HRS_TEXT, hours );
        }
    else if ( days == 1 && hours == 1 )
        {
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_DAY_HR_TEXT );
        }
    else if ( hours > 1 &&  ( mins == 0 || mins > 1 ) )
        {
        intArray->AppendL( hours );
        intArray->AppendL( mins );
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_HRS_MINS_TEXT, *intArray );
        }
    else if ( hours > 1 && mins == 1 )
        {
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_HRS_MIN_TEXT, hours );
        }
    else if ( hours == 1 && ( mins == 0 || mins > 1 ) )
        {
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_HR_MINS_TEXT, mins );
        }
    else if ( hours == 1 && mins == 1 )
        {
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_HR_MIN_TEXT );
        }
    else if ( mins > 1 && ( secs == 0 || secs > 1 ) )
        {
        intArray->AppendL( mins );
        intArray->AppendL( secs );
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_MINS_SECS_TEXT, *intArray );
        }
    else if ( mins > 1 && secs == 1 )
        {
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_MINS_SEC_TEXT, mins );
        }
    else if ( mins == 1 && ( secs == 0 || secs > 1 ) )
        {
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_MIN_SECS_TEXT, secs );
        }
    else if ( mins == 1 && secs == 1 )
        {
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_MIN_SEC_TEXT );
        }
    else if ( secs > 1 )
        {
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_SECS_TEXT, secs );
        }
    else
        {
        buf = StringLoader::LoadLC( R_MPX_CUI_TIME_SEC_TEXT );
        }

    TPtr bufPtr = buf->Des();
    AknTextUtils::LanguageSpecificNumberConversion( bufPtr );
    CleanupStack::Pop( buf );
    CleanupStack::PopAndDestroy( intArray );

    return buf;
    }

// ---------------------------------------------------------------------------
// Set current file as ringing tone.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonUiHelper::DoSetAsRingingToneL(
    const CMPXMedia& aMedia,
    TBool aSetToAllProfiles,
    TBool aShowQuery )
    {
    TInt rightsStatus( KErrNotFound );
    TBool canSetAutomated( EFalse );
    TBool okToSet( ETrue );
    TBool isProtected( EFalse );

    if ( aMedia.IsSupported ( KMPXMediaDrmProtected ) )
        {
        isProtected = aMedia.ValueTObjectL<TBool>( KMPXMediaDrmProtected );
        }
    if ( isProtected )
        {
        if ( aMedia.IsSupported ( KMPXMediaDrmRightsStatus ) )
            {
            rightsStatus = aMedia.ValueTObjectL<TInt>( KMPXMediaDrmRightsStatus );
            }
        if ( aMedia.IsSupported ( KMPXMediaDrmCanSetAutomated ) )
            {
            canSetAutomated = aMedia.ValueTObjectL<TBool>( KMPXMediaDrmCanSetAutomated );
            }
        }

    if ( ( !isProtected ) || ( rightsStatus == EMPXDrmRightsFull )
        || ( canSetAutomated ) )
        {
        if ( aShowQuery && aSetToAllProfiles )
            {
            // Show query only if set ringtone to all profiles and needed
            CAknQueryDialog* query =
                CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );
            okToSet = query->ExecuteLD(
                R_MPX_CHANGE_RT_FOR_ALL_CONFIRMATION_QUERY );
            }

        if ( okToSet )
            {
            TInt error( KErrNone );
            TInt leaveError( KErrNone );
            CProfileToneHandler* toneHandler = CProfileToneHandler::NewLC();

            if ( aSetToAllProfiles )
                {
                MPX_TRAP( leaveError, error = toneHandler->SetToneForAllProfilesL(
                        EProfileRingingToneSetting,
                        aMedia.ValueText( KMPXMediaGeneralUri ) ) );
                }
            else
                {
                MPX_TRAP( leaveError, error = toneHandler->SetActiveProfileRingingToneL(
                        aMedia.ValueText( KMPXMediaGeneralUri ) ) );
                }
            if ( leaveError != KErrNone )
                {
                error = leaveError;
                }

            CleanupStack::PopAndDestroy( toneHandler );

            if ( KErrNone == error && !aSetToAllProfiles )
                {
                HBufC* text = StringLoader::LoadLC(
                    R_MPX_RINGTONE_SET_TEXT, aMedia.ValueText(
                        KMPXMediaGeneralTitle ) );
                DisplayConfirmNoteL( *text );
                CleanupStack::PopAndDestroy( text );
                }
            else if ( KErrPermissionDenied == error )
                {
                DisplayInfoNoteL( R_MPX_RINGTONE_UNPROTECTED_FILE_TEXT );
                }
            else if ( KErrArgument == error ||
                      KErrAccessDenied == error )
                {
                DisplayInfoNoteL( R_MPX_RINGTONE_NOT_ALLOWED_TEXT );
                }
            else if ( KErrNotFound == error )
                {
                DisplayInfoNoteL( R_MPX_FILE_NOT_FOUND_TEXT );
                }
            else
                {
                // do nothing
                }
            }
        }
    else
        {
        DisplayInfoNoteL( R_MPX_DRM_PREV_RIGHTS_SET );
        }
    }

// -----------------------------------------------------------------------------
// Check if current profile mode is offline mode.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonUiHelper::IsProfileOfflineModeL()
    {
    TBool isOfflineMode( EFalse );
    MProfileEngine* profileEngine;
    profileEngine = CreateProfileEngineL();

    if ( profileEngine->ActiveProfileId() == EProfileOffLineId )
        {
        isOfflineMode = ETrue;
        }
    profileEngine->Release();

    return isOfflineMode;
    }

// -----------------------------------------------------------------------------
// Returns a path of the default drive used for saving playlists, ringtones,
// downloads and attachments.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CMPXCommonUiHelper::DefaultDriveLC()
    {
#ifdef RD_MULTIPLE_DRIVE
    // Use an internal mass storage drive as the default drive. If no internal
    // drive (other than the phone memory) exists, use an external drive.
    // Otherwise use the phone memory.
    RFs& fs = CEikonEnv::Static()->FsSession();
    TDriveList driveList;
    TInt driveCount(0);
    TInt defaultInternalDrive(0);
    TInt defaultExternalDrive(0);
    TBool foundInternalDrive(EFalse);
    TBool foundExternalDrive(EFalse);
    User::LeaveIfError( DriveInfo::GetUserVisibleDrives(
        fs, driveList, driveCount ) );

    for( TInt driveNum = EDriveA;
        ( driveNum <= EDriveZ ) && !foundInternalDrive;
        driveNum++ )
        {
        if ( driveList[driveNum] && ( driveNum != EDriveC ) ) // ignore C: drive
            {
            // Get the drive status
            TUint driveStatus(0);
            DriveInfo::GetDriveStatus( fs, driveNum, driveStatus );

            if ( driveStatus & DriveInfo::EDriveInternal )
                {
                defaultInternalDrive = driveNum;
                foundInternalDrive = ETrue;
                } // internal mass store drive
            else if ( !foundExternalDrive &&
                (driveStatus &
                (DriveInfo::EDriveRemovable | DriveInfo::EDriveRemote)) )
                {
                TVolumeInfo volumeInfo;
                TInt errCode = fs.Volume( volumeInfo, driveNum );
                if( errCode == KErrNone )
                    {
                    //driveNum is really ready for use      
                    defaultExternalDrive = driveNum;
                    foundExternalDrive = ETrue;    
                    }
                } // external drive
            } // if (available drive)
        } // for (all available drives)

    // Use the internal drive, external drive or phone memory as the default
    // path
    HBufC* defaultRootPath = HBufC::NewLC( 10 );
    TPtr pathPtr( defaultRootPath->Des() );
    if ( foundInternalDrive )
        {
        User::LeaveIfError(
            PathInfo::GetRootPath( pathPtr, defaultInternalDrive ) );
        }
    else if ( foundExternalDrive )
        {
        User::LeaveIfError(
            PathInfo::GetRootPath( pathPtr, defaultExternalDrive ) );
        }
    else // use phone memory path
        {
        pathPtr.Copy( PathInfo::PhoneMemoryRootPath() );
        }

    MPX_DEBUG2("CMPXCommonUiHelper::DefaultDriveLC - path = %S", &pathPtr);
#else
    HBufC* defaultRootPath = NULL;
    // Default drive used for saving playlists, etc
    // is as following:
    // 1. if E: drive exists ( MMC card inserted )
    //    playlist will be saved on the E: drive unless
    //    ( mmc is locked, readonly, or below critical disk space)
    // 2. E drive is not present ( MMC card ejected ), save the
    //    playlist on the phone memory instead.

    TChar drive;
    if ( !IsMemoryCardWritable( drive ) )
        {
        // load phone memory root path
        defaultRootPath = StringLoader::LoadLC(
            R_MPX_CUI_PHONE_MEMORY_ROOT_PATH );
        }
    else
        {
        defaultRootPath = HBufC::NewLC( 10 );
        TPtr ptr( defaultRootPath->Des() );
        ptr.Append( drive );
        ptr.Append( KDriveFormatString );
        }
#endif // RD_MULTIPLE_DRIVE
    return defaultRootPath;
    }

// -----------------------------------------------------------------------------
// Returns a path of the next available drive used for saving playlists, ringtones,
// downloads and attachments. The order of priority is internal mass store,
// MMC, then phone memory.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CMPXCommonUiHelper::AvailableDriveLC( TInt aSize )
    {
#ifdef RD_MULTIPLE_DRIVE
    // Use an available mass storage drive (other than the phone memory). If
    // none exists, use an external drive. Otherwise use the phone memory.
    RFs& fs = CEikonEnv::Static()->FsSession();
    TDriveList driveList;
    TInt driveCount(0);
    TInt availableInternalDrive(0);
    TInt availableExternalDrive(0);
    TBool foundInternalDrive(EFalse);
    TBool foundExternalDrive(EFalse);
    User::LeaveIfError( DriveInfo::GetUserVisibleDrives(
        fs, driveList, driveCount ) );

    for( TInt driveNum = EDriveA;
        ( driveNum <= EDriveZ ) && !foundInternalDrive;
        driveNum++ )
        {
        if ( driveList[driveNum] && ( driveNum != EDriveC ) ) // ignore C: drive
            {
            // Get the drive status
            TUint driveStatus(0);
            DriveInfo::GetDriveStatus( fs, driveNum, driveStatus );

            if ( driveStatus & DriveInfo::EDriveInternal )
                {
                // Ensure there is enough disk space for the file
                TBool internalDriveBelowCritical = EFalse;
                MPX_TRAPD( err, internalDriveBelowCritical =
                    SysUtil::DiskSpaceBelowCriticalLevelL(
                    &fs, aSize, driveNum ) );
                if ( internalDriveBelowCritical )
                    {
                    MPX_DEBUG2("CMPXCommonUiHelper::AvailableDriveLC - Internal drive below critical driveNum=%d", driveNum);
                    }
                else if ( err == KErrNone )
                    {
                    availableInternalDrive = driveNum;
                    foundInternalDrive = ETrue;
                    }
                } // internal mass store drive
            else if ( !foundExternalDrive &&
                (driveStatus &
                (DriveInfo::EDriveRemovable | DriveInfo::EDriveRemote)) )
                {
                TBool externalDriveBelowCritical = EFalse;
                MPX_TRAPD( err, externalDriveBelowCritical =
                    SysUtil::DiskSpaceBelowCriticalLevelL(
                    &fs, aSize, driveNum ) );
                if ( externalDriveBelowCritical )
                    {
                    MPX_DEBUG2("CMPXCommonUiHelper::AvailableDriveLC - External drive below critical driveNum=%d", driveNum);
                    }
                else if ( err == KErrNone )
                    {
                    availableExternalDrive = driveNum;
                    foundExternalDrive = ETrue;
                    }
                } // external drive
            } // if (available drive)
        } // for (all available drives)

    // Use the internal drive, external drive or phone memory as the default
    // path
    HBufC* defaultRootPath = HBufC::NewLC( 10 );
    TPtr pathPtr( defaultRootPath->Des() );
    if ( foundInternalDrive )
        {
        User::LeaveIfError(
            PathInfo::GetRootPath( pathPtr, availableInternalDrive ) );
        }
    else if ( foundExternalDrive )
        {
        User::LeaveIfError(
            PathInfo::GetRootPath( pathPtr, availableExternalDrive ) );
        }
    else // use phone memory path
        {
        pathPtr.Copy( PathInfo::PhoneMemoryRootPath() );
        }

    MPX_DEBUG2("CMPXCommonUiHelper::AvailableDriveLC - path = %S", &pathPtr);
    return defaultRootPath;
#else
    (void) aSize;
    return NULL;
#endif // RD_MULTIPLE_DRIVE
    }

// -----------------------------------------------------------------------------
// Display information note
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonUiHelper::DisplayInfoNoteL( TInt aResourceId )
    {
    HBufC* text = StringLoader::LoadLC( aResourceId );
    DisplayInfoNoteL( *text );
    CleanupStack::PopAndDestroy( text );
    }

// -----------------------------------------------------------------------------
// Display information note
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonUiHelper::DisplayInfoNoteL( const TDesC& aText )
    {
    CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
    dlg->ExecuteLD( aText );
    }

// -----------------------------------------------------------------------------
// Display confirmation note
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonUiHelper::DisplayConfirmNoteL( TInt aResourceId )
    {
    HBufC* text = StringLoader::LoadLC( aResourceId );

// Cover UI start
//#ifdef __COVER_DISPLAY
    DisplayConfirmNoteL( *text, aResourceId );
//#else
//    DisplayConfirmNoteL( *text );
//#endif // __COVER_DISPLAY
// CoverUI end

    CleanupStack::PopAndDestroy( text );
    }

// -----------------------------------------------------------------------------
// Display confirmation note
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonUiHelper::DisplayConfirmNoteL( const TDesC& aText )
    {
    CAknConfirmationNote* dlg = new ( ELeave ) CAknConfirmationNote( ETrue );
    dlg->ExecuteLD( aText );
    }

// Cover UI start
//#ifdef __COVER_DISPLAY
// -----------------------------------------------------------------------------
// Display confirmation note
// -----------------------------------------------------------------------------
//
void CMPXCommonUiHelper::DisplayConfirmNoteL(
    const TDesC& aText,
    TInt aResourceId )
    {
    CAknConfirmationNote* dlg = new ( ELeave ) CAknConfirmationNote( ETrue );

    CleanupStack::PushL( dlg );
    if ( aResourceId == R_MPX_SAVED_TO_COLLECTION_NOTE )
        {
        dlg->PublishDialogL(
            EMPlayerNoteSavedtoCollection,
            KMPlayerNoteCategory);
        }
    else if ( aResourceId == R_MPX_SAVED_NOTE )
        {
        dlg->PublishDialogL(
            EMPlayerNoteSavedtoGallery,
            KMPlayerNoteCategory);
        }
    CleanupStack::Pop( dlg );

    dlg->ExecuteLD( aText );
    }
//#endif // __COVER_DISPLAY
// Cover UI end

// -----------------------------------------------------------------------------
// Convert a value from bytes to kB/MB/GB.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CMPXCommonUiHelper::UnitConversionL( TInt64 aByte, TBool aFree )
    {
    HBufC* res = NULL;

    if ( aByte >= 0 && aByte < KMPXOneKB ) // unit is B
        {
        if ( aFree )
            {
            res = StringLoader::LoadLC( R_MPX_CUI_MEMORY_B_FREE, aByte );
            }
        else
            {
            res = StringLoader::LoadLC( R_MPX_CUI_MEMORY_B, aByte );
            }
        }
    else // unit is larger than a byte and must be displayed with decimal
        {
        TReal memoryUnitSize;
        TInt memoryUnitResId;

        if ( aByte >= KMPXOneKB && aByte < KMPXOneMB ) // unis is kB
            {
            memoryUnitSize = KMPXOneKB;
            if ( aFree)
                {
                memoryUnitResId = R_MPX_CUI_MEMORY_KB1_FREE;
                }
            else
                {
                memoryUnitResId = R_MPX_CUI_MEMORY_KB;
                }
            }
        else if ( aByte >= KMPXOneMB && aByte < KMPXOneGB ) // unit is MB
            {
            memoryUnitSize = KMPXOneMB;
            if ( aFree )
                {
                memoryUnitResId = R_MPX_CUI_MEMORY_MB1_FREE;
                }
            else
                {
                memoryUnitResId = R_MPX_CUI_MEMORY_MB;
                }
            }
        else // unit is GB
            {
            memoryUnitSize = KMPXOneGB;
            if ( aFree )
                {
                memoryUnitResId = R_MPX_CUI_MEMORY_GB1_FREE;
                }
            else
                {
                memoryUnitResId = R_MPX_CUI_MEMORY_GB;
                }
            }

        TBuf<KMPXMaxBufferLen> sizeBuf;
        TRealFormat format = TRealFormat( KMPXWidthOfChar, KMPXDecimalPlaces );
        TReal tmpValue = ( I64REAL(aByte) / memoryUnitSize );
        sizeBuf.AppendNum( tmpValue, format );
        if ( User::Language() == ELangArabic )
            {
            sizeBuf.Append( KRightToLeftMark );
            }
        res = StringLoader::LoadLC( memoryUnitResId, sizeBuf );
        }
    CleanupStack::Pop( res );
    return res;
    }

// -----------------------------------------------------------------------------
// Finds all user playlists in the given collection
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXMedia* CMPXCommonUiHelper::FindPlaylistsL()
    {
    MPX_FUNC( "CMPXCommonUiHelper::FindPlaylistsL" );

    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.Append( KMPXMediaGeneralTitle );
    attrs.Append( KMPXMediaGeneralId );
    attrs.Append( KMPXMediaGeneralFlags );

    CMPXMedia* ret = FindAllL( EMPXPlaylist, attrs.Array() );
    CleanupStack::PopAndDestroy( &attrs );

    return ret;
    }

// -----------------------------------------------------------------------------
// Add to saved playlist
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonUiHelper::AddToSavedPlaylistL(
    const CMPXMedia& aPlaylists, const CMPXMedia& aTracks,
    MMPXCHelperObserver* aObserver, MProgressDialogCallback* aCallback )
    {
    MPX_FUNC( "CMPXCommonUiHelper::AddToSavedPlaylistL" );

    TBool ret( EFalse );

    const CMPXMediaArray* mediaArray =
        aPlaylists.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    User::LeaveIfNull( const_cast<CMPXMediaArray*>( mediaArray ));

    TInt count( 0 );
    if ( mediaArray )
        {
        count = mediaArray->Count();
        }
    else
        {
        User::Leave( KErrArgument );
        }

    if ( count < 1)
        {
        User::Leave( KErrArgument );
        }
    else
        {
        TInt index = 0;
        CAknListQueryDialog* queryDlg =
            new ( ELeave ) CAknListQueryDialog( &index );
        queryDlg->PrepareLC( R_MPX_CUI_PLAYLIST_QUERY_LIST );

//#ifdef __COVER_DISPLAY
        queryDlg->PublishDialogL(
            EMPlayerQuerySelectPlaylist, KMPlayerNoteCategory );
//#endif //__COVER_DISPLAY

        // Get the text array from the listbox model
        CTextListBoxModel* lbxModel =
            static_cast<CTextListBoxModel*>( queryDlg->ListBox()->Model() );
        CDesCArray* descArray =
            static_cast<CDesCArray*>( lbxModel->ItemTextArray() );
        descArray->Reset();

        // Set up icon array
        CAknIconArray* icons = new ( ELeave ) CAknIconArray( 1 ); // magic: icon count
        CleanupStack::PushL( icons );
        TParse mbmFileName;
        mbmFileName.Set( KMPXCommonUiIconFile,
            &KDC_APP_RESOURCE_DIR, NULL );
        TFileName iconFile( mbmFileName.FullName() );
        User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        AknsUtils::CreateColorIconLC(
            AknsUtils::SkinInstance(), // aInstance
            KAknsIIDQgnIndiMmcAdd,     // aID
            KAknsIIDQsnIconColors,      // aColorID
            EAknsCIQsnIconColorsCG26,   // aColorIndex
            bitmap,
            mask,
            iconFile,
            EMbmMpxcommonuiQgn_indi_mmc_add,
            EMbmMpxcommonuiQgn_indi_mmc_add_mask,
            KRgbBlack );                // aDefaultColor
        CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
        icon->SetBitmapsOwnedExternally( EFalse );

        // icon now owns the bitmaps, no need to keep on cleanup stack.
        CleanupStack::Pop(2); // mask, bitmap
        bitmap = NULL;
        mask = NULL;
        CleanupStack::PushL( icon );
        icons->AppendL( icon );
        queryDlg->SetIconArrayL(icons);
        CleanupStack::Pop( icon );
        CleanupStack::Pop( icons );

        TInt mmc = MMCDriveNumber();
        // Prepare for query dialog
        for ( TInt i = 0; i < count; i++ )
            {
            CMPXMedia* media( mediaArray->AtL( i ));
            TUint flags( 0 );
            TRAP_IGNORE(flags = media->ValueTObjectL<TUint>( KMPXMediaGeneralFlags ))
            TInt driveNum = ( ( flags ) & ( KMPXMediaGeneralFlagsDriveInfo ) );
            HBufC* line = NULL;
            HBufC* title = media->ValueText( KMPXMediaGeneralTitle ).AllocLC();

            if( driveNum == mmc )
                {
                line = HBufC::NewLC(
                    KMPXAddToPlaylistMemoryCardIcon().Length() +
                    title->Length() );
                TPtr linePtr = line->Des();
                linePtr += *title;
                linePtr.Append( KMPXAddToPlaylistMemoryCardIcon );
                }
            else
                {
                line = title->AllocLC();
                }
            descArray->AppendL( *line );
            CleanupStack::PopAndDestroy( line );
            CleanupStack::PopAndDestroy( title );
            }

        if ( queryDlg->RunLD() )
            {
            if ( index >= 0 )
                {
                CMPXMedia* media( mediaArray->AtL( index ));
                TMPXItemId playlistId =
                    media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
                const CMPXMediaArray* tracksArray =
                    aTracks.Value<CMPXMediaArray>( KMPXMediaArrayContents );
                User::LeaveIfNull( const_cast<CMPXMediaArray*>( tracksArray ));
                if ( tracksArray->Count() < 1 )
                    {
                    User::Leave( KErrArgument );
                    }
                else
                    {
                    if ( tracksArray->Count() == 1 )
                        {
                        HBufC* text = StringLoader::LoadLC(
                            R_MPX_QTN_NMP_NOTE_ADDING_ONE_SONG );
                        ShowWaitNoteL( *text,
                            R_AVKON_SOFTKEYS_EMPTY, ETrue, aCallback );
                        CleanupStack::PopAndDestroy( text );
                        }
                    else
                        {
                        HBufC* text = StringLoader::LoadLC(
                            R_MPX_QTN_MUS_NOTE_ADDING_TRACKS );
                        ShowWaitNoteL( *text,
                            R_MPX_WAITNOTE_SOFTKEYS_EMPTY_STOP, ETrue, aCallback );
                        CleanupStack::PopAndDestroy( text );
                        }
                    CMPXMedia* media = CMPXMedia::NewL();
                    CleanupStack::PushL( media );
                    media->SetTObjectValueL<TMPXItemId>(
                        KMPXMediaGeneralId, playlistId );
                    media->SetTObjectValueL<TMPXGeneralType>(
                        KMPXMediaGeneralType, EMPXItem );
                    media->SetTObjectValueL<TMPXGeneralCategory>(
                        KMPXMediaGeneralCategory, EMPXPlaylist );
                    media->SetCObjectValueL(
                        KMPXMediaArrayContents,
                        const_cast<CMPXMediaArray*>(tracksArray ));
                    media->SetTObjectValueL(
                        KMPXMediaArrayCount, tracksArray->Count() );

                    RArray<TUid> ary;
                    CleanupClosePushL( ary );
                    ary.AppendL( TUid::Uid(EMPXCollectionPluginMusic) );
                    TUid musicCollection = iCollectionUtility->CollectionIDL( ary.Array() );
                    CleanupStack::PopAndDestroy( &ary );

                    media->SetTObjectValueL<TUid>( KMPXMediaGeneralCollectionId, musicCollection );

                    if ( !iCollectionUiHelper )
                        {
                        iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();
                        }
                    iCollectionUiHelper->IncAddL( *media, aObserver, KMPXChunkSize );
                    ret = ETrue;
                    CleanupStack::PopAndDestroy( media );
                    }
                }
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// Create playlist
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonUiHelper::CreatePlaylistL(
    const CMPXMedia& aTracks, MMPXCHelperObserver* aObserver,
    MProgressDialogCallback* aCallback )
    {
    MPX_FUNC( "CMPXCommonUiHelper::CreatePlaylistL" );

    RFs& fs = CEikonEnv::Static()->FsSession();
    HBufC* playlistName = HBufC::NewLC( KMaxFileName );
    TPtr playlistNamePtr = playlistName->Des();
    const TInt playListSize = 8;

#ifdef RD_MULTIPLE_DRIVE
    // Create playlist on the next available drive (internal mass store,
    // MMC or phone memory)
    HBufC* defaultRootPath = AvailableDriveLC( playListSize );
    playlistNamePtr = *defaultRootPath;
    CleanupStack::PopAndDestroy( defaultRootPath );
#else
    HBufC* defaultRootPath = DefaultDriveLC();
    playlistNamePtr = *defaultRootPath;
    CleanupStack::PopAndDestroy( defaultRootPath );

    if ( playlistNamePtr.CompareF( PathInfo::MemoryCardRootPath() ) == 0 )
        {
        // only if memory card is supported, check if there is enough space
        // on the card.
        TBool isBelowCritical = EFalse;
        MPX_TRAPD( err, isBelowCritical = SysUtil::MMCSpaceBelowCriticalLevelL(
            &fs, playListSize ) );
        if ( err || isBelowCritical )
            {
            MPX_DEBUG1( "CMPXCommonUiHelper::CreatePlaylistL Error in MMC, default path changed to phone memory" );
            playlistNamePtr = PathInfo::PhoneMemoryRootPath();
            }
        }
#endif // RD_MULTIPLE_DRIVE

    HBufC* defaultPath =
        StringLoader::LoadLC( R_MPX_CUI_TARGETFOLDER_PLAYLIST );
    playlistNamePtr += *defaultPath;
    CleanupStack::PopAndDestroy( defaultPath );

    TBool ret( PlaylistCreationQueriesL( playlistNamePtr ) );
    if ( ret )
        {
        MPX_DEBUG2( "CMPXCommonUiHelper::CreatePlaylistL Saving playlist %S", &playlistNamePtr );
        const CMPXMediaArray* tracksArray =
            aTracks.Value<CMPXMediaArray>( KMPXMediaArrayContents );
        User::LeaveIfNull( const_cast<CMPXMediaArray*>( tracksArray ));
        if ( tracksArray->Count() == 1 )
            {
            HBufC* text = StringLoader::LoadLC(
                R_MPX_QTN_NMP_NOTE_ADDING_ONE_SONG );
            ShowWaitNoteL( *text,
                R_AVKON_SOFTKEYS_EMPTY, ETrue, aCallback );
            CleanupStack::PopAndDestroy( text );
            }
        else if ( tracksArray->Count() > 0 )
            {
            // only show wait note if we're adding more than 1 song
            HBufC* text = StringLoader::LoadLC(
                R_MPX_QTN_MUS_NOTE_ADDING_TRACKS );
            ShowWaitNoteL( *text,
                R_MPX_WAITNOTE_SOFTKEYS_EMPTY_STOP, ETrue, aCallback );
            CleanupStack::PopAndDestroy( text );
            }

        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL( media );
        media->SetTObjectValueL<TMPXGeneralType>(
            KMPXMediaGeneralType, EMPXItem );
        media->SetTObjectValueL<TMPXGeneralCategory>(
            KMPXMediaGeneralCategory, EMPXPlaylist );
        TParsePtrC fn( playlistNamePtr );
        // no extension in virtual playlist, all "extension" are inputed
        // by users as a valid name
        media->SetTextValueL(
            KMPXMediaGeneralTitle, fn.NameAndExt() );

        media->SetCObjectValueL(
            KMPXMediaArrayContents,
            const_cast<CMPXMediaArray*>(tracksArray ) );
        media->SetTObjectValueL(
            KMPXMediaArrayCount, tracksArray->Count() );
        media->SetTextValueL(
            KMPXMediaGeneralUri, fn.DriveAndPath() ); // exculde the file name
        // new playlist
        if ( !iCollectionUiHelper )
            {
            iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();
            }
        iCollectionUiHelper->IncAddL( *media, aObserver, KMPXChunkSize );
        CleanupStack::PopAndDestroy( media );
        }
    CleanupStack::PopAndDestroy( playlistName );
    return ret;
    }

// -----------------------------------------------------------------------------
// Launch rename dialog
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXCommonUiHelper::LaunchRenameDialogL(
    const TDesC& aOldName,
	TDes& aNewName,
	const TDesC& aPath )
    {
    MPX_FUNC( "CMPXCommonUiHelper::LaunchRenameDialogL" );
    CMPXFileNameQueryDlg* renameDlg = CMPXFileNameQueryDlg::NewL(
        aOldName, aNewName, aPath, EMPXPlaylist, this );
    TInt ret = renameDlg->ExecuteLD( R_MPX_FILE_NAME_QUERY );

    if ( ret )
        {
        if ( aNewName.CompareF( aOldName ) == 0 )
            {
            ret = EFalse;
            }
        else
            {
            if ( !iCollectionUiHelper )
                {
                iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();
                }
            if ( iCollectionUiHelper->TitleExistsL( EMPXPlaylist, aNewName, EMPXOtherType ) )
                {
                HBufC* info = StringLoader::LoadLC( R_MPX_PLAYLIST_NAME_EXIST, aNewName );
                CAknQueryDialog* query = new(ELeave)CAknQueryDialog( *info );
                if( query->ExecuteLD( R_RENAME_QUERY ) )
                    {
                    if ( !LaunchRenameDialogL( aOldName, aNewName, aPath ) )
                        {
                        ret = EFalse;
                        }
                    }
                else
                    {
                    ret = EFalse;
                    }
                CleanupStack::PopAndDestroy( info );//query ,info
                }
            }
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// Shows wait note
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonUiHelper::ShowWaitNoteL( TDesC& aText, TInt aCBAId,
    TBool aVisibilityDelayOff, MProgressDialogCallback* aCallback, TInt aResId )
    {
    MPX_FUNC( "CMPXCommonUiHelper::ShowWaitNoteL" );
    if( iWaitDialog )
        {
        // if wait note is already running
        // cancel it and recreate will have timing issues
        if ( aCallback )
            {
            iWaitDialog->SetCallback( aCallback );
            }
        iWaitDialog->SetTextL( aText );
        MEikButtonGroup* buttonGroup =
            iWaitDialog->ButtonGroupContainer().ButtonGroup();
        if ( aCBAId )
            {
            buttonGroup->SetCommandSetL( aCBAId );
            }
        buttonGroup->AsControl()->DrawDeferred();
        iWaitDialog->DrawDeferred();
        }
    else
        {
        iWaitDialog = new ( ELeave ) CAknWaitDialog(
            ( CEikDialog** )&iWaitDialog, aVisibilityDelayOff );
        if ( aCallback )
            {
            iWaitDialog->SetCallback( aCallback );
            }
        iWaitDialog->PrepareLC( aResId );
        iWaitDialog->SetTextL( aText );
        MEikButtonGroup* buttonGroup =
            iWaitDialog->ButtonGroupContainer().ButtonGroup();
        if ( aCBAId )
            {
            buttonGroup->SetCommandSetL( aCBAId );
            }
        buttonGroup->AsControl()->DrawDeferred();
        iWaitDialog->RunLD();
        }
    }

// -----------------------------------------------------------------------------
// Dismiss wait note
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonUiHelper::DismissWaitNoteL()
    {
    MPX_FUNC( "CMPXCommonUiHelper::DismissWaitNoteL" );
    if( iWaitDialog )
        {
        iWaitDialog->ProcessFinishedL();
        }
    }

// -----------------------------------------------------------------------------
// Handles displaying the error message for given error code
// according to the media object provided
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXCommonUiHelper::HandleErrorL(
    TInt aError,
    const CMPXMedia* aMedia, /*=NULL*/
    RFile* aFile /*=NULL*/ )
    {
    MPX_DEBUG2("CMPXCommonUiHelper::HandleErrorL(%d) entering", aError);

    TBool usingGlobalNote = EFalse; // To use CAknGlobalNote if necessary
    TInt ret( KErrNone );
    CTextResolver* textResolver( NULL );
    HBufC* errorText( NULL );
    switch ( aError )
        {
        case KMPXRightsAboutToExpire:
        case KErrCANotSupported:
        case KErrCANoPermission:
        case KErrCANoRights:
        case KErrCANoAgent:
        case KErrCAOutOfRange:
        case KErrCAPendingRights:
        case KErrCASizeNotDetermined:
        case KErrCANewFileHandleRequired:
            {
            TRAPD(err, ret = iMpxDrmHelper->HandleDrmErrorL( aError, aMedia, aFile ));
            if (err)
                {
                ret = err;
                if ( ret == KErrArgument )
                    // If error is KErrArgument, map it to R_MPX_INVALID_CLIP
                    errorText = StringLoader::LoadLC( R_MPX_INVALID_CLIP );
                else
                    {
                    // TextResolver instance for error resolving.
                    textResolver = CTextResolver::NewLC();
                    // Resolve the error text
                    const TDesC& text =
                        textResolver->ResolveErrorString( ret );
                    errorText = text.AllocLC();
                    }
                }
            break;
            }
        case KMPXAllTracksInvalid:
            {
            errorText =
                StringLoader::LoadLC( R_MPX_INVALID_GROUP_TEXT );
            break;
            }
        case KErrNotSupported:
        case KErrCorrupt:
            {
            errorText = StringLoader::LoadLC( R_MPX_INVALID_CLIP );
            break;
            }
        case KErrAccessDenied:
            {
            if ( aMedia )
                {
                if ( aMedia->IsSupported( KMPXMediaDrmType ) )
                    {
                    TMPXMediaDrmType type =
                        aMedia->ValueTObjectL<TMPXMediaDrmType>( KMPXMediaDrmType );
                    if ( EMPXDrmTypeOMA == type )
                        {
                        errorText = StringLoader::LoadLC( R_MPX_NO_DRM_OVER_BT );
                        break;
                        }
                    }
                }
            // fall through on purpose to use default handling
            // if error was not handled above
            }
        case KMPXErrorVideoCall:
            {
            errorText = StringLoader::LoadLC( R_MPX_VIDEO_CALL_ONGOING );
            usingGlobalNote = ETrue;
            break;
            }
        case KMPXErrorExpiredRights:
            {
            // if cannot retrieve value from aMedia, use invalid_group error	
			if( !aMedia )
			    {
			    errorText = StringLoader::LoadLC( R_MPX_INVALID_GROUP_TEXT );
			    }	
			else
				{
				if ( aMedia->IsSupported( KMPXMediaGeneralTitle ) )
				    {
            		const TDesC& trackTitle( aMedia->ValueText( 
            			                           KMPXMediaGeneralTitle ) );
            	    errorText = StringLoader::LoadLC( 
            		                            R_MPX_DRM_EXPIRED_PLAYBACK, 
            			  	                    trackTitle );
            	    }
                }
            break;
            }
        case KErrLocked:
            errorText = StringLoader::LoadLC( R_MPX_CANNOT_PROCEED_WITH_OPERATION );
            break;
        case KErrDisMounted:
        	break;
        case KErrDiskFull:
        case KSQLErrGeneral:
            errorText = StringLoader::LoadLC( R_MPX_CUI_MEMLO_NOT_ENOUGH_MEMORY );
            break;
        default:
            {
            // TextResolver instance for error resolving.
            textResolver = CTextResolver::NewLC();
            // Resolve the error text
            const TDesC& text =
                textResolver->ResolveErrorString( aError );
            errorText = text.AllocLC();
            break;
            }
        }

    if ( errorText )
        {
        if ( !usingGlobalNote )
            {
            CAknErrorNote* dlg = new ( ELeave ) CAknErrorNote( ETrue );
            dlg->ExecuteLD( *errorText );
            }
        else
            {
            // CAknErrorNote will not display when mpx is not currently foreground, 
            // so if necessary, use CAknGlobalNote to replace it.
            CAknGlobalNote* globalInfoNote = CAknGlobalNote::NewLC();
            globalInfoNote->ShowNoteL(
                    EAknGlobalInformationNote,
                    errorText->Des() );
            CleanupStack::PopAndDestroy( globalInfoNote );
            }
        CleanupStack::PopAndDestroy( errorText );
        }
    if ( textResolver )
        {
        CleanupStack::PopAndDestroy( textResolver );
        }

    MPX_DEBUG2("CMPXCommonUiHelper::HandleErrorL() exiting, returning %d", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// Sets middle softkey label.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonUiHelper::SetMiddleSoftKeyLabelL(
    CEikButtonGroupContainer& aCba,
    TInt aResourceId,
    TInt aCommandId )
    {
#ifdef __ENABLE_MSK
    MPX_FUNC( "CMPXCommonUiHelper::SetMiddleSoftKeyLabelL" );
    aCba.RemoveCommandFromStack(
        CEikButtonGroupContainer::EMiddleSoftkeyPosition,
        aCba.ButtonGroup()->CommandId(
            CEikButtonGroupContainer::EMiddleSoftkeyPosition ) );
    HBufC* middleSKText = StringLoader::LoadLC( aResourceId );
    TPtr mskPtr = middleSKText->Des();
    aCba.AddCommandToStackL(
        CEikButtonGroupContainer::EMiddleSoftkeyPosition,
        aCommandId,
        mskPtr );
    CleanupStack::PopAndDestroy( middleSKText );
#else
    (void)aCba;
    (void)aResourceId;
    (void)aCommandId;
    User::Leave( KErrNotSupported );
#endif // __ENABLE_MSK
    }

// ---------------------------------------------------------------------------
// Removes middle softkey label.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonUiHelper::RemoveMiddleSoftKeyLabel(
    CEikButtonGroupContainer& aCba )
    {
#ifdef __ENABLE_MSK
    MPX_FUNC( "CMPXCommonUiHelper::RemoveMiddleSoftKeyLabel" );
    aCba.RemoveCommandFromStack(
        CEikButtonGroupContainer::EMiddleSoftkeyPosition,
        aCba.ButtonGroup()->CommandId(
            CEikButtonGroupContainer::EMiddleSoftkeyPosition ) );
    TRAP_IGNORE( aCba.AddCommandToStackL(
        CEikButtonGroupContainer::EMiddleSoftkeyPosition,
        EAknSoftkeyEmpty, // no action
        KNullDesC() ) ); // empty label

#else
    (void)aCba;
    ASSERT(0);
#endif // __ENABLE_MSK
    }

// ---------------------------------------------------------------------------
// Sets middle softkey icon.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonUiHelper::SetMiddleSoftKeyIconL(
    CEikButtonGroupContainer& aCba,
    CFbsBitmap* aBitmap,
    CFbsBitmap* aMask )
    {
#ifdef __ENABLE_MSK
    MPX_FUNC( "CMPXCommonUiHelper::SetMiddleSoftKeyIconL" );
    aCba.RemoveCommandFromStack(
        CEikButtonGroupContainer::EMiddleSoftkeyPosition,
        aCba.ButtonGroup()->CommandId(
            CEikButtonGroupContainer::EMiddleSoftkeyPosition ) );
    aCba.AddCommandToStackL(
        CEikButtonGroupContainer::EMiddleSoftkeyPosition,
        EAknSoftkeyContextOptions,
        *aBitmap,
        *aMask );

#else
    (void)aCba;
    (void)aBitmap;
    (void)aMask;
    User::Leave( KErrNotSupported );
#endif // __ENABLE_MSK
    }

// ---------------------------------------------------------------------------
// Generate the next available title from the given title
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CMPXCommonUiHelper::GenerateTitleL( TMPXGeneralCategory aCategory,
                                                    const TDesC& aBaseTitle )
    {
    MPX_FUNC( "CMPXCommonUiHelper::GenerateTitleL" );

    //
    // Find titles of all playlists in the collection
    //
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.Append( KMPXMediaGeneralTitle );

    CMPXMedia* results = FindAllL(aCategory, attrs.Array());
    CleanupStack::PopAndDestroy(&attrs);
    CleanupStack::PushL(results);

    if (!results->IsSupported(KMPXMediaArrayContents))
        {
        User::Leave(KErrArgument);
        }
    const CMPXMediaArray* resultArray = results->Value<CMPXMediaArray>(KMPXMediaArrayContents);
    User::LeaveIfNull( const_cast<CMPXMediaArray*>( resultArray ));

    //
    // shorten this list by removing those that do not contain this pattern:
    // aBaseTitle(*)
    //
    HBufC* matchingPattern = HBufC::NewLC(aBaseTitle.Length()+KWildCardCharacter().Length()+2);
    matchingPattern->Des().Format(KAutoNamingFormat, &aBaseTitle, &KWildCardCharacter);

    CMPXMediaArray* medias = CMPXMediaArray::NewL();
    CleanupStack::PushL(medias);

    TInt count(resultArray->Count());

    for (TInt i=0; i<count; i++)
        {
        const CMPXMedia* media( resultArray->AtL( i ));

        if (!media->IsSupported(KMPXMediaGeneralTitle))
            {
            User::Leave(KErrArgument);
            }

        if (media->ValueText(KMPXMediaGeneralTitle).Match(*matchingPattern) != KErrNotFound)
            {
            medias->AppendL(*media);
            }
        }

    CleanupStack::Pop(medias);
    CleanupStack::PopAndDestroy(2, results); // matchingPattern & results
    CleanupStack::PushL(medias);

    //
    // determine if the suggested title is already used, if so, suggest another one;
    // otherwise, return the suggested title
    //
    TBool found(EFalse);
    TUint autoNumber(0);

    HBufC* autoNumberTextBuf = HBufC::NewLC(KMaxFileName);
    TPtr autoNumberText = autoNumberTextBuf->Des();

    HBufC* suggestedTitle(NULL);

    count = medias->Count();

    while (!found)
        {
        autoNumberText.Format(
            ++autoNumber < 10 ? KAutoNumberPaddedFormat() : KAutoNumberFormat(), autoNumber);

        AknTextUtils::LanguageSpecificNumberConversion(autoNumberText);

        HBufC* testTitle = HBufC::NewLC(aBaseTitle.Length()+autoNumberText.Length()+2);
        testTitle->Des().Format(KAutoNamingFormat, &aBaseTitle, autoNumberTextBuf);
        MPX_DEBUG2("testTitle = %S", testTitle);

        TBool exists(EFalse);
        for (TInt i=0; i<count && !exists; i++)
            {
            if (testTitle->Compare((*medias)[i]->ValueText(KMPXMediaGeneralTitle)) == 0)
                {
                exists = ETrue;
                }
            }

        if (!exists)
            {
            found = ETrue;
            suggestedTitle = testTitle;
            CleanupStack::Pop(testTitle);
            }
        else
            {
            CleanupStack::PopAndDestroy(testTitle);
            }
        }

    CleanupStack::PopAndDestroy(2, medias); // autoNumberTextBuf & medias

    MPX_DEBUG2("SuggestedTitle = %S", suggestedTitle);

    return suggestedTitle;
    }

// ---------------------------------------------------------------------------
// Checks if Exit option should be hidden
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonUiHelper::ExitOptionHiddenL()
    {
    MPX_FUNC( "CMPXCommonUiHelper::ExitOptionHiddenL" );
    TInt flags( 0 );
    CRepository* repository = CRepository::NewL( KCRUidMPXMPFeatures );
    repository->Get( KMPXMPLocalVariation, flags );
    delete repository;
    return !static_cast<TBool>( flags & KMPXEnableExitOption );
    }

// ---------------------------------------------------------------------------
// Sets standalone mode process ID
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonUiHelper::SetStandAloneModePId( TInt64 aPId )
    {
    MPX_FUNC( "CMPXCommonUiHelper::SetStandAloneModePId" );
    TInt err( RProperty::Define(
        KAppUidMusicPlayerX, KMPXStandAloneProcessIDTop32, RProperty::EInt ) );
    MPX_DEBUG2( "CMPXCommonUiHelper::SetStandAloneModePId() RProperty top err = %d", err );
    if ( err == KErrNone || err == KErrAlreadyExists )
        {
        err = RProperty::Define(
            KAppUidMusicPlayerX, KMPXStandAloneProcessIDBottom32, RProperty::EInt );
        MPX_DEBUG2( "CMPXCommonUiHelper::SetStandAloneModePId() RProperty bottom err = %d", err );
        }
    if ( err == KErrNone || err == KErrAlreadyExists )
        {
        MPX_DEBUG2( "CMPXCommonUiHelper::SetStandAloneModePId() proc id = 0x%lx", aPId );
        TInt top32( aPId >> 32 );
        TInt bottom32( aPId & 0xFFFFFFFF );
        err = RProperty::Set( KAppUidMusicPlayerX,
                    KMPXStandAloneProcessIDTop32,
                    top32 );
        MPX_DEBUG2( "CMPXCommonUiHelper::SetStandAloneModePId() set top err = %d", err );
        err = RProperty::Set( KAppUidMusicPlayerX,
                    KMPXStandAloneProcessIDBottom32,
                    bottom32 );
        MPX_DEBUG2( "CMPXCommonUiHelper::SetStandAloneModePId() set bottom err = %d", err );
        }
    return ( err == KErrNone ? ETrue : EFalse );
    }

// ---------------------------------------------------------------------------
// Gets stand alone mode's process id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt64 CMPXCommonUiHelper::StandAloneModePId()
    {
    MPX_FUNC( "CMPXCommonUiHelper::StandAloneModePId" );
    TInt ret( 0 );
    TInt64 procId( 0 );
    TInt err( RProperty::Get(
        KAppUidMusicPlayerX, KMPXStandAloneProcessIDTop32, ret ) );
    if ( err == KErrNone )
        {
        procId = TInt64( ret ) << 32;
        err = RProperty::Get(
            KAppUidMusicPlayerX, KMPXStandAloneProcessIDBottom32, ret );
        if ( err == KErrNone )
            {
            procId += ret;
            }
        else
            {
            procId = 0;
            }
        }
    MPX_DEBUG2( "CMPXCommonUiHelper::StandAloneModePId() combined proc id = 0x%lx", procId );
    return procId;
    }

// ---------------------------------------------------------------------------
// Checks if the application with the provided window group id is in foreground
// This method is required because AppUi()->IsForeground() does not work properly
// when the screen saver is active
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonUiHelper::IsForegroundApplication(TInt aWindowGroupId)
    {
    TBool applicationInForeground(EFalse);
    TApaTaskList taskList(CCoeEnv::Static()->WsSession());
    TApaTask taskInForeground = taskList.FindByPos( 0 );
    if ( taskInForeground.Exists() &&
         (taskInForeground.WgId() == aWindowGroupId))
        {
        applicationInForeground = ETrue;
    	}
    return applicationInForeground;
    }

EXPORT_C TInt CMPXCommonUiHelper::MMCDriveNumber()
    {
#ifdef RD_MULTIPLE_DRIVE
    TInt mmc( 0 );
    TInt r = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRemovableMassStorage, mmc );
    if (r != KErrNone)
        mmc = r;
#else
    TParsePtrC memorycardparse( PathInfo::MemoryCardRootPath() );
    TDriveUnit mmc( memorycardparse.Drive() );
#endif // RD_MULTIPLE_DRIVE
    return mmc;
    }

// ----------------------------------------------------------------------------
// Cancel subsequent operations scheduled by CollectionUiHelper
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonUiHelper::CancelCollectionOperation()
    {
    iCollectionUiHelper->Cancel();
    }

// ----------------------------------------------------------------------------
// MPlayerCommonUi::IsMemoryCardWritable
// ----------------------------------------------------------------------------
//
TBool CMPXCommonUiHelper::IsMemoryCardWritable( TChar& aDrive )
    {
    TBool ret = EFalse;
    TDriveList driveList;
    TInt driveNum( EDriveE );
    CEikonEnv* eikEnv( CEikonEnv::Static() );
    RFs& fs( eikEnv->FsSession() );
    TInt err( fs.DriveList( driveList ) );

    if ( err == KErrNone )
        {
        // search from drive E to drive N (10 drives)
        for (; driveNum <= EDriveN; ++driveNum )
            {
            // Check that volume is valid (ie. not corrupted)
            TVolumeInfo volumeInfo;
            err = eikEnv->FsSession().Volume( volumeInfo, driveNum );

            if ( ( err == KErrNone ) &&
                ( RFs::DriveToChar( driveNum, aDrive ) == KErrNone ) )
                {
                ret = ETrue;
                break;
                }
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMPXCommonUiHelper::PlaylistCreationQueriesL
// -----------------------------------------------------------------------------
//
TBool CMPXCommonUiHelper::PlaylistCreationQueriesL( TDes& aPath )
    {
    MPX_FUNC( "CMPXCommonUiHelper::PlaylistCreationQueriesL" );

    TBool ret = EFalse;

    // Get playlist filename
    HBufC* playlistFileName = HBufC::NewLC( KMPXMaxPlaylistPathLen );
    TPtr playlistFileNamePtr = playlistFileName->Des();
    playlistFileNamePtr.Copy( aPath );
    User::LeaveIfError( GetNextPlaylistNameL( playlistFileNamePtr ) );
    TParsePtrC parse( playlistFileNamePtr );

    HBufC* playlistName =
        HBufC::NewLC( KMPXMaxPlaylistPathLen );
    TPtr playlistNamePtr = playlistName->Des();
    playlistNamePtr.Copy( parse.NameAndExt() );

    if ( aPath.Length() > 0 )
        {
        ret = PlaylistCreateNameExistL( playlistNamePtr , parse );
        if ( ret )
            {
            aPath = parse.DriveAndPath();
            aPath.Append( playlistName->Des() );
            }
        }
    else
        {
        HBufC* saveTitle =
            StringLoader::LoadLC( R_MPX_PLAYLIST_NAME_QUERY_TITLE );
        aPath.Copy( playlistNamePtr );

        ret = AknCommonDialogs::RunSaveDlgNoDirectorySelectionLD( aPath,
            R_MPX_PLAYLIST_MEMORY_SELECTION_LOCATIONS, *saveTitle );

        CleanupStack::PopAndDestroy( saveTitle );
        }

    CleanupStack::PopAndDestroy( playlistName );
    CleanupStack::PopAndDestroy( playlistFileName );

    return ret;
    }

// -----------------------------------------------------------------------------
// Get next available playlist full pathname based on the information passed.
// -----------------------------------------------------------------------------
//
TInt CMPXCommonUiHelper::GetNextPlaylistNameL( TDes& aBuf )
    {
    MPX_FUNC( "CMPXCommonUiHelper::GetNextPlaylistNameL" );
    // Keep track of playlist file path
    HBufC* filePath = aBuf.AllocLC();
    TParsePtrC pathParse( *filePath );
    HBufC* origPlaylistName( NULL );
    if ( !pathParse.NamePresent() )
        {
        // Load default name if filename is missing
        origPlaylistName =
            StringLoader::LoadLC( R_MPX_CUI_DEFAULT_PLAYLIST_NAME );
        }
    else
        {
        origPlaylistName = pathParse.Name().AllocLC();
        }

    if ( !iCollectionUiHelper )
        {
        iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();
        }
    if ( iCollectionUiHelper->TitleExistsL( EMPXPlaylist, *origPlaylistName ) )
        {
        HBufC* newTitle = GenerateTitleL( EMPXPlaylist, *origPlaylistName );
        aBuf.Copy( pathParse.DriveAndPath() );
        aBuf.Append( *newTitle );
        delete newTitle;
        }
    else
        {
        aBuf.Copy( pathParse.DriveAndPath() );
        aBuf.Append( *origPlaylistName );
        }
    CleanupStack::PopAndDestroy( 2, filePath );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Finds all items in the specified category
// -----------------------------------------------------------------------------
//
CMPXMedia* CMPXCommonUiHelper::FindAllL(TMPXGeneralCategory aCategory, const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_FUNC( "CMPXCommonUiHelper::FindAllL" );

    if (!iCollectionUtility)
        {
        User::Leave(KErrArgument);
        }

    CMPXMedia* ret( NULL );
    CMPXMedia* criteria = CMPXMedia::NewL();
    CleanupStack::PushL( criteria );
    criteria->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXGroup );
    criteria->SetTObjectValueL<TMPXGeneralCategory>(
        KMPXMediaGeneralCategory, aCategory );

    // Look up collection UID and set to criteria
    //
    RArray<TUid> ary;
    CleanupClosePushL( ary );
    ary.AppendL( TUid::Uid(EMPXCollectionPluginMusic) );
    TUid musicCollection = iCollectionUtility->CollectionIDL( ary.Array() );
    CleanupStack::PopAndDestroy( &ary );

    criteria->SetTObjectValueL<TUid>( KMPXMediaGeneralCollectionId, musicCollection );

    ret = iCollectionUtility->Collection().FindAllL( *criteria, aAttrs );
    CleanupStack::PopAndDestroy( criteria );
    return ret;
    }

// -----------------------------------------------------------------------------
// Handles the completion of setting a media event.
// -----------------------------------------------------------------------------
//
void CMPXCommonUiHelper::HandleOperationCompleteL( TCHelperOperation aOperation,
                                                   TInt aErr,
                                                   void* aArgument )
    {
    if ( iAddObserver )
        {
        iAddObserver->HandleOperationCompleteL( aOperation, aErr, aArgument );
        }
    iAddObserver = NULL;
    }

// ----------------------------------------------------------------------------
// CMPXCommonUiHelper::PlaylistCreateNameExistL
// ----------------------------------------------------------------------------
//
TBool CMPXCommonUiHelper::PlaylistCreateNameExistL( TDes& aName, const TParseBase& aParse )
    {
    TBool ret( EFalse );
    CMPXFileNameQueryDlg* nameDlg = CMPXFileNameQueryDlg::NewL(
                                    aName, aName, aParse.DriveAndPath(),
                                    EMPXPlaylist, this );
// Cover UI start
//#ifdef __COVER_DISPLAY
        nameDlg->PublishDialogL(
            EMPlayerQueryCreatePlaylist,
            KMPlayerNoteCategory);
//#endif
// Cover UI end
    ret = nameDlg->ExecuteLD( R_MPX_CREATE_PLAYLIST_NAME_QUERY );

    if ( ret )
        {
        if ( iCollectionUiHelper->TitleExistsL( EMPXPlaylist, aName, EMPXOtherType ) )
            {
            HBufC* info = StringLoader::LoadLC( R_MPX_PLAYLIST_NAME_EXIST, aName );
            CAknQueryDialog* query = new(ELeave)CAknQueryDialog( *info );
            if( query->ExecuteLD( R_RENAME_QUERY ) )
                {
                if ( !PlaylistCreateNameExistL( aName, aParse ) )
                    {
                    ret = EFalse;
                    }
                }
             else
                {
                ret = EFalse;
                }
             CleanupStack::PopAndDestroy( info );//query ,info
             }
        }

    return ret;
    }    
//  End of File
