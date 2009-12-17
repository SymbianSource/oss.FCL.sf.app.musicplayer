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
* Description:  Cmpxmusicplayertest test class for STIF Test Framework TestScripter.
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include <badesca.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include <mpxtlshelper.h>
#include "ui_mpxmusicplayertest.h"
#include <COEAUI.H>
#include <EIKENV.H>
#include "edwintestcontrol.h"
#include <mpxmediadrmdefs.h>
#include <mpxmedia.h>
#include <mpxattribute.h>
#include <StringLoader.h>
#include <mpx.rsg>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <mpxcommonui.rsg>
#include <eikbtgpc.h>
#include <mpxinternalcrkeys.h>
#include <APGTASK.H>
#include <avkon.mbg>
#include <AknIconUtils.h>
#include <aknconsts.h>
#include <avkon.rsg>
#include <COEMAIN.H>
//#include <MediaPlayer.rsg> //though it seems a little bit curious to use mediaplayer's resource 4,11,2008
#include <mpxplaybackcommanddefs.h>
#include <AknWaitDialog.h>
#include <eikbtgpc.h>
#include <eikbtgrp.h>
#include <EIKDIALG.H>


class CMPXMedia;
class CMPXMediaArray;
class CEikButtonGroupContainer;
class TApaTaskList;
class CCoeEnv;
class CAknWaitDialog;
class CEikButtonGroupContainer;
class MEikButtonGroup;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// ---------------------------------------------------------------------------
//
void Cmpxmusicplayertest::Delete()
    {
	MPXTlsHelper::Uninitialize();
    }

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "Initialize", Cmpxmusicplayertest::Initialize ),
        ENTRY( "VerifyHostUid", Cmpxmusicplayertest::VerifyHostUid ),
        ENTRY( "VerifyNeedSave", Cmpxmusicplayertest::VerifyNeedSave ),
        ENTRY( "VerifyAllowMove", Cmpxmusicplayertest::VerifyAllowMove ),
        ENTRY( "VerifyLaunchMode", Cmpxmusicplayertest::VerifyLaunchMode ),
        ENTRY( "VerifyFilePath", Cmpxmusicplayertest::VerifyFilePath ),
        ENTRY( "Uninitialize", Cmpxmusicplayertest::Uninitialize ),
       
        //test functions for mpxcommonuihelper.h
        ENTRY( "CMPXCommonUiHelperNewL", Cmpxmusicplayertest::CMPXCommonUiHelperNewL ),
        ENTRY( "CMPXCommonUiHelperDestruct", Cmpxmusicplayertest::CMPXCommonUiHelperDestruct ),
        ENTRY( "UiHelperIsHostMessagingBrowserL", Cmpxmusicplayertest::UiHelperIsHostMessagingBrowserL ),
        ENTRY( "UiHelperIsHostPodcastingAppL", Cmpxmusicplayertest::UiHelperIsHostPodcastingAppL ),
        ENTRY( "UiHelperIsHostMessagingL", Cmpxmusicplayertest::UiHelperIsHostMessagingL ),
        ENTRY( "UiHelperIsHostBrowserL", Cmpxmusicplayertest::UiHelperIsHostBrowserL ),
        ENTRY( "UiHelperDisplayableDurationL", Cmpxmusicplayertest::UiHelperDisplayableDurationL ),
        ENTRY( "UiHelperDisplayableDurationInTextL", Cmpxmusicplayertest::UiHelperDisplayableDurationInTextL ),
        ENTRY( "UiHelperDoSetAsRingingToneL", Cmpxmusicplayertest::UiHelperDoSetAsRingingToneL ),
        ENTRY( "UiHelperIsProfileOfflineModeL", Cmpxmusicplayertest::UiHelperIsProfileOfflineModeL ),
        ENTRY( "UiHelperDefaultDriveLC", Cmpxmusicplayertest::UiHelperDefaultDriveLC ),
        ENTRY( "UiHelperAvailableDriveLC", Cmpxmusicplayertest::UiHelperAvailableDriveLC ),
        ENTRY( "UiHelperDisplayInfoNoteLRSCID", Cmpxmusicplayertest::UiHelperDisplayInfoNoteLRSCID ),
        ENTRY( "UiHelperDisplayInfoNoteLText", Cmpxmusicplayertest::UiHelperDisplayInfoNoteLText ),
        ENTRY( "UiHelperDisplayConfirmNoteLRSCID", Cmpxmusicplayertest::UiHelperDisplayConfirmNoteLRSCID ),
        ENTRY( "UiHelperDisplayConfirmNoteLText", Cmpxmusicplayertest::UiHelperDisplayConfirmNoteLText ),
        ENTRY( "UiHelperUnitConversionL", Cmpxmusicplayertest::UiHelperUnitConversionL ),
        ENTRY( "UiHelperFindPlaylistsL", Cmpxmusicplayertest::UiHelperFindPlaylistsL ),
        ENTRY( "UiHelperAddToSavedPlaylistL", Cmpxmusicplayertest::UiHelperAddToSavedPlaylistL ),
        ENTRY( "UiHelperCreatePlaylistL", Cmpxmusicplayertest::UiHelperCreatePlaylistL ),
        ENTRY( "UiHelperLaunchRenameDialogL", Cmpxmusicplayertest::UiHelperLaunchRenameDialogL ),
        ENTRY( "UiHelperShowWaitNoteL", Cmpxmusicplayertest::UiHelperShowWaitNoteL ),
        ENTRY( "UiHelperDismissWaitNoteL", Cmpxmusicplayertest::UiHelperDismissWaitNoteL ),
        ENTRY( "UiHelperHandleErrorL", Cmpxmusicplayertest::UiHelperHandleErrorL ),
        ENTRY( "UiHelperSetMiddleSoftKeyLabelL", Cmpxmusicplayertest::UiHelperSetMiddleSoftKeyLabelL ),
        ENTRY( "UiHelperRemoveMiddleSoftKeyLabel", Cmpxmusicplayertest::UiHelperRemoveMiddleSoftKeyLabel ),
        ENTRY( "UiHelperSetMiddleSoftKeyIconL", Cmpxmusicplayertest::UiHelperSetMiddleSoftKeyIconL ),  
        ENTRY( "UiHelperGenerateTitleL", Cmpxmusicplayertest::UiHelperGenerateTitleL ),
        ENTRY( "UiHelperExitOptionHiddenL", Cmpxmusicplayertest::UiHelperExitOptionHiddenL ),
        ENTRY( "UiHelperSetStandAloneModePId", Cmpxmusicplayertest::UiHelperSetStandAloneModePId ),
        ENTRY( "UiHelperStandAloneModePId", Cmpxmusicplayertest::UiHelperStandAloneModePId ),
        ENTRY( "UiHelperIsForegroundApplication", Cmpxmusicplayertest::UiHelperIsForegroundApplication ),
        ENTRY( "UiHelperMMCDriveNumber", Cmpxmusicplayertest::UiHelperMMCDriveNumber ),
        ENTRY( "UiHelperCancelCollectionOperation", Cmpxmusicplayertest::UiHelperCancelCollectionOperation ),
        
        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::Initialize
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::Initialize( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::Initialize"));
    TRAPD( err , MPXTlsHelper::InitializeL());
    if ( err != KErrNone )
    	{
    	iLog->Log(_L("InitializeL returned: %d"), err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::Uninitialize
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::Uninitialize( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::Uninitialize"));
    TInt err = KErrNone;
    MPXTlsHelper::Uninitialize();
    if ( err != KErrNone )
    	{
    	iLog->Log(_L("Uninitialize returned: %d"), err);
    	}
    return err;
    }

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::VerifyHostUid
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::VerifyHostUid( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxmusicplayertest::VerifyHostUid"));
    TInt err = KErrNone;
    TPtrC string;
	if ( KErrNone == aItem.GetNextString(string) )
		{
		if ( string == KTagKErrNone )
			{
			TRAP( err , MPXTlsHelper::SetHostUidL(KHostUid));
			if ( err == KErrNone )
				{
				if ( KHostUid == MPXTlsHelper::HostUid() )
					{
					iLog->Log(_L("Host Uid match!"));
					}
				else
					{
					iLog->Log(_L("ERROR: Host Uid doesn't match!"));
					err = KErrGeneral;
					}
				}
			else
				{
				iLog->Log(_L("SetHostUidL returned: %d"), err);
				}
			}
		else if ( string == KTagKErrNotReady )
			{
			TRAP( err , MPXTlsHelper::SetHostUidL(KHostUid));
			if ( err == KErrNotReady )
				{
				err = KErrNone;
				}
			else
				{
				iLog->Log(_L("ERROR: %d"), err);
				}
			}
		else
			{
			iLog->Log(_L("Bad parameter on config file"));
			err = KErrBadTestParameter;
			}
		}
	else
		{
		iLog->Log(_L("Bad parameter on config file"));
		err = KErrBadTestParameter;
		}

    return err;
    }

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::VerifyNeedSave
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::VerifyNeedSave( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::VerifyNeedSave"));
    MPXTlsHelper::SetNeedSave(EFalse);
    if ( MPXTlsHelper::NeedSave() )
    	{
    	iLog->Log(_L("ERROR: Flag doesn't match! - true"));
    	return KErrGeneral;
    	}

    MPXTlsHelper::SetNeedSave(ETrue);
    if ( !MPXTlsHelper::NeedSave() )
    	{
    	iLog->Log(_L("ERROR: Flag doesn't match! - false"));
    	return KErrGeneral;
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::VerifyAllowMove
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::VerifyAllowMove( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::VerifyAllowMove"));
    MPXTlsHelper::SetAllowMove(EFalse);
    if ( MPXTlsHelper::AllowMove() )
    	{
    	iLog->Log(_L("ERROR: Flag doesn't match! - true"));
    	return KErrGeneral;
    	}

    MPXTlsHelper::SetAllowMove(ETrue);
    if ( !MPXTlsHelper::AllowMove() )
    	{
    	iLog->Log(_L("ERROR: Flag doesn't match! - false"));
    	return KErrGeneral;
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::VerifyLaunchMode
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::VerifyLaunchMode( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxmusicplayertest::VerifyLaunchMode"));
    TInt err = KErrNone;
    TPtrC string;
	if ( KErrNone == aItem.GetNextString(string) )
		{
		if ( string == KTagKErrNone )
			{
			TRAP( err , MPXTlsHelper::SetLaunchModeL(EMPXLaunchModePlaying));
			if ( err == KErrNone )
				{
				if ( EMPXLaunchModePlaying == MPXTlsHelper::LaunchMode() )
					{
					iLog->Log(_L("Launch mode match!"));
					}
				else
					{
					iLog->Log(_L("ERROR: Launch mode doesn't match!"));
					err = KErrGeneral;
					}
				}
			else
				{
				iLog->Log(_L("SetLaunchModeL returned: %d"), err);
				}
			}
		else if ( string == KTagKErrNotReady )
			{
			TRAP( err , MPXTlsHelper::SetLaunchModeL(EMPXLaunchModePlaying));
			if ( err == KErrNotReady )
				{
				err = KErrNone;
				}
			else
				{
				iLog->Log(_L("ERROR: %d"), err);
				}
			}
		else
			{
			iLog->Log(_L("Bad parameter on config file"));
			err = KErrBadTestParameter;
			}
		}
	else
		{
		iLog->Log(_L("Bad parameter on config file"));
		err = KErrBadTestParameter;
		}

    return err;
    }

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::VerifyFilePath
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::VerifyFilePath( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::VerifyFilePath"));
    TInt err = KErrNone;
    TBuf<120> KPath;
    KPath.Append(_L("Cmpxmusicplayertest"));

    MPXTlsHelper::SetFilePath(KPath);
    if ( KPath == MPXTlsHelper::FilePath() )
    	{
    	iLog->Log(_L("File path match!"));
    	}
    else
    	{
    	iLog->Log(_L("ERROR: File path doesn't match!"));
    	err = KErrGeneral;
    	}

    return err;
    }

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::CMPXCommonUiHelperNewL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::CMPXCommonUiHelperNewL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::CMPXCommonUiHelperNewL"));
    TInt err = KErrNone;    
    iUtilityForUiHelper=MMPXCollectionUtility::NewL();
    TRAP(err,iUiHelper = CMPXCommonUiHelper::NewL(iUtilityForUiHelper));
    if ( err == KErrNone )
    	{
    	iLog->Log(_L("Creation of CMPXCommonUiHelper succeeds!"));
    	}
    else
    	{
    	iLog->Log(_L("ERROR: Creation of CMPXCommonUiHelper fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::CMPXCommonUiHelperDestruct
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::CMPXCommonUiHelperDestruct( CStifItemParser& /*aItem*/ )
    {
	TInt err = KErrNone;  
    if(iUiHelper)
    	{
    	delete iUiHelper;
    	iUiHelper=NULL;
    	    if(iUtilityForUiHelper)
    	    	{
			    	iUtilityForUiHelper->Close();
			    	iUtilityForUiHelper=NULL;
    	    	}
    	}
    else
    	{
    	iLog->Log(_L("Cmpxmusicplayertest::CMPXCommonUiHelperDestruct"));  
    	TRAP(err,iUtilityForUiHelper=MMPXCollectionUtility::NewL());
    	TRAP(err,iUiHelper = CMPXCommonUiHelper::NewL(iUtilityForUiHelper) );
    	TRAP(err,delete iUiHelper);
    	iUiHelper=NULL;
    	iUtilityForUiHelper->Close();
    	iUtilityForUiHelper=NULL;
    	if ( err == KErrNone )
    		{
    		iLog->Log(_L("CMPXCommonUiHelperDestruct succeeds!"));
    		}
    	else
    		{
    		iLog->Log(_L("ERROR:CMPXCommonUiHelperDestruct fails!%d returns!"),err);
    		}
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperIsHostMessagingBrowserL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperIsHostMessagingBrowserL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperIsHostMessagingBrowserL"));
    TInt err = KErrNone;  
    TBool isHostMessagingBrowser;
    TRAP(err,isHostMessagingBrowser=iUiHelper->IsHostMessagingBrowserL());
    if(err==KErrNone)
    	{
    	if(isHostMessagingBrowser)
    		{
    		iLog->Log(_L("UiHelperIsHostMessagingBrowserL succeeds with TBool ETrue returned!"));
    		}
    	else
    		{
    		iLog->Log(_L("UiHelperIsHostMessagingBrowserL succeeds with TBool EFalse returned!"));
    		}
    	}
    else
    	{
    	iLog->Log(_L("UiHelperIsHostMessagingBrowserL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperIsHostPodcastingAppL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperIsHostPodcastingAppL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperIsHostPodcastingAppL"));
    TInt err = KErrNone;  
    TBool isHostPodcastingApp;
    TRAP( err , isHostPodcastingApp=iUiHelper->IsHostPodcastingAppL());
    if(err == KErrNone)
    	{
    	if(isHostPodcastingApp)
    		{
    		iLog->Log(_L("UiHelperIsHostPodcastingAppL succeeds with TBool ETrue returned!"));
    		}
    	else
    		{
    		iLog->Log(_L("UiHelperIsHostPodcastingAppL succeeds with TBool EFalse returned!"));
    		}
    	}
    else
    	{
    	iLog->Log(_L("UiHelperIsHostPodcastingAppL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperIsHostMessagingL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperIsHostMessagingL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperIsHostMessagingL"));
    TInt err = KErrNone;  
    TBool isHostMessaging;
    TRAP( err , isHostMessaging=iUiHelper->IsHostMessagingL());
    if(err==KErrNone)
    	{
    	if(isHostMessaging)
    		{
    		iLog->Log(_L("UiHelperIsHostMessagingL succeeds with TBool ETrue returned!"));
    		}
    	else
    		{
    		iLog->Log(_L("UiHelperIsHostMessagingL succeeds with TBool EFalse returned!"));
    		}
    	}
    else
    	{
    	iLog->Log(_L("UiHelperIsHostMessagingL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperIsHostBrowserL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperIsHostBrowserL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperIsHostBrowserL"));
    TInt err = KErrNone;  
    TBool isHostBrowser;
    TRAP( err , isHostBrowser=iUiHelper->IsHostBrowserL());
    if( err == KErrNone )
    	{
    	if( isHostBrowser )
    		{
    		iLog->Log(_L("UiHelperIsHostBrowserL succeeds with TBool ETrue returned!"));
    		}
    	else
    		{
    		iLog->Log(_L("UiHelperIsHostBrowserL succeeds with TBool EFalse returned!"));
    		}
    	}
    else
    	{
    	iLog->Log(_L("UiHelperIsHostBrowserL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperDisplayableDurationL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperDisplayableDurationL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperDisplayableDurationL"));
    TInt err = KErrNone;    
    TInt64 duration=100;
    HBufC* recievedDuration;
    TRAP( err , recievedDuration = iUiHelper->DisplayableDurationL(duration));
    delete recievedDuration;
    if ( err == KErrNone )
    	{
    	iLog->Log(_L("UiHelperDisplayableDurationL succeeds!"));
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperDisplayableDurationL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperDisplayableDurationInTextL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperDisplayableDurationInTextL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperDisplayableDurationInTextL"));
    TInt err = KErrNone;    
    TInt64 duration=1000000;
    HBufC* recievedDuration;
    TRAP( err , recievedDuration=iUiHelper->DisplayableDurationInTextL(duration));
    delete recievedDuration;
    if ( err == KErrNone )
    	{
    	iLog->Log(_L("UiHelperDisplayableDurationInTextL succeeds!"));
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperDisplayableDurationInTextL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperDoSetAsRingingToneL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperDoSetAsRingingToneL( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperDoSetAsRingingToneL"));
    TInt err = KErrNone;    
    CMPXMedia* media=CMPXMedia::NewL();
    CleanupStack::PushL(media);
    media->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralUri )       ,EMPXSong);
    media->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralTitle)      ,EMPXSong);
    media->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdDrm,     EMPXMediaDrmProtected )     ,EMPXSong);
    media->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdDrm,     EMPXMediaDrmRightsStatus )  ,EMPXSong);
    media->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdDrm,     EMPXMediaDrmCanSetAutomated),EMPXSong);
    TPtrC string;
    if ( KErrNone == aItem.GetNextString(string) )
    	{
    	TBuf<120> ringToneFileName;
    	ringToneFileName.Append(KRingToneFilePath);
    	ringToneFileName.Append(string);
    	media->SetTextValueL( KMPXMediaGeneralUri, ringToneFileName );
    	}
    else
    	{
    	media->SetTextValueL( KMPXMediaGeneralUri, KRingToneFileName );
    	}   
    TRAP( err , iUiHelper->DoSetAsRingingToneL(*media,EFalse));
    CleanupStack::PopAndDestroy(media);
    if ( err == KErrNone )
    	{
    	iLog->Log(_L("UiHelperDoSetAsRingingToneL succeeds!"));
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperDoSetAsRingingToneL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperIsProfileOfflineModeL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperIsProfileOfflineModeL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperIsProfileOfflineModeL"));
    TInt err = KErrNone;    
    TBool isOfflineMode;
    TRAP( err , isOfflineMode=iUiHelper->IsProfileOfflineModeL());
    if ( err == KErrNone )
    	{
    	if(isOfflineMode)
    		{
    		iLog->Log(_L("UiHelperIsProfileOfflineModeL succeeds with TBool ETrue returned!"));
    		}
    	else
    		{
    		iLog->Log(_L("UiHelperIsProfileOfflineModeL succeeds with TBool EFalse returned!"));
    		}
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperIsProfileOfflineModeL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperDefaultDriveLC
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperDefaultDriveLC( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperDefaultDriveLC"));
    TInt err = KErrNone;    
    TRAP( err , CleanupStack::PopAndDestroy(iUiHelper->DefaultDriveLC()));
    if ( err == KErrNone )
    	{    	
    	iLog->Log(_L("UiHelperDefaultDriveLC succeeds!"));   		
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperDefaultDriveLC fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperAvailableDriveLC
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperAvailableDriveLC( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperAvailableDriveLC"));
    TInt err = KErrNone;    
    TRAP( err , CleanupStack::PopAndDestroy(iUiHelper->AvailableDriveLC(0)));   
    if ( err == KErrNone )
    	{    	
    	iLog->Log(_L("UiHelperAvailableDriveLC succeeds!"));   		
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperAvailableDriveLC fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperDisplayInfoNoteL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperDisplayInfoNoteLRSCID( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperDisplayInfoNoteLRSCID"));
    TInt err = KErrNone; 
    TPtrC string;
    if ( KErrNone == aItem.GetNextString(string) )
        {
        if(string==KMPXIliegalCharacters)
        	{
        	TRAP( err , iUiHelper->DisplayInfoNoteL(R_MPX_QTN_FLDR_ILLEGAL_CHARACTERS));   ;
        	}
        else if(string==KMPXFileNotFound)
        	{
        	TRAP( err , iUiHelper->DisplayInfoNoteL(R_MPX_FILE_NOT_FOUND_TEXT));   
        	}
        if ( err == KErrNone )
        	{    	
        	iLog->Log(_L("UiHelperDisplayInfoNoteLRSCID succeeds!"));   		
        	}
        else
        	{
        	iLog->Log(_L("ERROR:UiHelperDisplayInfoNoteLRSCID fails!%d returns!"),err);
        	}
        } 
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperDisplayInfoNoteLText
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperDisplayInfoNoteLText( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperDisplayInfoNoteLText"));
    TInt err = KErrNone; 
    TPtrC string;
    if ( KErrNone == aItem.GetNextString(string) )
        {
        if(string==KMPXIliegalCharacters)
        	{
        	_LIT(KilligalCharacters,"Name cannot contain the characters < > \\ /\" |:*?");
        	HBufC* text=HBufC::New(50);
        	(*text) = KilligalCharacters;
        	TRAP( err , iUiHelper->DisplayInfoNoteL(*text)); 
        	delete text;
        	}
        else if(string==KMPXFileNotFound)
        	{
        	_LIT(KfileNotFound,"File not found. Operation cancelled.");
        	HBufC* text=HBufC::New(40);
        	(*text) = KfileNotFound;
        	TRAP( err , iUiHelper->DisplayInfoNoteL(*text));
        	delete text;
        	}
        if ( err == KErrNone )
        	{    	
        	iLog->Log(_L("UiHelperDisplayInfoNoteLText succeeds!"));   		
        	}
        else
        	{
        	iLog->Log(_L("ERROR:UiHelperDisplayInfoNoteLText fails!%d returns!"),err);
        	}
        } 
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperDisplayConfirmNoteLRSCID
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperDisplayConfirmNoteLRSCID( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperDisplayConfirmNoteLRSCID"));
    TInt err = KErrNone; 
    TPtrC string;
    if ( KErrNone == aItem.GetNextString(string) )
        {
        if(string==KMPXSaveToCollectionNote)
        	{
        	TRAP( err , iUiHelper->DisplayConfirmNoteL(R_MPX_SAVED_TO_COLLECTION_NOTE));   ;
        	}
        else if(string==KMPXSaveNote)
        	{
        	TRAP( err , iUiHelper->DisplayConfirmNoteL(R_MPX_SAVED_NOTE));   
        	}
        if ( err == KErrNone )
        	{    	
        	iLog->Log(_L("UiHelperDisplayConfirmNoteLRSCID succeeds!"));   		
        	}
        else
        	{
        	iLog->Log(_L("ERROR:UiHelperDisplayConfirmNoteLRSCID fails!%d returns!"),err);
        	}
        } 
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperDisplayConfirmNoteLText
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperDisplayConfirmNoteLText( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperDisplayConfirmNoteLText"));
    TInt err = KErrNone; 
    TPtrC string;
    if ( KErrNone == aItem.GetNextString(string) )
        {
        if(string==KMPXSaveToCollectionNote)
        	{
        	_LIT(KilligalCharacters,"Saved to Music library");
        	HBufC* text=HBufC::New(50);
        	(*text) = KilligalCharacters;
        	TRAP( err , iUiHelper->DisplayConfirmNoteL(*text)); 
        	delete text;
        	}
        else if(string==KMPXSaveNote)
        	{
        	_LIT(KfileNotFound,"Saved to Gallery");
        	HBufC* text=HBufC::New(40);
        	(*text) = KfileNotFound;
        	TRAP( err , iUiHelper->DisplayConfirmNoteL(*text));
        	delete text;
        	}
        if ( err == KErrNone )
        	{    	
        	iLog->Log(_L("UiHelperDisplayConfirmNoteLText succeeds!"));   		
        	}
        else
        	{
        	iLog->Log(_L("ERROR:UiHelperDisplayConfirmNoteLText fails!%d returns!"),err);
        	}
        } 
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperUnitConversionL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperUnitConversionL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperUnitConversionL"));
    TInt err = KErrNone;    
    HBufC* conversion;
    TInt64 byte=100000;
    TRAP( err , conversion=iUiHelper->UnitConversionL(byte));   
    delete conversion;
    if ( err == KErrNone )
    	{    	
    	iLog->Log(_L("UiHelperUnitConversionL succeeds!"));   		
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperUnitConversionL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperFindPlaylistsL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperFindPlaylistsL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperFindPlaylistsL"));
    TInt err = KErrNone;    
    CMPXMedia* media;
    TRAP( err , media=iUiHelper->FindPlaylistsL()); 
    delete media;
    if ( err == KErrNone )
    	{    	
    	iLog->Log(_L("UiHelperFindPlaylistsL succeeds!"));   		
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperFindPlaylistsL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperAddToSavedPlaylistL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperAddToSavedPlaylistL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperAddToSavedPlaylistL"));
    TInt err = KErrNone;  
    
    CMPXMedia* mediaPlaylist=CMPXMedia::NewL();
    CleanupStack::PushL(mediaPlaylist);
    CMPXMediaArray* mediaArrayForPlaylist=CMPXMediaArray::NewL();
    mediaPlaylist->SetCObjectValueL<CMPXMediaArray>(KMPXMediaArrayContents,mediaArrayForPlaylist);
    CMPXMedia* mediaPlaylistEntry1=CMPXMedia::NewL();
    CleanupStack::PushL(mediaPlaylistEntry1);
    mediaPlaylistEntry1->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralTitle ),EMPXSong);
    mediaPlaylistEntry1->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralId )   ,EMPXSong);
    mediaPlaylistEntry1->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralDrive ),EMPXSong);
    mediaArrayForPlaylist->AppendL(mediaPlaylistEntry1);
        
    CMPXMedia* mediaPlaylistEntry2=CMPXMedia::NewL();
    CleanupStack::PushL(mediaPlaylistEntry2);
    mediaPlaylistEntry2->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralTitle ) ,EMPXSong);
    mediaPlaylistEntry2->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralId )    ,EMPXSong);
    mediaPlaylistEntry2->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralDrive ) ,EMPXSong);
    mediaArrayForPlaylist->AppendL(mediaPlaylistEntry2);
        
    CMPXMedia* mediaTrack=CMPXMedia::NewL();
    CleanupStack::PushL(mediaTrack);
    CMPXMediaArray* mediaArrayForTrack=CMPXMediaArray::NewL();    
    mediaTrack->SetCObjectValueL<CMPXMediaArray>(KMPXMediaArrayContents,mediaArrayForTrack);
    CMPXMedia* mediaTrackEntry1=CMPXMedia::NewL();
    CleanupStack::PushL(mediaTrackEntry1);
    mediaTrackEntry1->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralType ),    EMPXSong);
    mediaTrackEntry1->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralCategory ),EMPXSong);
    mediaTrackEntry1->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralId ),	  EMPXSong);  
    mediaTrackEntry1->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralTitle ),	  EMPXSong);
    mediaArrayForTrack->AppendL(mediaTrackEntry1);
      
    CMPXMedia* mediaTrackEntry2=CMPXMedia::NewL();
    CleanupStack::PushL(mediaTrackEntry2);
    mediaTrackEntry2->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralType ),    EMPXSong);
    mediaTrackEntry2->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralCategory ),EMPXSong);
    mediaTrackEntry2->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralId ),	  EMPXSong);  
    mediaTrackEntry2->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralTitle ),	  EMPXSong);
    mediaArrayForTrack->AppendL(mediaTrackEntry2);
    
    TBool isPlaylistSaved(EFalse);
    TRAP(err,isPlaylistSaved=iUiHelper->AddToSavedPlaylistL(*mediaPlaylist,*mediaTrack,this,this));

    CleanupStack::Pop(mediaTrackEntry2);
    CleanupStack::Pop(mediaTrackEntry1);
    CleanupStack::PopAndDestroy(mediaTrack);
    CleanupStack::Pop(mediaPlaylistEntry2);
    CleanupStack::Pop(mediaPlaylistEntry1);
    CleanupStack::PopAndDestroy(mediaPlaylist);
    mediaPlaylist=NULL;
    delete mediaArrayForPlaylist;
    delete mediaArrayForTrack;
    mediaArrayForTrack=NULL;

    if ( err == KErrNone )
    	{
    	iLog->Log(_L("UiHelperAddToSavedPlaylistL succeeds!"));
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperAddToSavedPlaylistL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperCreatePlaylistL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperCreatePlaylistL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperCreatePlaylistL"));
    TInt err = KErrNone;  
         
    CMPXMedia* mediaTrack=CMPXMedia::NewL();
    CleanupStack::PushL(mediaTrack);
    CMPXMediaArray* mediaArrayForTrack=CMPXMediaArray::NewL();    
    mediaTrack->SetCObjectValueL<CMPXMediaArray>(KMPXMediaArrayContents,mediaArrayForTrack);
    CMPXMedia* mediaTrackEntry1=CMPXMedia::NewL();
    CleanupStack::PushL(mediaTrackEntry1);
    mediaTrackEntry1->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralType ),    EMPXSong);
    mediaTrackEntry1->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralCategory ),EMPXSong);
    mediaTrackEntry1->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralId ),	  EMPXSong);  
    mediaTrackEntry1->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralTitle ),	  EMPXSong);
    mediaArrayForTrack->AppendL(mediaTrackEntry1);
      
    CMPXMedia* mediaTrackEntry2=CMPXMedia::NewL();
    CleanupStack::PushL(mediaTrackEntry2);
    mediaTrackEntry2->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralType ),    EMPXSong);
    mediaTrackEntry2->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralCategory ),EMPXSong);
    mediaTrackEntry2->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralId ),	  EMPXSong);  
    mediaTrackEntry2->SetTObjectValueL<TMPXGeneralCategory>(TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralTitle ),	  EMPXSong);
    mediaArrayForTrack->AppendL(mediaTrackEntry2);

    TBool isPlaylistCreated = EFalse;
    TRAP(err,isPlaylistCreated=iUiHelper->CreatePlaylistL( *mediaTrack, this, this ));
    
    CleanupStack::Pop(mediaTrackEntry2);
    CleanupStack::Pop(mediaTrackEntry1);
    CleanupStack::PopAndDestroy(mediaTrack);
  
    if ( err == KErrNone )
    	{
    	iLog->Log(_L("UiHelperCreatePlaylistL succeeds!"));
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperCreatePlaylistL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperLaunchRenameDialogL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperLaunchRenameDialogL( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperLaunchRenameDialogL"));
    TInt err = KErrNone; 
    TBuf<120> oldName;
    TBuf<120> newName;
    TBuf<120> path;
    TPtrC ptrOldname;
    TPtrC ptrNewname;
    TPtrC ptrPath;
    if ( KErrNone == aItem.GetNextString(ptrOldname) )
    	{
    	oldName.Append(ptrOldname);
    	if(KErrNone == aItem.GetNextString(ptrNewname))
    		{
    		newName.Append(ptrNewname);
    		if(KErrNone == aItem.GetNextString(ptrPath))
    			{
    			path.Append(ptrPath);
    			TInt recievedInt;
    			TRAP(err,recievedInt=iUiHelper->LaunchRenameDialogL(oldName,newName,path));
    			if ( err == KErrNone )
		    		{
		    		iLog->Log(_L("UiHelperLaunchRenameDialogL succeeds!"));
		    		return err;
		    		}
		    	else
		    		{
		    		iLog->Log(_L("ERROR:UiHelperLaunchRenameDialogL fails!%d returns!"),err);
		    		return err;
		    		}
    			}
    		else
    			{
    			iLog->Log(_L("UiHelperLaunchRenameDialogL cannot get the path!"));
    			return KErrBadTestParameter;
    			}
    		}
    	else
    		{
    		iLog->Log(_L("UiHelperLaunchRenameDialogL cannot get the new name!"));
    		return KErrBadTestParameter;
    		}
    	}
    else
    	{
    	iLog->Log(_L("UiHelperLaunchRenameDialogL cannot get the old name!"));
    	return KErrBadTestParameter;
    	}   
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperShowWaitNoteL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperShowWaitNoteL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperShowWaitNoteL"));
    TInt err = KErrNone;  
    HBufC* text = StringLoader::LoadLC(
                                R_MPX_QTN_NMP_NOTE_ADDING_ONE_SONG );
    iUiHelper->ShowWaitNoteL(*text,R_AVKON_SOFTKEYS_EMPTY,EFalse,this);
    CleanupStack::PopAndDestroy( text );
    if ( err == KErrNone )
    	{
    	iLog->Log(_L("UiHelperShowWaitNoteL succeeds!"));
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperShowWaitNoteL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperDismissWaitNoteL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperDismissWaitNoteL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperDismissWaitNoteL"));
    TInt err = KErrNone;  
    TRAP(err,iUiHelper->DismissWaitNoteL());  
    iLog->Log(_L("UiHelperDismissWaitNoteL succeeds!"));	
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperHandleErrorL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperHandleErrorL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperHandleErrorL"));
    TInt err = KErrNone;    
    TRAP( err , iUiHelper->HandleErrorL(KMPXRightsAboutToExpire));  
    if ( err == KErrNone )
    	{
    	iLog->Log(_L("UiHelperHandleErrorL succeeds!"));
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperHandleErrorL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperSetMiddleSoftKeyLabelL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperSetMiddleSoftKeyLabelL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperSetMiddleSoftKeyLabelL"));
    TInt err = KErrNone;  
    CEikButtonGroupContainer::TUse use = CEikButtonGroupContainer::ECba;
    CEikButtonGroupContainer::TOrientation oriecntation = CEikButtonGroupContainer::EVertical;
    CEikButtonGroupContainer* groupContainer = CEikButtonGroupContainer::NewL(
    																		use,
    																		oriecntation,
    																		this,
    																		NULL,
    																		CEikButtonGroupContainer::EDelayActivation);
    CleanupStack::PushL(groupContainer);
    TBuf<10> mskText;
    _LIT(KMSKText,"MskKey");
    mskText.Append(KMSKText);
    groupContainer->AddCommandToStackL(CEikButtonGroupContainer::EMiddleSoftkeyPosition, EPbCmdNext,mskText);
    //cannot find the right resource ID
    TRAP(err,iUiHelper->SetMiddleSoftKeyLabelL(*groupContainer,R_MPX_STATUS_PANE,EPbCmdPlay)); 
    CleanupStack::PopAndDestroy(groupContainer);
    if ( err == KErrNone )
    	{
    	iLog->Log(_L("UiHelperSetMiddleSoftKeyLabelL succeeds!"));
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperSetMiddleSoftKeyLabelL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperRemoveMiddleSoftKeyLabel
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperRemoveMiddleSoftKeyLabel( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperRemoveMiddleSoftKeyLabel"));
    TInt err = KErrNone;   
    CEikButtonGroupContainer::TUse use=CEikButtonGroupContainer::ECba;
    CEikButtonGroupContainer::TOrientation oriecntation=CEikButtonGroupContainer::EVertical;
    CEikButtonGroupContainer* groupContainer;
    TRAP(err,groupContainer=CEikButtonGroupContainer::NewL(
    																		use,
    																		oriecntation,
    																		this,
    																		NULL,
    																		CEikButtonGroupContainer::EDelayActivation));
    CleanupStack::PushL( groupContainer );//added on 2009.3.18
    //    CleanupStack::PushL(groupContainer);
    TBuf<10> mskText;
    _LIT(KMSKText,"MskKey");
    mskText.Append(KMSKText);
    TRAP(err,groupContainer->AddCommandToStackL(CEikButtonGroupContainer::EMiddleSoftkeyPosition, EPbCmdNext,mskText));
    
    iUiHelper->RemoveMiddleSoftKeyLabel(*groupContainer);
    CleanupStack::PopAndDestroy(groupContainer);
    iLog->Log(_L("UiHelperRemoveMiddleSoftKeyLabel succeeds!"));
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperSetMiddleSoftKeyIconL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperSetMiddleSoftKeyIconL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperSetMiddleSoftKeyIconL"));
    TInt err = KErrNone; 
    CEikButtonGroupContainer::TUse use=CEikButtonGroupContainer::ECba;
    CEikButtonGroupContainer::TOrientation oriecntation=CEikButtonGroupContainer::EVertical;
    CEikButtonGroupContainer* groupContainer=CEikButtonGroupContainer::NewL(
    																		use,
    																		oriecntation,
    																		this,
    																		NULL,
    																		CEikButtonGroupContainer::EDelayActivation);
    CleanupStack::PushL(groupContainer);
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    AknIconUtils::CreateIconLC( bitmap, mask, 
                               TFileName(KAvkonBitmapFile), 
                               (TInt)EMbmAvkonQgn_note_progress,
                               (TInt)EMbmAvkonQgn_note_progress_mask );
    TBuf<10> mskText;
    _LIT(KMSKText,"MskKey");
    mskText.Append(KMSKText);
    groupContainer->AddCommandToStackL(CEikButtonGroupContainer::EMiddleSoftkeyPosition, EPbCmdNext,mskText);
    
    TRAP(err,iUiHelper->SetMiddleSoftKeyIconL(*groupContainer,bitmap,mask));    
    CleanupStack::Pop(2);
    CleanupStack::PopAndDestroy(groupContainer); 
    if ( err == KErrNone )
    	{
    	iLog->Log(_L("UiHelperSetMiddleSoftKeyIconL succeeds!"));
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperSetMiddleSoftKeyIconL fails!%d returns!"),err);
    	}
    return err;
    }

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperGenerateTitleL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperGenerateTitleL( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperGenerateTitleL"));
    TInt err = KErrNone;   
    TPtrC string;
    if ( KErrNone == aItem.GetNextString(string) )
    	{
    	HBufC* retCategoryTitle;
    	TBuf<120> newCategoryTitle;
    	newCategoryTitle.Append(string);
    	//Maybe we could modify the cfg file to make it really generate a new title.
    	TRAP(err,retCategoryTitle=iUiHelper->GenerateTitleL(EMPXSong,newCategoryTitle);); 
    	if ( err == KErrNone )
    		{
    		iLog->Log(_L("UiHelperGenerateTitleL succeeds!"));
    		return err;
    		}
    	else
    		{
    		iLog->Log(_L("ERROR:UiHelperGenerateTitleL fails!%d returns!"),err);
    		return err;
    		}
    	} 
    else
    	{
    	return KErrBadTestParameter;
    	}  
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperExitOptionHiddenL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperExitOptionHiddenL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperExitOptionHiddenL"));
    TInt err = KErrNone;   
    TBool isHidden;
    TRAP(err,isHidden=iUiHelper->ExitOptionHiddenL()); 
    if ( err == KErrNone )
    	{
    	if(isHidden)
    		{
    		iLog->Log(_L("UiHelperExitOptionHiddenL succeeds with TBool ETrue!"));
    		}
    	else
    		{
    		iLog->Log(_L("UiHelperExitOptionHiddenL succeeds with TBool EFalse!"));
    		}
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperExitOptionHiddenL fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperSetStandAloneModePId
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperSetStandAloneModePId( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperSetStandAloneModePId"));
    TInt err = KErrNone; 
    RProcess proc;
    TBool isSetDone;
    isSetDone=iUiHelper->SetStandAloneModePId( proc.Id().Id());
    if ( err == KErrNone )
    	{
    	if(isSetDone)
    		{
    		iLog->Log(_L("UiHelperSetStandAloneModePId succeeds with TBool ETrue!"));
    		}
    	else
    		{
    		iLog->Log(_L("UiHelperSetStandAloneModePId succeeds with TBool EFalse!"));
    		}
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperSetStandAloneModePId fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperStandAloneModePId
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperStandAloneModePId( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperStandAloneModePId"));
    TInt err = KErrNone; 
    TInt64 retPId;
    retPId=iUiHelper->StandAloneModePId();
    if ( err == KErrNone )
    	{
    	iLog->Log(_L("UiHelperStandAloneModePId succeeds with StandAloneModePId returned %d!"),retPId);
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperStandAloneModePId fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperIsForegroundApplication
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperIsForegroundApplication( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperIsForegroundApplication"));
    TInt err = KErrNone; 
    TApaTaskList taskList(CCoeEnv::Static()->WsSession());
    TApaTask taskInForeground = taskList.FindByPos( 0 );
    TInt WindowGroupId=taskInForeground.WgId();
    TBool isForeApp;
    isForeApp=iUiHelper->IsForegroundApplication(WindowGroupId);
    if ( err == KErrNone )
    	{
    	if(isForeApp)
    		{
    		iLog->Log(_L("UiHelperIsForegroundApplication succeeds.It's foreground application."));
    		}
    	else
    		{
    		iLog->Log(_L("UiHelperIsForegroundApplication succeeds.It's not foreground application."));
    		}
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperIsForegroundApplication fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperMMCDriveNumber
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperMMCDriveNumber( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperMMCDriveNumber"));
    TInt err = KErrNone;    
    TInt DriveNO;
    DriveNO=iUiHelper->MMCDriveNumber();
    if ( err == KErrNone )
    	{
    	iLog->Log(_L("UiHelperMMCDriveNumber succeeds.MMCDriveNumber is %d."),DriveNO);
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperMMCDriveNumber fails!%d returns!"),err);
    	}
    return err;
    }
// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::UiHelperCancelCollectionOperation
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cmpxmusicplayertest::UiHelperCancelCollectionOperation( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxmusicplayertest::UiHelperCancelCollectionOperation"));
    TInt err = KErrNone;    
    iUiHelper->CancelCollectionOperation();
    if ( err == KErrNone )
    	{
    	iLog->Log(_L("UiHelperCancelCollectionOperation succeeds."));
    	}
    else
    	{
    	iLog->Log(_L("ERROR:UiHelperCancelCollectionOperation fails!%d returns!"),err);
    	}
    return err;
    }

