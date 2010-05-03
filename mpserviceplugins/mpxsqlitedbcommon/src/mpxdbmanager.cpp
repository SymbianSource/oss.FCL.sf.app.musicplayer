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
* Description:  This class is responsible for managing all database access
*                databases.
*
*/


// INCLUDE FILES

#include <sqldb.h>
#include <badesca.h>
#include <f32file.h>
// PREQ2536 the files sqlrowsetutil.h and sqlrowsetutil.cpp has been removed
//#ifdef __WINSCW__    
//#include <sqlrowsetutil.h>
//#endif
#include <sysutil.h>
#ifdef __RAMDISK_PERF_ENABLE
#include <centralrepository.h>
#include <BAUTILS.H>  
#endif //__RAMDISK_PERF_ENABLE

#include <mpxlog.h>

#include "mpxdbcommondef.h"
#include "mpxtable.h"
#include "mpxdbmanager.h"

// CONSTANTS

// Version of   Database
const   TInt KMPXDbVersion[] = {6,0,0};

_LIT8( KMCSqlConfig, "cache_size=1024; page_size=16384; " );

_LIT(KSecureFilePath,   "%S[%x]%S");
_LIT(KRootDrive, "C:");
_LIT(KAliasName, "%1SDrive");
_LIT(KBeginTransaction, "BEGIN TRANSACTION");
_LIT(KCommitTransaction, "COMMIT TRANSACTION");
_LIT(KRollbackTransaction, "ROLLBACK TRANSACTION");
_LIT(KOrderByToken, "ORDER BY");
_LIT(KDBNameToken, ":dbname");
_LIT(KPlDBNameToken, ":pldbname");
_LIT(KUnionAllToken, " UNION ALL ");
_LIT(KSelectToken, "SELECT");

//for database deletion
_LIT( KDBFilePath, "\\private\\10281e17\\" );
_LIT( KDBFilePattern, "*.db*" );    

#ifdef _DEBUG
_LIT(KTableQuery, "SELECT * FROM %S");
_LIT(KAttachedTableQuery, "SELECT * FROM :dbname.%S");
_LIT(KFindAllCDriveTablesQuery, "SELECT name FROM   sqlite_master WHERE type = 'table' ORDER BY name");
_LIT(KFindAllAttachedTablesQuery, "SELECT name FROM :dbname.sqlite_master WHERE type = 'table' ORDER BY name");
_LIT(KNameColumn, "name");
#endif

const TInt KMaxLogQuery = 248;
const TInt KBufIncrement = 10;

#ifdef __RAMDISK_PERF_ENABLE
_LIT(KSecurePath,   "[%x]%S");
_LIT(KRAMAliasName, "%S");
_LIT( KDummyDbFile, "%c:\\private\\10281e17\\dummydb.dat" );
const TInt64 KMPMegaByte = 1048576;
const TInt64 KMPEstimatedSongInBytes = KMPMegaByte * 2; 
const TInt KMPEstimatedSizePerDBEntry = 3000; // worst scenario, can be lower if needed
const TInt KMPMinimumRAMSizeToRun = 6 * KMPMegaByte; 
// if RAM is lower than 5MB, doesn't seem enough for SQL as well.
// so we set this number to move back DBs before being kicked out

// Cenrep key defs -- Only temporary:  Need to find a better place for these
const TUid KMPCenRepSettingsFeature               = { 0x10207C92 };
const TUint32 KMPCenRepSettingRamdiskEnabled      = { 0x00000005 };
const TUint32 KMPCenRepSettingRamdiskMaxDiskSpace = { 0x00000006 };

#endif //__RAMDISK_PERF_ENABLE

// Used to suppress overflow when appending formatted text to a buffer.
class TOverflowHandle :
    public TDesOverflow
    {
    public:
        TOverflowHandle() :
            iFlag(EFalse)
            {
            }

        virtual void Overflow(TDes& /* aDes */)
            {
            iFlag = ETrue;
            return;
            }

        TBool GetOverflowFlag()
            {
            TBool flag(iFlag);
            iFlag = EFalse;
            return flag;
            }
    protected:
        TBool iFlag;
    };

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
EXPORT_C CMPXDbManager::CMPXDbManager(
    RFs& aFs) :
    iFs(aFs), 
    iRAMDiskPerfEnabled(EFalse),
    iMaximumAllowedRAMDiskSpaceToCopy(0),
    iRAMInUse(EFalse)
    {
    MPX_FUNC("CMPXDbManager::CMPXDbManager");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::ConstructL(
    const TFileName& aDatabaseFile)
    {
    MPX_FUNC("CMPXDbManager::ConstructL");
    iDbFile = aDatabaseFile.AllocL();
    
#ifdef __RAMDISK_PERF_ENABLE
    TInt temp;
    CRepository* repository = CRepository::NewLC( KMPCenRepSettingsFeature );
    User::LeaveIfError( repository->Get( KMPCenRepSettingRamdiskEnabled, temp ));
    iRAMDiskPerfEnabled = temp;
    
    User::LeaveIfError( repository->Get( KMPCenRepSettingRamdiskMaxDiskSpace, temp) );
    iMaximumAllowedRAMDiskSpaceToCopy = temp * KMPMegaByte;
    CleanupStack::PopAndDestroy(repository);
            
    if ( iRAMDiskPerfEnabled )
        {
        MPX_DEBUG1("CMPXDbManager::ConstructL RAMDisk performance is enabled.");
        MPX_DEBUG2("CMPXDbManager::ConstructL RAMDisk iMaximumAllowedRAMDiskSpaceToCopy=%Lu", iMaximumAllowedRAMDiskSpaceToCopy);
        if ( GetRAMDiskPath() != KErrNone )
            {
            // Error finding ram drive, disable ram disk
            iRAMDiskPerfEnabled = EFalse;
            }
        }
    else
        {
        MPX_DEBUG2("CMPXDbManager::ConstructL RAMDisk iRAMDiskPerfEnabled=%d", iRAMDiskPerfEnabled);
        MPX_DEBUG2("CMPXDbManager::ConstructL RAMDisk iMaximumAllowedRAMDiskSpaceToCopy=%Lu", iMaximumAllowedRAMDiskSpaceToCopy);
        }
#endif //__RAMDISK_PERF_ENABLE
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
EXPORT_C CMPXDbManager::~CMPXDbManager()
    {
    MPX_FUNC("CMPXDbManager::~CMPXDbManager");

    // Close the state array
    iPreparedStatements.Close();

    // Close and destroy all RSQLStatements
    TInt c( iStatements.Count() );
    for( TInt i=0; i<c; ++i )
        {
        iStatements[i]->Close();
        }
    iStatements.ResetAndDestroy();

    iTables.Close();
    CloseAllDatabases();

    delete iDbFile;
#ifdef __RAMDISK_PERF_ENABLE
    TInt count(iDatabaseHandles.Count());
    for (TInt i = 0; i < count; ++i)
        {
        RemoveDummyFile(i);
        }    
#endif //__RAMDISK_PERF_ENABLE
    iDatabaseHandles.Close();
    }

// ----------------------------------------------------------------------------
// Checks if all databases have been initialized.
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CMPXDbManager::IsInitialized()
    {
    MPX_FUNC("CMPXDbManager::IsInitialized");
    return iInitialized;
    }

// ----------------------------------------------------------------------------
// Begins a transaction on all databases.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::BeginL()
    {
    MPX_FUNC("CMPXDbManager::BeginL");

    ASSERT(iTransactionCount >= 0);

    if (++iTransactionCount == 1)
        {
        DoBeginL();
        }
    }

void CMPXDbManager::DoBeginL()
    {
    MPX_FUNC("CMPXDbManager::DoBeginL");

    TInt err = iDatabase.Exec(KBeginTransaction);
        
    // transforms SQL error to KErrNotReady
    if( (err <= KSqlErrGeneral && err >= KSqlErrNotDb) || err == KSqlErrStmtExpired )
        {
        User::Leave(KErrNotReady);
        }
    else
        {
        User::LeaveIfError(err);
        }
    }

// ----------------------------------------------------------------------------
// Copy all DBs to RAM disk
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::CopyDBsToRamL( TBool aIsMTPInUse )
    {
#ifdef __RAMDISK_PERF_ENABLE
    MPX_DEBUG1("-->CMPXDbManager::CopyDBsToRamL");
    if( iRAMDiskPerfEnabled )
        {
        if ( !IsRamDiskSpaceAvailable() )
            {
            return;
            }
        
        // Check if we are over the allowed ram space.
        TInt dbSize=0;
        TInt err = GetTotalDatabasesSize(dbSize);
        if ( err || (dbSize > iMaximumAllowedRAMDiskSpaceToCopy) )
            {
            MPX_DEBUG2("<--CMPXDbManager::CopyDBsToRamL Over the allowed Ram disk limit %Lu", iMaximumAllowedRAMDiskSpaceToCopy );
            return;
            }
        
        TInt transactionCount = iTransactionCount;
        if (iTransactionCount > 0) 
            {
            iTransactionCount = 0;
            DoCommitL();
            }

        TInt count(iDatabaseHandles.Count());
        TBool ret = EFalse;
        for ( TInt i = 0; i < count ; ++i )
            {
            if ( iDatabaseHandles[i].iUseRAMdb )
                {
                // already used
                MPX_DEBUG1("CMPXDbManager::CopyDBsToRamL iUseRAMdb already ETrue");
                continue;
                }
            iDatabaseHandles[i].iUseRAMdb = ETrue;
            TRAPD(err, ret = DoCopyDBToRamL( iDatabaseHandles[i].iDrive, aIsMTPInUse ))
            if ( err != KErrNone )
                {
                MPX_DEBUG2("CMPXDbManager::CopyDBsToRamL error=%d", err);
                // remove dymmy file
                RemoveDummyFile(i);
                // try to close database that is opened from RAM disk
                TRAP_IGNORE(CloseDatabaseL( iDatabaseHandles[i].iDrive ));
                iDatabaseHandles[i].iUseRAMdb = EFalse;
                // reopen database from drive not from RAM
                OpenDatabaseL( iDatabaseHandles[i].iDrive );
                continue; // continue to copy for next drive
                }
            if ( !ret )
                {
                iDatabaseHandles[i].iUseRAMdb = EFalse;
                }
            }
            
        if (transactionCount > 0) 
            {
            DoBeginL();
            iTransactionCount = transactionCount;
            }
        }
    iRAMInUse = ETrue;
	
    MPX_DEBUG1("<--CMPXDbManager::CopyDBsToRamL");
#endif //__RAMDISK_PERF_ENABLE

    }


// ----------------------------------------------------------------------------
// CMPXDbManager::DoCopyDBsToRamL
// ----------------------------------------------------------------------------
//
TBool CMPXDbManager::DoCopyDBToRamL( TDriveUnit aDrive, TBool aIsMTPInUse )
    {
#ifdef __RAMDISK_PERF_ENABLE
    MPX_DEBUG2("-->CMPXDbManager::DoCopyDBsToRamL drive=%d", (TInt)aDrive);
    TFileName dst;
    TFileName src;
    dst.Append(iRAMFolder);
    src.Append(aDrive.Name());
    src.Append(KDBFilePath);
    TRAPD( err, BaflUtils::EnsurePathExistsL( iFs, dst ));
    if ( err != KErrNone )
        {
        return EFalse;
        }
    TFileName filename;            
    filename.Format(KSecurePath, User::Identity().iUid, iDbFile); //x:\private\10281e17\[sldfdsf]mpxv2_5.db
    src.Append(filename);
    MPX_DEBUG2("RAMDisk src path=%S", &src);
    TEntry entry;
    iFs.Entry( src, entry );
    if (!BlockDiskSpace( aDrive, entry.iSize, aIsMTPInUse ) )
        {
        MPX_DEBUG1("CMPXDbManager::DoCopyDBsToRamL() BlockDiskSpace failed");
        return EFalse; // continue for next drive
        }
    TBuf<2> d;
    d.Append(aDrive.Name());
    HBufC* temp = HBufC::NewLC(KMaxFileName);
    temp->Des().Append(d.Left(1));
    temp->Des().Append(iDbFile->Des());
    filename.Format(KSecurePath, User::Identity().iUid, temp);
    CleanupStack::PopAndDestroy(temp);
    dst.Append(filename);
    MPX_DEBUG2("RAMDisk dst path=%S", &dst);
    TInt index( GetDatabaseIndex((TInt)aDrive) );
    delete iDatabaseHandles[index].iOrigFullFilePath;
    iDatabaseHandles[index].iOrigFullFilePath = 0;
    delete iDatabaseHandles[index].iTargetFullFilePath;
    iDatabaseHandles[index].iTargetFullFilePath = 0;
    
    // Save these path so it is convenient to copy back
    iDatabaseHandles[index].iOrigFullFilePath = HBufC::NewL(src.Length());
    iDatabaseHandles[index].iTargetFullFilePath = HBufC::NewL(dst.Length());
        
    iDatabaseHandles[index].iOrigFullFilePath->Des().Append(src);
    iDatabaseHandles[index].iTargetFullFilePath->Des().Append(dst);

    TRAP(err, CloseDatabaseL(aDrive));
    if ( err != KErrNone )
        {
        MPX_DEBUG2("<--CMPXDbManager::DoCopyDBsToRamL error=%d", err);
        TInt index(GetDatabaseIndex((TInt)aDrive));
        if ( index >= 0 )
            {
            RemoveDummyFile( index );
            }
        return EFalse;
        }
    User::LeaveIfError( BaflUtils::CopyFile(iFs, src, dst ));
    OpenDatabaseL((TInt)aDrive);

    MPX_DEBUG2("RAMDisk Database opened=%d", (TInt)aDrive);
    MPX_DEBUG1("<--CMPXDbManager::DoCopyDBsToRamL");
    return ETrue;    
#endif //__RAMDISK_PERF_ENABLE
    }

// ----------------------------------------------------------------------------
// Copy all DBs from RAM disk back to normal drives
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::CopyDBsFromRamL()
    {
#ifdef __RAMDISK_PERF_ENABLE
    MPX_DEBUG1("-->CMPXDbManager::CopyDBsFromRamL");
    if( iRAMDiskPerfEnabled )
       {
        TInt transactionCount = iTransactionCount;
        if (iTransactionCount > 0) 
            {
            iTransactionCount = 0;
            DoCommitL();
            }

        TInt count(iDatabaseHandles.Count());
        for (TInt i = 0; i < count && iDatabaseHandles[i].iUseRAMdb; ++i)
            {
            TRAPD(err, DoCopyDBFromRamL(i));
            if ( err != KErrNone )
                {
                MPX_DEBUG2("<--CMPXDbManager::CopyDBsFromRamL error=%d", err);                
                //anyting wrong, delete the temp file and open database from drive
                RemoveDummyFile(i);
                // delete Db on RAM
                User::LeaveIfError( BaflUtils::DeleteFile(iFs, 
                    *iDatabaseHandles[i].iTargetFullFilePath));
                }
            iDatabaseHandles[i].iUseRAMdb = EFalse;
            // open db from drive
            OpenDatabaseL( iDatabaseHandles[i].iDrive );      
            }

        if (transactionCount > 0) 
            {
            DoBeginL();
            iTransactionCount = transactionCount;
            }
        }
    iRAMInUse = EFalse;
        
    MPX_DEBUG1("<--CMPXDbManager::CopyDBsFromRamL");
#endif //__RAMDISK_PERF_ENABLE
    }


// ----------------------------------------------------------------------------
// CMPXDbManager::DoCopyDBsToRamL
// ----------------------------------------------------------------------------
//
void CMPXDbManager::DoCopyDBFromRamL( TInt aIndex )
    {
#ifdef __RAMDISK_PERF_ENABLE
    MPX_DEBUG1("-->CMPXDbManager::DoCopyDBsFromRamL");    
    MPX_DEBUG2("-->CMPXDbManager::DoCopyDBsFromRamL Drive %d will be closed before copying db from RAM.",
         iDatabaseHandles[aIndex].iDrive);

    CloseDatabaseL(iDatabaseHandles[aIndex].iDrive);            

    // Delete existing DB on drive
    User::LeaveIfError( BaflUtils::DeleteFile(iFs, 
        *iDatabaseHandles[aIndex].iOrigFullFilePath));
    MPX_DEBUG1("CMPXDbManager::DoCopyDBsFromRamL old DB on drive deleted");

    // Rename dummy file to be orignal file name
    User::LeaveIfError( BaflUtils::RenameFile(iFs, 
        iDatabaseHandles[aIndex].iDummyFilePath, 
        *iDatabaseHandles[aIndex].iOrigFullFilePath) );
    MPX_DEBUG1("CMPXDbManager::CopyDBsFromRamL RAMDisk renamed.");

    //Copy Db from RAM to replace dummy file
    ReplaceFileL( *iDatabaseHandles[aIndex].iTargetFullFilePath, *iDatabaseHandles[aIndex].iOrigFullFilePath);
    
    MPX_DEBUG1("CMPXDbManager::CopyDBsFromRamL RAMDisk copied back.");

    // Delete existing DB on RAM
    User::LeaveIfError( BaflUtils::DeleteFile(iFs, *iDatabaseHandles[aIndex].iTargetFullFilePath));
    MPX_DEBUG1("CMPXDbManager::DoCopyDBsFromRamL RAMDisk deleted");

    MPX_DEBUG1("<--CMPXDbManager::DoCopyDBsFromRamL");
#endif //__RAMDISK_PERF_ENABLE
    } 

// ----------------------------------------------------------------------------
// CMPXDbManager::ReplaceFileL
//
// Replaces a file with another writing over the destination file.
// Leaves on error.
// Implementation follows CFileMan::Copy except that 
//  - we don't resize target file to zero
//  - we can assume that files already exist
//  - we don't copy file attributes & timestamp
// ----------------------------------------------------------------------------
//
void CMPXDbManager::ReplaceFileL( const TDesC& aSrcName, const TDesC& aDstName )
    {
    // open files
    RFile srcFile;
    User::LeaveIfError( srcFile.Open(iFs, aSrcName, EFileRead|EFileShareReadersOnly) );
    CleanupClosePushL( srcFile );
    
    RFile dstFile;
    User::LeaveIfError( dstFile.Open(iFs, aDstName, EFileWrite|EFileWriteDirectIO|EFileShareExclusive) );
    CleanupClosePushL( dstFile );
    
    // resize destination file
    TInt remainingBytes;
    User::LeaveIfError( srcFile.Size(remainingBytes) );
    User::LeaveIfError( dstFile.SetSize(remainingBytes) );

    // allocate buffer
   	const TInt KBigBufSize = 512 * 1024;
    const TInt KMediumBufSize = 32 * 1024;
    const TInt KSmallBufSize = 4 * 1024;
    HBufC8* bufPtr=HBufC8::New( Min(KBigBufSize, remainingBytes) );
    if (bufPtr==NULL)
        bufPtr=HBufC8::New(KMediumBufSize);
    if (bufPtr==NULL)
        bufPtr=HBufC8::New(KSmallBufSize);
    if (bufPtr == NULL)
        User::Leave(KErrNoMemory);
    CleanupStack::PushL(bufPtr);

    // copy
    TPtr8 copyBuf=bufPtr->Des();
    TInt pos=0;
    while( remainingBytes > 0 )
        {
        TInt s = Min( remainingBytes, copyBuf.MaxSize() );
        TInt ret = srcFile.Read(pos, copyBuf, s);
        if (ret == KErrNone && copyBuf.Length()!= s )
            {
            ret = KErrCorrupt;
            }
        if (ret == KErrNone)
            {
            ret = dstFile.Write(pos, copyBuf, s);
            }
        User::LeaveIfError (ret);
        pos += s;
        remainingBytes -= s;
        }
    User::LeaveIfError( dstFile.Flush() );
    CleanupStack::PopAndDestroy(3); // bufPtr, dstFile, srcFile
    }

// ----------------------------------------------------------------------------
// CMPXDbManager::RemoveDummyFile
// ----------------------------------------------------------------------------
//
void CMPXDbManager::RemoveDummyFile( TInt index )
    {
#ifdef __RAMDISK_PERF_ENABLE
    MPX_DEBUG1("-->CMPXDbManager::RemoveDummyFile");
       
    if ( iDatabaseHandles[index].iDummyFilePath.Length() )
        {
        TInt err = BaflUtils::DeleteFile(iFs, iDatabaseHandles[index].iDummyFilePath);
        if ( !err )
            {
            iDatabaseHandles[index].iDummyFilePath.Zero();
            }
        }
    MPX_DEBUG1("<--CMPXDbManager::RemoveDummyFile");
#endif //__RAMDISK_PERF_ENABLE

    }


// ----------------------------------------------------------------------------
// Commits a transaction on all databases.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::CommitL()
    {
    MPX_FUNC("CMPXDbManager::CommitL");

    if(iTransactionCount > 0)
        {
        if (--iTransactionCount == 0)
            {
            DoCommitL();
            }
        }
    }
    
// ----------------------------------------------------------------------------
// Commits a transaction on all databases.
// ----------------------------------------------------------------------------
//
void CMPXDbManager::DoCommitL()
    {
    MPX_FUNC("CMPXDbManager::DoCommitL");
    TInt err = iDatabase.Exec(KCommitTransaction);
    
    // transforms SQL error to KErrNotReady
    if( (err <= KSqlErrGeneral && err >= KSqlErrNotDb) || err == KSqlErrStmtExpired )
        {
        MPX_DEBUG2("CMPXDbManager::CommitL failed err=%d", err);
        User::Leave(KErrNotReady);
        }
    else
        {
        User::LeaveIfError(err);
        }
    }

// ----------------------------------------------------------------------------
// Rolls back a transaction on all databases.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::RollbackL()
    {
    MPX_FUNC("CMPXDbManager::RollbackL");

    if(iTransactionCount > 0)
        {
        if (--iTransactionCount == 0)
            {
            TInt err = iDatabase.Exec(KRollbackTransaction);
            
            // transforms SQL error to KErrNotReady
            if( (err <= KSqlErrGeneral && err >= KSqlErrNotDb) || err == KSqlErrStmtExpired )
                {
                User::Leave(KErrNotReady);  
                }
            else
                {
                User::LeaveIfError(err);
                }
            }
    	}
    }

// ----------------------------------------------------------------------------
// Rolls back a transaction on all databases.
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CMPXDbManager::InTransaction()
    {
    MPX_FUNC("CMPXDbManager::InTransaction");
    return iDatabase.InTransaction();
    }

// ----------------------------------------------------------------------------
// Tries to create and open the databases on all specified drives.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::InitDatabasesL(
    RArray<TInt> aDrives)
    {
    MPX_FUNC("CMPXDbManager::InitDatabasesL");

    CloseAllDatabases();

    TDriveUnit cdrive(KRootDrive());

    CreateDatabaseL(cdrive);
    OpenDatabaseL(cdrive);

    TInt count(aDrives.Count());
    for (TInt i = 0; i < count; ++i)
        {
        TDriveUnit drive(aDrives[i]);
        if ((drive != cdrive) && !IsRemoteDrive(static_cast<TDriveNumber>(aDrives[i])))
            {
            const TDesC& driveName = drive.Name();

            DatabaseHandle handle;

            handle.iDrive = aDrives[i];
            handle.iAliasname = HBufC::NewL(KAliasName().Length());
            handle.iAliasname->Des().Format(KAliasName, &driveName);
            handle.iOpen = EFalse;
#ifdef __RAMDISK_PERF_ENABLE
            handle.iOrigFullFilePath = HBufC::NewL(0);
            handle.iTargetFullFilePath = HBufC::NewL(0);
            handle.iDummyFilePath.Zero();
            handle.iUseRAMdb = EFalse;
#endif //__RAMDISK_PERF_ENABLE

            iDatabaseHandles.AppendL(handle);

            TVolumeInfo vol;
            if (iFs.Volume(vol, drive) == KErrNone)
                {
                CreateDatabaseL(drive);
                AttachDatabaseL(drive);
                }
            }
        }

    iInitialized = ETrue;
    }

// ----------------------------------------------------------------------------
// Opens a specified database.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::OpenDatabaseL(
    TInt aDrive)
    {
    MPX_FUNC("CMPXDbManager::OpenDatabaseL");

    if (iInitialized == EFalse)
        {
        User::Leave(KErrNotReady);
        }

    TDriveUnit drive(aDrive);
    TDriveUnit cdrive(KRootDrive());
    TBool found(EFalse);

    if ((drive != cdrive) && !IsRemoteDrive(static_cast<TDriveNumber>(aDrive)))
        {
        TInt count(iDatabaseHandles.Count());
        for (TInt i = 0; i < count; ++i)
            {
            if (iDatabaseHandles[i].iDrive == aDrive)
                {
                MPX_DEBUG2("CMPXDbManager::OpenDatabaseL found %d", aDrive);
                if (!iDatabaseHandles[i].iOpen)
                    {
                    MPX_DEBUG1("CMPXDbManager::OpenDatabaseL not open found");
                    // make sure the database is created
					CreateDatabaseL(drive);

					// attach
                    AttachDatabaseL(drive);
                    }

                found = ETrue;
                break;
                }
            }
        }
    if (!found)
        {
        MPX_DEBUG1("CMPXDbManager::OpenDatabaseL not found");
        User::Leave(KErrArgument);
        }

    // Close all prepared statements if a db is opened
    //
    ResetPreparedQueries();
    }

// ----------------------------------------------------------------------------
// Closes a specified database.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::CloseDatabaseL(
    TInt aDrive)
    {
    MPX_FUNC("CMPXDbManager::CloseDatabaseL");

    if (iInitialized == EFalse)
        {
        User::Leave(KErrNotReady);
        }

    // Close all prepared statements if a db is closed
    //
    ResetPreparedQueries();

    TDriveUnit drive(aDrive);
    TDriveUnit cdrive(KRootDrive());
    TBool found(EFalse);

    if ((drive != cdrive) && !IsRemoteDrive(static_cast<TDriveNumber>(aDrive)))
        {
        TInt count(iDatabaseHandles.Count());
        for (TInt i = 0; i < count; ++i)
            {
            if (iDatabaseHandles[i].iDrive == aDrive)
                {
                MPX_DEBUG2("CMPXDbManager::CloseDatabaseL found %d", aDrive);
                if (iDatabaseHandles[i].iOpen)
                    {
                    MPX_DEBUG1("CMPXDbManager::CloseDatabaseL found open");
                    DetachDatabaseL(drive);
                    }

                found = ETrue;
                break;
                }
            }
        }
    if (!found)
        {
        MPX_DEBUG1("CMPXDbManager::CloseDatabaseL not found");
        User::Leave(KErrArgument);
        }

    }

// ----------------------------------------------------------------------------
// Closes all databases.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::CloseAllDatabases()
    {
    MPX_FUNC("CMPXDbManager::CloseAllDatabases");

    if (iInitialized)
        {
        // Close all prepared statements if a db is closed
        //
        ResetPreparedQueries();

        TInt count(iDatabaseHandles.Count());
        for (TInt i = 0; i < count; ++i)
            {
            delete iDatabaseHandles[i].iAliasname;
#ifdef __RAMDISK_PERF_ENABLE 
            delete iDatabaseHandles[i].iOrigFullFilePath;
            delete iDatabaseHandles[i].iTargetFullFilePath;
            iDatabaseHandles[i].iDummyFilePath.Zero();
#endif //__RAMDISK_PERF_ENABLE 
            }

        iDatabaseHandles.Reset();
        iDatabase.Close();
        iInitialized = EFalse;
        }
    }

// ----------------------------------------------------------------------------
// Open all databases.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::OpenAllDatabasesL()
    {
    MPX_FUNC("CMPXDbManager::OpenAllDatabasesL");

    if (!iInitialized)
        {
        TDriveUnit cdrive(KRootDrive());
        OpenDatabaseL(cdrive);
        }

    TInt count(iDatabaseHandles.Count());
    for (TInt i = 0; i < count; ++i)
        {
        TVolumeInfo vol;
        if (iFs.Volume(vol, iDatabaseHandles[i].iDrive) == KErrNone)
            {
            AttachDatabaseL(iDatabaseHandles[i].iDrive);
            }
        }
    iInitialized = ETrue;

    // Close all prepared statements if a db is closed
    //
    ResetPreparedQueries();
    }

// ----------------------------------------------------------------------------
// Checks if the database on a specified drive is open.
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CMPXDbManager::IsOpen(
    TInt aDrive) const
    {
    MPX_FUNC("CMPXDbManager::IsOpen");

    TDriveUnit drive(aDrive);
    TDriveUnit cdrive(KRootDrive());

    if (!iInitialized)
        {
        return EFalse;
        }
    else if (drive == cdrive)
        {
        return ETrue;
        }
    else
        {
        TInt count(iDatabaseHandles.Count());
        for (TInt i = 0; i < count; ++i)
            {
            if (iDatabaseHandles[i].iDrive == aDrive)
                {
                return iDatabaseHandles[i].iOpen;
                }
            }
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// Returns the number of currently open databases.
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXDbManager::DatabaseCount() const
    {
    MPX_FUNC("CMPXDbManager::DatabaseCount");

    TInt openCount(0);
    if (iInitialized)
        {
        ++openCount;

        TInt count(iDatabaseHandles.Count());
        for (TInt i = 0; i < count; ++i)
            {
            if (iDatabaseHandles[i].iOpen)
                {
                ++openCount;
                }
            }
        }

    return openCount;
    }

// ----------------------------------------------------------------------------
// Returns the drive corresponding to a given index.
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXDbManager::DbDrive(
    TInt aIndex) const
    {
    MPX_FUNC("CMPXDbManager::DbDrive");

    ASSERT((aIndex >= 0) || (aIndex < iDatabaseHandles.Count()));
    return iDatabaseHandles[aIndex].iDrive;
    }

// ----------------------------------------------------------------------------
// Recreate a specified database.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::RecreateDatabaseL(
    TInt aDrive)
    {
    MPX_FUNC("CMPXDbManager::RecreateDatabaseL");

    if (iInitialized == EFalse)
        {
        User::Leave(KErrNotReady);
        }

    TBool found(EFalse);

    if (aDrive == EDriveC)
        {
        found = ETrue;
        }
    else
        {
        TInt count(iDatabaseHandles.Count());
        for (TInt i = 0; i < count; ++i)
            {
            if ((iDatabaseHandles[i].iDrive == aDrive) && (iDatabaseHandles[i].iOpen))
                {
                found = ETrue;
                break;
                }
            }
        }
    if (found)
        {
        HBufC * filename = CreateFilenameL(aDrive);
        CleanupStack::PushL(filename);

        TRAPD(err, DoRecreateDatabaseL(filename));
        if(err < 0)
            {
            TDriveUnit drive_unit(aDrive);

            if(aDrive == EDriveC)
                {
                iDatabase.Close();
                iInitialized = EFalse;

                RSqlDatabase::Delete(*filename);
                CreateDatabaseL(drive_unit);

                User::LeaveIfError(iDatabase.Open(*filename));
                iInitialized = ETrue;
                }
            else
                {
                DetachDatabaseL(drive_unit);

                RSqlDatabase::Delete(*filename);
                CreateDatabaseL(drive_unit);

                AttachDatabaseL(drive_unit);
                }
            }

        CleanupStack::PopAndDestroy(filename);
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    }

// ----------------------------------------------------------------------------
// Recreate all databases.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::RecreateAllDatabasesL()
    {
    MPX_FUNC("CMPXDbManager::RecreateAllDatabasesL");

    if (iInitialized == EFalse)
        {
        User::Leave(KErrNotReady);
        }

    // Recreate on drive C
    RecreateDatabaseL(EDriveC);

    // Recreate all attached drives
    TInt count(iDatabaseHandles.Count());
    for (TInt i = 0; i < count; ++i)
        {
        if (iDatabaseHandles[i].iOpen)
            {
            RecreateDatabaseL(iDatabaseHandles[i].iDrive);
            }
        }
    }

// ----------------------------------------------------------------------------
// Returns current DB version
// ----------------------------------------------------------------------------
//
EXPORT_C TVersion CMPXDbManager::Version() const
    {
    MPX_FUNC("CMPXDbManager::Version");
    return TVersion(KMPXDbVersion[0], KMPXDbVersion[1], KMPXDbVersion[2]);
    }

// ----------------------------------------------------------------------------
// Registes a table with the database
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::RegisterTableL(
    MMPXTable& aTable)
    {
    MPX_FUNC("CMPXDbManager::RegisterTableL");
    iTables.AppendL(&aTable);
    }

// ----------------------------------------------------------------------------
// Executes a select query with variable number of parameters
// The query is executed on all available databases with a format like:
//
//      <query on database1> UNION ALL <query on database2> ...
//
// The query string passed in by the caller must have the ":dbname" prefix for all
// the tables in the FROM clause. This will be replaced with the right alias for
// attached databases or with no alias for the C database.
//
// In case the original query contains an ORDER BY clause, this will be extracted
// and added at the end of the union query.
//
// Note: Running the union query seems to be similar in speed even if one of the
// databases is empty and therefore no optimization was done for this case.
// ----------------------------------------------------------------------------
//
EXPORT_C RSqlStatement CMPXDbManager::ExecuteSelectQueryL(
    TRefByValue<const TDesC> aFmt,
    ...)
    {
    MPX_FUNC("CMPXDatabase::ExecuteSelectQueryL");

    VA_LIST list;
    VA_START(list, aFmt);

    // Will reallocate
    HBufC* selectBuf = FormatQueryLC(aFmt, list);
    RSqlStatement statement = ExecuteSelectQueryOnAllDrivesL(selectBuf->Des());
    CleanupStack::PopAndDestroy(selectBuf);

    VA_END(list);

    return statement;
    }

// ----------------------------------------------------------------------------
// Executes a select query against a specified drive
// ----------------------------------------------------------------------------
//
EXPORT_C RSqlStatement CMPXDbManager::ExecuteSelectQueryL(
    TInt aDrive,
    TRefByValue<const TDesC> aFmt,
    ...)
    {
    MPX_FUNC("CMPXDatabase::ExecuteSelectQueryL");

    VA_LIST list;
    VA_START(list, aFmt);

    // Will reallocate
    HBufC* selectBuf = FormatQueryLC(aFmt, list);
    RSqlStatement statement = ExecuteSelectQueryOnDriveL(aDrive, selectBuf->Des());
    CleanupStack::PopAndDestroy(selectBuf);

    VA_END(list);

    return statement;
    }

// ----------------------------------------------------------------------------
// CMPXDbManager::ExecuteSelectQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C RSqlStatement& CMPXDbManager::ExecuteSelectQueryL(
    TUint aStatementId,
    TInt aFirstValue,
    TInt aSecondValue,
    TRefByValue<const TDesC> aFmt,
    ...)
    {
    MPX_FUNC("CMPXDatabase::ExecuteOffsetSelectQueryL");

    // Prepare the Query first
    VA_LIST list;
    VA_START(list, aFmt);
    RSqlStatement& statement = PrepareQueryL( aStatementId, aFmt, list );
    VA_END(list);

    // Bind the Limit and Offset variables
    User::LeaveIfError(statement.BindInt(0, aFirstValue));
    User::LeaveIfError(statement.BindInt(1, aSecondValue));

    return statement;
    }

// ----------------------------------------------------------------------------
// CMPXDbManager::ExecuteSelectQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C RSqlStatement& CMPXDbManager::ExecuteSelectQueryL( TUint aStatementId,
                                                            const TDesC& aFirstValue,
                                                            TInt aSecondValue,
                                                            TRefByValue<const TDesC> aFmt, ...)
    {
    MPX_FUNC("CMPXDbManager::ExecuteMediaAscQueryL");

    // Prepare the Query first
    VA_LIST list;
    VA_START(list, aFmt);
    RSqlStatement& statement = PrepareQueryL( aStatementId, aFmt, list );
    VA_END(list);

    // bind the title and limit values
    User::LeaveIfError(statement.BindText(0, aFirstValue));
    User::LeaveIfError(statement.BindInt(1, aSecondValue));

    return statement;
    }

// ----------------------------------------------------------------------------
// Executes a query that does not return a record set (INSERT, UPDATE, DELETE,
// CREATE, DROP, etc).
//
// If a valid drive is specified then the query is only executed only on
// that drive. If KDbManagerAllDrives is specified then the query is executed
// separately on each available drive.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::ExecuteQueryL(
    TInt aDrive,
    TRefByValue<const TDesC> aFmt,
    ...)
    {
    MPX_FUNC("CMPXDatabase::ExecuteQueryL");

    // make sure there is enough space on all drives affected
    CheckDiskSpaceL(aDrive);

    VA_LIST list;
    VA_START(list, aFmt);

    HBufC* selectBuf = FormatQueryLC(aFmt, list);
    TPtr selectBufPtr = selectBuf->Des();
    TInt dbCount(iDatabaseHandles.Count());

    // a specified drive or all drives
    TInt loopCount = (aDrive == KDbManagerAllDrives) ? (dbCount + 1) : 1;
    TBool queryExecuted(EFalse); // flag to check if the query was executed at least once
    for (TInt j = 0; j < loopCount; ++j)
        {
        HBufC* query = HBufC::NewLC(selectBufPtr.Length() + KBufIncrement);
        TPtr queryPtr = query->Des();
        queryPtr.Copy(selectBufPtr);
        if (aDrive == EDriveC) // if C drive only
            {
            RemoveDriveAlias(queryPtr);
            }
        else // all drives or a particular drive other than C drive
            {
            if (aDrive == 0) // all drives
                {
                if (j == dbCount) // C drive
                    {
                    RemoveDriveAlias(queryPtr);
                    }
                else //all other drives, except C drive
                    {
                    if (iDatabaseHandles[j].iOpen)
                        {
                        ReplaceDriveAlias(queryPtr, *(iDatabaseHandles[j].iAliasname));
                        }
                    }
                }
            else //a particular drive, other than C drive
                {
                for (TInt i = 0; i < dbCount; ++i)
                    {
                    if (iDatabaseHandles[i].iOpen && iDatabaseHandles[i].iDrive == aDrive)
                        {
                        ReplaceDriveAlias(queryPtr, *(iDatabaseHandles[i].iAliasname));
                        break;
                        }
                    }
                }
            }
        TInt dbnamePos = queryPtr.Find(KDBNameToken);// check if the query was created correctly
        if (dbnamePos == KErrNotFound)
            {
            // log the query
            TPtrC ptr(query->Left(KMaxLogQuery));
            MPX_DEBUG2("Query: %S", &ptr);

            User::LeaveIfError(ExecuteSqlStatement(iDatabase, queryPtr));
            queryExecuted = ETrue;
            }
        CleanupStack::PopAndDestroy(query);
        }   //for (TInt j = 0; j < loopCount; ++j)
    CleanupStack::PopAndDestroy(selectBuf);
    VA_END(list);
    if (!queryExecuted && aDrive !=   0)
        {
        // the requested drive(s) is not open
        User::Leave(KErrNotFound);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbManager::FormatQueryLC
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbManager::FormatQueryLC(
    TRefByValue<const TDesC> aFmt,
    VA_LIST aList)
    {
    MPX_FUNC("CMPXDatabase::FormatQueryLC");

    TOverflowHandle overflow;

    HBufC* selectBuf = HBufC::NewLC(TDesC(aFmt).Length());//will reallocate
    selectBuf->Des().AppendFormatList(aFmt, aList, &overflow);
    while (overflow.GetOverflowFlag())
        {
        TInt len = selectBuf->Des().MaxLength() + KBufIncrement;
        CleanupStack::PopAndDestroy(selectBuf);
        selectBuf = HBufC::NewLC(len);
        selectBuf->Des().AppendFormatList(aFmt, aList, &overflow);
        }

    return selectBuf;
    }

// ----------------------------------------------------------------------------
// Executes a select query against a specified drive
// ----------------------------------------------------------------------------
//
EXPORT_C RSqlStatement CMPXDbManager::ExecuteSelectQueryOnAllDrivesL(
    TInt aDrive,
    TRefByValue<const TDesC> aFmt,
    ...)
    {
    MPX_FUNC("CMPXDatabase::ExecuteSelectQueryL");

    VA_LIST list;
    VA_START(list, aFmt);

    // Will reallocate
    HBufC* selectBuf = FormatQueryLC(aFmt, list);
    RSqlStatement statement = ExecuteSelectQueryOnAllDrivesL(aDrive, selectBuf->Des());
    CleanupStack::PopAndDestroy(selectBuf);

    VA_END(list);

    return statement;
    }

// ----------------------------------------------------------------------------
// CMPXDbManager::ExecuteSelectQueryOnAllDrivesL
// ----------------------------------------------------------------------------
//
RSqlStatement CMPXDbManager::ExecuteSelectQueryOnAllDrivesL(
    TPtr aQuery)
    {
    MPX_FUNC("CMPXDatabase::ExecuteSelectQueryOnAllDrivesL");

    TInt dbCount = iDatabaseHandles.Count();
    HBufC* query = HBufC::NewLC(aQuery.Length() * (dbCount + 1) +
        KUnionAllToken().Length() * dbCount);
    TPtr queryPtr = query->Des();
    HBufC* selectOutBuf = NULL;
    TInt enclosed = aQuery.Mid(1, aQuery.Length() - 1).Find(KSelectToken);
    if (enclosed != KErrNotFound)
        {
        enclosed++;//to compensate the indent
        selectOutBuf = HBufC::NewLC(aQuery.Length() * (dbCount + 1) +
            KUnionAllToken().Length() * dbCount);
        selectOutBuf->Des().Copy(aQuery.Left(enclosed));
        selectOutBuf->Des().Append(aQuery.Right(1));//the closing bracket
        aQuery.Delete(0, enclosed);
        aQuery.Delete(aQuery.Length() - 1, 1);
        }

    HBufC* orderBuf = NULL;
    TInt orderPos = aQuery.Find(KOrderByToken);
    if (orderPos != KErrNotFound)
        {
        orderBuf = aQuery.Right(aQuery.Length() - orderPos).AllocL();
        aQuery.Delete(orderPos, aQuery.Length() - orderPos);
        }
    queryPtr.Append(aQuery);// for cdrive
    RemoveDriveAlias(queryPtr);
    for (TInt i = 0; i < dbCount; ++i)//for other drives
        {
        if (iDatabaseHandles[i].iOpen)
            {
            queryPtr.Append(KUnionAllToken);
            queryPtr.Append(aQuery);
            ReplaceDriveAlias(queryPtr, *(iDatabaseHandles[i].iAliasname));
            }
        }
    if (orderBuf)
        {
        queryPtr.Append(orderBuf->Des());
        }
    delete orderBuf;
    if (enclosed != KErrNotFound)
        {
        selectOutBuf->Des().Insert(enclosed, query->Des());
        queryPtr.Copy(selectOutBuf->Des());
        CleanupStack::PopAndDestroy(selectOutBuf);
        }

    // Log the query string before execution
    TPtrC ptr(query->Left(KMaxLogQuery));
    MPX_DEBUG2("Query: %S", &ptr);

    // Return a temporary statement and not a member variable.
    // This ensures that a copy is done and a second embedded query can be
    // executed while the first result set is processed.
    RSqlStatement statement;
    User::LeaveIfError(statement.Prepare(iDatabase, queryPtr));
    CleanupStack::PopAndDestroy(query);

    return statement;
    }

// ----------------------------------------------------------------------------
// CMPXDbManager::ExecuteSelectQueryOnAllDrivesL
// ----------------------------------------------------------------------------
//
RSqlStatement CMPXDbManager::ExecuteSelectQueryOnAllDrivesL( TInt aDrive,
    TPtr aQuery)
    {
    MPX_FUNC("CMPXDatabase::ExecuteSelectQueryOnAllDrivesL");

    TInt dbCount = iDatabaseHandles.Count();
    HBufC* query = HBufC::NewLC(aQuery.Length() * (dbCount + 1) +
        KUnionAllToken().Length() * dbCount);
    TPtr queryPtr = query->Des();
    HBufC* selectOutBuf = NULL;
    TInt enclosed = aQuery.Mid(1, aQuery.Length() - 1).Find(KSelectToken);
    if (enclosed != KErrNotFound)
        {
        enclosed++;//to compensate the indent
        selectOutBuf = HBufC::NewLC(aQuery.Length() * (dbCount + 1) +
            KUnionAllToken().Length() * dbCount);
        selectOutBuf->Des().Copy(aQuery.Left(enclosed));
        selectOutBuf->Des().Append(aQuery.Right(1));//the closing bracket
        aQuery.Delete(0, enclosed);
        aQuery.Delete(aQuery.Length() - 1, 1);
        }

    HBufC* orderBuf = NULL;
    TInt orderPos = aQuery.Find(KOrderByToken);
    if (orderPos != KErrNotFound)
        {
        orderBuf = aQuery.Right(aQuery.Length() - orderPos).AllocL();
        aQuery.Delete(orderPos, aQuery.Length() - orderPos);
        }
    
    //remove KPlDBNameToken
    if ( aDrive == EDriveC )//if playlist on c drive
    	{
    	RemoveDriveAlias(aQuery,KPlDBNameToken);
    	}
    else
    	{//for other drives
	    for (TInt i = 0; i < dbCount; ++i)
	        {
	        if (iDatabaseHandles[i].iOpen && (iDatabaseHandles[i].iDrive == aDrive))
	            {
	            ReplaceDriveAlias(aQuery, *(iDatabaseHandles[i].iAliasname),
	            		KPlDBNameToken);
	            break;
	            }
	        }
    	}
    
    queryPtr.Append(aQuery);// for cdrive
    RemoveDriveAlias(queryPtr);
    for (TInt i = 0; i < dbCount; ++i)//for other drives
        {
        if (iDatabaseHandles[i].iOpen)
            {
            queryPtr.Append(KUnionAllToken);
            queryPtr.Append(aQuery);
            ReplaceDriveAlias(queryPtr, *(iDatabaseHandles[i].iAliasname));
            }
        }
   
    if (orderBuf)
        {
        queryPtr.Append(orderBuf->Des());
        }
    delete orderBuf;
    if (enclosed != KErrNotFound)
        {
        selectOutBuf->Des().Insert(enclosed, query->Des());
        queryPtr.Copy(selectOutBuf->Des());
        CleanupStack::PopAndDestroy(selectOutBuf);
        }

    // Log the query string before execution
    TPtrC ptr(query->Left(KMaxLogQuery));
    MPX_DEBUG2("Query: %S", &ptr);

    // Return a temporary statement and not a member variable.
    // This ensures that a copy is done and a second embedded query can be
    // executed while the first result set is processed.
    RSqlStatement statement;
    TInt err(statement.Prepare(iDatabase, queryPtr));
    User::LeaveIfError(err);
    CleanupStack::PopAndDestroy(query);

    return statement;
    }

// ----------------------------------------------------------------------------
// CMPXDbManager::ExecuteSelectQueryOnDriveLryLC
// ----------------------------------------------------------------------------
//
RSqlStatement CMPXDbManager::ExecuteSelectQueryOnDriveL(
    TInt aDrive,
    TPtr aQuery)
    {
    MPX_FUNC("CMPXDatabase::ExecuteSelectQueryOnDriveL");

    RSqlStatement statement;
    if (KDbManagerAllDrives == aDrive)
        {
        statement = ExecuteSelectQueryOnAllDrivesL(aQuery);
        }
    else
        {
        TInt dbCount(iDatabaseHandles.Count());

        // flag to check if the query was executed at least once
        TBool queryExecuted = EFalse;

        HBufC* query = HBufC::NewLC(aQuery.Length() + KBufIncrement);
        TPtr queryPtr = query->Des();
        queryPtr.Copy(aQuery);
        if (aDrive == EDriveC) //if C drive
            {
            RemoveDriveAlias(queryPtr);
            }
        else // drive other than C drive
            {
            for (TInt i = 0; i < dbCount; ++i)
                {
                if (iDatabaseHandles[i].iOpen && (iDatabaseHandles[i].iDrive == aDrive))
                    {
                    ReplaceDriveAlias(queryPtr, *(iDatabaseHandles[i].iAliasname));
                    break;
                    }
                }
            }

        TInt dbnamePos = queryPtr.Find(KDBNameToken);// check if the query was created correctly
        if (dbnamePos == KErrNotFound)
            {
            // Log the query string before execution
            TPtrC ptr(query->Left(KMaxLogQuery));
            MPX_DEBUG2("Query: %S", &ptr);

            User::LeaveIfError(statement.Prepare(iDatabase, queryPtr));
            queryExecuted = ETrue;
            }
        CleanupStack::PopAndDestroy(query);

        if (!queryExecuted)
            {
            // the requested drive(s) is not open
            User::Leave(KErrNotFound);
            }
        }

    return statement;
    }

// ----------------------------------------------------------------------------
// Prepare a query for execution on all open database. This query's lifetime
// is owned by the dbmanager
// ----------------------------------------------------------------------------
//
RSqlStatement& CMPXDbManager::PrepareQueryL( TUint aStatementId,
                                             TRefByValue<const TDesC> aFmt,
                                             VA_LIST aList )
    {
    // Try to find the query first if it has been created
    TInt index(KErrNotFound);
    TInt c(iPreparedStatements.Count());

    for( TInt i=0; i<c; ++i )
        {
        if( iPreparedStatements[i].iId == aStatementId )
            {
            index = i;
            break;
            }
        }

    // If the index isn't found we create a new query statement
    //
    if( index == KErrNotFound )
        {
        RSqlStatement* newStatement = new(ELeave) RSqlStatement();
        CleanupStack::PushL(newStatement);

        TSqlStatementState newState;
        newState.iId = aStatementId;
        newState.iPrepared = EFalse;
        iPreparedStatements.AppendL( newState );

        TInt err = iStatements.Append( newStatement ); // ownership x-fer
        if (err != KErrNone)
            {
            iPreparedStatements.Remove(c);
            User::Leave(err);
            }
        CleanupStack::Pop(newStatement);
        index = c;
        }

    // Finally create the statement
    if ( !iPreparedStatements[index].iPrepared )
        {

        // Will reallocate
        HBufC* selectBuf = FormatQueryLC(aFmt, aList);
        TPtr selectBufPtr = selectBuf->Des();
        TInt dbCount = iDatabaseHandles.Count();
        HBufC* query = HBufC::NewLC(selectBufPtr.Length() * (dbCount + 1) +
            KUnionAllToken().Length() * dbCount);
        TPtr queryPtr = query->Des();
        HBufC* selectOutBuf = NULL;
        TInt enclosed = selectBufPtr.Mid(1,selectBufPtr.Length() - 1).Find(KSelectToken);
        if (enclosed != KErrNotFound)
            {
            enclosed++;//to compensate the indent
            selectOutBuf = HBufC::NewLC(selectBufPtr.Length() * (dbCount + 1) +
                KUnionAllToken().Length() * dbCount);
            selectOutBuf->Des().Copy(selectBufPtr.Left(enclosed));
            selectOutBuf->Des().Append(selectBufPtr.Right(1));//the closing bracket
            selectBufPtr.Delete(0, enclosed);
            selectBufPtr.Delete(selectBufPtr.Length()   -   1, 1);
            }

        HBufC* orderBuf = NULL;
        TInt orderPos = selectBufPtr.Find(KOrderByToken);
        if (orderPos != KErrNotFound)
            {
            orderBuf = selectBufPtr.Right(selectBufPtr.Length() - orderPos).AllocL();
            selectBufPtr.Delete(orderPos,   selectBufPtr.Length() - orderPos);
            }
        queryPtr.Append(selectBufPtr);// for cdrive
        RemoveDriveAlias(queryPtr);
        for (TInt i = 0; i < dbCount; ++i)//for other drives
            {
            if (iDatabaseHandles[i].iOpen)
                {
                queryPtr.Append(KUnionAllToken);
                queryPtr.Append(selectBufPtr);
                ReplaceDriveAlias(queryPtr, *(iDatabaseHandles[i].iAliasname));
                }
            }
        if (orderBuf)
            {
            queryPtr.Append(orderBuf->Des());
            }
        delete orderBuf;
        if (enclosed != KErrNotFound)
            {
            selectOutBuf->Des().Insert(enclosed, query->Des());
            queryPtr.Copy(selectOutBuf->Des());
            CleanupStack::PopAndDestroy(selectOutBuf);
            }

        // Log the query string before execution
        TPtrC ptr(query->Left(KMaxLogQuery));
        MPX_DEBUG2("Query: %S", &ptr);

        // use the member variable statement
        User::LeaveIfError(iStatements[index]->Prepare(iDatabase, queryPtr));
        CleanupStack::PopAndDestroy(2, selectBuf); //query

        iPreparedStatements[index].iPrepared = ETrue;
        }
    else
        {
        iStatements[index]->Reset();
        }

    return *iStatements[index];
    }

// ----------------------------------------------------------------------------
// Resets all prepared queries
// ----------------------------------------------------------------------------
//
void CMPXDbManager::ResetPreparedQueries()
    {
    iPreparedStatements.Reset();

    TInt c( iStatements.Count() );
    for( TInt i=0; i<c; ++i )
        {
        iStatements[i]->Close();
        }
    iStatements.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// Asks all registered tables to create themselves
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::CreateTablesL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbManager::CreateTablesL");

	CreateTablesL(aDatabase, EFalse);
    }

// ----------------------------------------------------------------------------
// CMPXDbManager::CreateTablesL
// ----------------------------------------------------------------------------
//
void CMPXDbManager::CreateTablesL(
	RSqlDatabase& aDatabase,
	TBool aCorrupt)
	{
    TInt count(iTables.Count());
    for (TInt i = 0; i < count; ++i)
        {
        iTables[i]->CreateTableL(aDatabase, aCorrupt);
        }
	}

// ----------------------------------------------------------------------------
// Opens a specified database.
// ----------------------------------------------------------------------------
//
void CMPXDbManager::OpenDatabaseL(
    TDriveUnit aDrive)
    {
    MPX_FUNC("CMPXDbManager::OpenDatabaseL");

    HBufC * filename = CreateFilenameL(aDrive);
    CleanupStack::PushL(filename);
    User::LeaveIfError(iDatabase.Open(filename->Des()));

    CleanupStack::PopAndDestroy(filename);
    }

// ----------------------------------------------------------------------------
// Creates a specified database.
// ----------------------------------------------------------------------------
//
void CMPXDbManager::CreateDatabaseL(
    TDriveUnit aDrive)
    {
    MPX_FUNC("CMPXDbManager::CreateDatabaseL");

    RSqlDatabase database;
    CleanupClosePushL(database);

    HBufC* filename = CreateFilenameL(aDrive);
    CleanupStack::PushL(filename);

    if (database.Open(filename->Des()) != KErrNone)
        {
        MPX_DEBUG3("CMPXDbManager::CreateDatabaseL - cannot open db on drive %d %S", TInt(aDrive), filename);

        // close the database first
        database.Close();
        DoCreateDatabaseL( aDrive );
        }
    else
        {
        TBool tableOK(ETrue);

        // try to detect any corrupt tables
        TInt count(iTables.Count());
        for (TInt i = 0; i < count; ++i)
            {
            // ask the table to check its structure
            if (!iTables[i]->CheckTableL(database))
                {
                tableOK = EFalse;
                break;
                }
            }

        if (!tableOK)
            {
            // close the database first
            database.Close();
			
			// delete database and create database
            DoCreateDatabaseL( aDrive );
            }
        }
    CleanupStack::PopAndDestroy(filename);
    CleanupStack::PopAndDestroy(&database);
    }

// ----------------------------------------------------------------------------
// Attaches a specified database.
// ----------------------------------------------------------------------------
//
void CMPXDbManager::AttachDatabaseL(
    TDriveUnit aDrive)
    {
    MPX_FUNC("CMPXDbManager::AttachDatabaseL");

    TBool found(EFalse);

    TInt count(iDatabaseHandles.Count());
    for (TInt i = 0; i < count; ++i)
        {
        if (iDatabaseHandles[i].iDrive == aDrive)
            {
            if (!iDatabaseHandles[i].iOpen)
                {
                HBufC* filename = CreateFilenameL(aDrive);
                CleanupStack::PushL(filename);
                
#ifdef __RAMDISK_PERF_ENABLE
                if( iDatabaseHandles[i].iUseRAMdb )
                    {
                    delete iDatabaseHandles[i].iAliasname;
                    iDatabaseHandles[i].iAliasname = HBufC::NewL(KAliasName().Length());
                    HBufC* temp = HBufC::NewLC(2); // form of DE, DF, DX,...
                    temp->Des().Append(iRAMDrive); // length == 2
                    TDriveUnit pdrive(aDrive);
                    temp->Des().Append(pdrive.Name().Left(1)); //length == 2+ 1
                    iDatabaseHandles[i].iAliasname->Des().Format(KRAMAliasName, temp);
                    MPX_DEBUG2("CMPXDbManager::AttachDatabaseL - RAM change aliasname of %S", iDatabaseHandles[i].iAliasname );
                    CleanupStack::PopAndDestroy(temp);
                    }
                else
#endif //__RAMDISK_PERF_ENABLE
                   {
                   delete iDatabaseHandles[i].iAliasname;
                   TDriveUnit drive(aDrive);
                   const TDesC& driveName = drive.Name();
                   iDatabaseHandles[i].iAliasname = HBufC::NewL(KAliasName().Length());
                   iDatabaseHandles[i].iAliasname->Des().Format(KAliasName, &driveName);
                   MPX_DEBUG2("CMPXDbManager::AttachDatabaseL - normal change aliasname of %S", iDatabaseHandles[i].iAliasname);
                   }

                TInt err = iDatabase.Attach(filename->Des(), *(iDatabaseHandles[i].iAliasname));
                MPX_DEBUG2("CMPXDbManager::AttachDatabaseL - Attach Error =%d", err);
                User::LeaveIfError(err);
                iDatabaseHandles[i].iOpen = ETrue;

                CleanupStack::PopAndDestroy(filename);
                }
            else
                {
                MPX_DEBUG1("CMPXDbManager::AttachDatabaseL - found already open");    
                }
            
            found = ETrue;
            break;
            }
        }
    if (!found)
        {
        MPX_DEBUG1("CMPXDbManager::AttachDatabaseL - not found");
        User::Leave(KErrNotFound);
        }
    }

// ----------------------------------------------------------------------------
// Detaches a specified database.
// ----------------------------------------------------------------------------
//
void CMPXDbManager::DetachDatabaseL(
    TDriveUnit aDrive)
    {
    MPX_FUNC("CMPXDbManager::DetachDatabaseL");

    ASSERT(iInitialized);
    TBool found(EFalse);

    TInt count(iDatabaseHandles.Count());
    for (TInt i = 0; i < count; ++i)
        {
        if (iDatabaseHandles[i].iDrive == aDrive)
            {
#ifdef __RAMDISK_PERF_ENABLE
            if ( iDatabaseHandles[i].iOpen || iDatabaseHandles[i].iUseRAMdb )
#else //__RAMDISK_PERF_ENABLE
            if ( iDatabaseHandles[i].iOpen )
#endif //__RAMDISK_PERF_ENABLE

                {
                MPX_DEBUG1("CMPXDbManager::DetachDatabaseL found drive that is opening");
                TInt err = iDatabase.Detach(*(iDatabaseHandles[i].iAliasname));
                if ( err )
                    {
                    MPX_DEBUG2("CMPXDbManager::DetachDatabaseL detach failed Error=%d", err);
                    }
                else
                    {
                    MPX_DEBUG2("CMPXDbManager::DetachDatabaseL iAliasname=%S", iDatabaseHandles[i].iAliasname);
                    }
                User::LeaveIfError(err);
                iDatabaseHandles[i].iOpen = EFalse;
                }

            found = ETrue;
            break;
            }
        }
    if (!found)
        {
        MPX_DEBUG1("CMPXDbManager::DetachDatabaseL drive not found in iDatabaseHandlers");
        User::Leave(KErrNotFound);
        }
    }

// ----------------------------------------------------------------------------
// Creates the absolute database filename on a specified drive.
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbManager::CreateFilenameL(
    TDriveUnit aDrive)
    {
    MPX_FUNC("CMPXDbManager::CreateFilenameL");

    HBufC* filename = HBufC::NewL(KMaxFileName);

    const TDesC& securefilePath = KSecureFilePath;
    TDriveUnit cdrive(KRootDrive());

#ifdef __RAMDISK_PERF_ENABLE
    TInt index(GetDatabaseIndex((TInt)aDrive));    
    if ( index >=0 && iDatabaseHandles[index].iUseRAMdb && aDrive != cdrive )
        {
        MPX_DEBUG1("CMPXDbManager::CreateFilenameL - use RAMDisk");
        TFileName path;
        path.Append(iRAMDrive);
        path.Append(_L(":"));
        TBuf<2> d;
        d.Append(aDrive.Name());
        HBufC* temp = HBufC::NewLC(KMaxFileName);
        temp->Des().Append(d.Left(1)); // attach original drive name
        temp->Des().Append(iDbFile->Des()); 
        filename->Des().Format(securefilePath, &path, User::Identity().iUid, temp);
        MPX_DEBUG3("CMPXDbManager::CreateFilenameL - path=%S filename=%S", &path, filename);
        CleanupStack::PopAndDestroy(temp);
        }
    else
#endif //__RAMDISK_PERF_ENABLE
        {
        MPX_DEBUG1("CMPXDbManager::CreateFilenameL - use normal drive");
        const TDesC& driveName = aDrive.Name();
        filename->Des().Format(securefilePath, &driveName, User::Identity().iUid, iDbFile);

        }
    
    MPX_DEBUG2("CMPXDbManager::CreateFilenameL filename = %S", filename); 
    return filename;
    }

// ----------------------------------------------------------------------------
// Replaces :dbname with a drive alias
// ----------------------------------------------------------------------------
//
void CMPXDbManager::ReplaceDriveAlias(
    TDes& aQuery,
    const TDesC& aAlias)
    {
//  MPX_FUNC("CMPXDbManager::ReplaceDriveAlias");

    TInt dbnamePos(aQuery.Find(KDBNameToken));
    while (dbnamePos != KErrNotFound)
        {
        aQuery.Delete(dbnamePos, KDBNameToken().Length());
        aQuery.Insert(dbnamePos, aAlias);
        dbnamePos = aQuery.Find(KDBNameToken);
        }
    }

// ----------------------------------------------------------------------------
// Replaces :dbname with a drive alias
// ----------------------------------------------------------------------------
//
void CMPXDbManager::ReplaceDriveAlias(
    TDes& aQuery,
    const TDesC& aAlias,
    const TDesC& aToKen)
    {
    
    TInt dbnamePos(aQuery.Find(aToKen));
    while (dbnamePos != KErrNotFound)
        {
        aQuery.Delete(dbnamePos, aToKen.Length());
        aQuery.Insert(dbnamePos, aAlias);
        dbnamePos = aQuery.Find(aToKen);
        }
    }

// ----------------------------------------------------------------------------
// Removes :dbname
// ----------------------------------------------------------------------------
//
void CMPXDbManager::RemoveDriveAlias(
    TDes& aQuery)
    {
    MPX_FUNC("CMPXDbManager::RemoveDriveAlias");

    TInt dbnamePos(aQuery.Find(KDBNameToken));
    while (dbnamePos != KErrNotFound)
        {
        aQuery.Delete(dbnamePos, KDBNameToken().Length() + 1);
        dbnamePos = aQuery.Find(KDBNameToken);
        }
    }


// ----------------------------------------------------------------------------
// Removes :dbname
// ----------------------------------------------------------------------------
//
void CMPXDbManager::RemoveDriveAlias(
    TDes& aQuery,const TDesC& aToKen)
    {
    MPX_FUNC("CMPXDbManager::RemoveDriveAlias");

    TInt dbnamePos(aQuery.Find(aToKen));
    while (dbnamePos != KErrNotFound)
        {
        aQuery.Delete(dbnamePos, aToKen.Length() + 1);
        dbnamePos = aQuery.Find(aToKen);
        }
    }


// ----------------------------------------------------------------------------
// CMPXDbManager::CheckDiskSpaceL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::CheckDiskSpaceL(
    TInt aDrive)
    {
    MPX_FUNC("CMPXDbManager::CheckDiskSpaceL");
    
    // LTAN-7GH6BZ, crash if eject memory card when adding song to existing playlist
    // due to special timing issue, it is possible drive number is -1 and create a
    // panic when use for TDriveUnit
    MPX_DEBUG2("aDrive = %d", aDrive);
    
    if (aDrive < 0)
        {
        MPX_DEBUG1("invalid driveId, leave with KErrNotReady");
        User::Leave(KErrNotReady);
        }
    
#ifndef __RAMDISK_PERF_ENABLE 
    
    EnsureDiskSpaceL(aDrive);
    
#endif //__RAMDISK_PERF_ENABLE

    }
    
// ----------------------------------------------------------------------------
// Regenerate all databases.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::RegenerateAllDatabasesL()
    {
    MPX_DEBUG1("CMPXDbManager::RegenerateAllDatabasesL Enter");
    ResetPreparedQueries(); //just in case ...
    TInt handles(iDatabaseHandles.Count());
    for (TInt i = 0; i < handles; ++i)
        {
        iDatabaseHandles[i].iOpen = EFalse; //attach will open them again
        }    
    iDatabase.Close(); //close the database before deleting the file
    iInitialized = EFalse;

    MPX_DEBUG1("RegenerateAllDatabasesL: Regenerating main DB on C:");
    HBufC * filename = CreateFilenameL(EDriveC);
    CleanupStack::PushL(filename);
    RSqlDatabase::Delete(*filename);
    TDriveUnit cdrive(KRootDrive());
    CreateDatabaseL(cdrive);
    User::LeaveIfError(iDatabase.Open(*filename)); // will set handle status later
    CleanupStack::PopAndDestroy(filename);
    MPX_DEBUG1("RegenerateAllDatabasesL: DB regeneration complete");
    
    // Recreate all attached drives
    TInt count(iDatabaseHandles.Count());
    for (TInt i = 0; i < count; ++i)
        {
        if (iDatabaseHandles[i].iDrive != EDriveC)
            {
            MPX_DEBUG2("RegenerateAllDatabasesL: Regenerating DB on %d",iDatabaseHandles[i].iDrive);
            TVolumeInfo volumeInfo; 
            TInt err = iFs.Volume(volumeInfo,iDatabaseHandles[i].iDrive);
            if(err != KErrNone)
                {
                continue; //if drive is not currently accessible, skip
                }
            filename = CreateFilenameL(iDatabaseHandles[i].iDrive);
            CleanupStack::PushL(filename);
            MPX_DEBUG1("RegenerateAllDatabasesL: Detaching DB");
            err = iDatabase.Detach(*(iDatabaseHandles[i].iAliasname)); //ignore the error if any
            MPX_DEBUG2("RegenerateAllDatabasesL: Detached[err=%d]; Deleting DB",err);
            err = RSqlDatabase::Delete(*filename);
            MPX_DEBUG2("RegenerateAllDatabasesL: Deleted[err=%d]; Creating new DB",err);
            TDriveUnit drive(iDatabaseHandles[i].iDrive);
            CreateDatabaseL(drive);
            MPX_DEBUG1("RegenerateAllDatabasesL: Attaching new DB");
            AttachDatabaseL(drive);    
            MPX_DEBUG1("RegenerateAllDatabasesL: DB regeneration complete");
            CleanupStack::PopAndDestroy(filename);
            }
        else
            {
            iDatabaseHandles[i].iOpen = ETrue; //if we got here it is opened
            }
        }
    iInitialized = ETrue;
    MPX_DEBUG1("CMPXDbManager::RegenerateAllDatabasesL Exit");
    }    

// ----------------------------------------------------------------------------
// CMPXDbManager::DoRecreateDatabaseL
// ----------------------------------------------------------------------------
//
void CMPXDbManager::DoRecreateDatabaseL(HBufC * aFilename)
    {
    RSqlDatabase database;
    CleanupClosePushL(database);

    User::LeaveIfError(database.Open(aFilename->Des()));

    TInt count(iTables.Count());
    for (TInt i = 0; i < count; ++i)
        {
        iTables[i]->DropTableL(database);
        iTables[i]->CreateTableL(database, EFalse);
        }
    CleanupStack::PopAndDestroy(&database);
    }

// ----------------------------------------------------------------------------
// CMPXDbManager::ExecuteSqlStatement
// ----------------------------------------------------------------------------
//
TInt CMPXDbManager::ExecuteSqlStatement(RSqlDatabase& aDatabase,const TDesC& aStatement)
    {
    MPX_FUNC("CMPXDbManager::ExecuteSqlStatement");
    TInt result( KErrNone );
    RSqlStatement sqlStatement;
    //Prepare and execute SQL statement
    result = sqlStatement.Prepare(aDatabase, aStatement);
    if (result == KErrNone)
        {
        result = sqlStatement.Exec();
        //If the database schema was changed or the session expired repeat all the steps
        if((result == KSqlErrStmtExpired) || (result == KSqlErrSchema))
            {
            sqlStatement.Close();
            result = sqlStatement.Prepare(aDatabase, aStatement);
            if (result == KErrNone)
                {
                result = sqlStatement.Exec();
                }
            }
        sqlStatement.Close();
        }
    return result;
    }

#ifdef _DEBUG

// ----------------------------------------------------------------------------
// Returns the number of columns from a specified SQL statement
// ----------------------------------------------------------------------------
//
TInt CMPXDbManager::GetColumnCountL(
    RSqlStatement& aStatement)
    {
    TInt columnCount(0);

// Using TSqlRowSetUtil causes linker errors on ARMv5 UDEB
// Enabling this functionality for WINSCW UDEB only
// PREQ2536 the files sqlrowsetutil.h and sqlrowsetutil.cpp has been removed
//#ifdef __WINSCW__
//
//    HBufC* headers = TSqlRowSetUtil::GetDeclColumnTypesL(aStatement);
//    CleanupStack::PushL(headers);
//
//    // Count the number of semicolons to get the number of columns
//    TPtr headerPtr = headers->Des();
//    TInt location(headerPtr.Locate(';'));
//    while ((location != KErrNotFound) && (location < headers->Length()))
//        {
//        ++columnCount;
//        if (++location < headers->Length())
//            {
//            headerPtr = headers->Des().Mid(location);
//            location = headerPtr.Locate(';');
//            }
//        }
//    CleanupStack::PopAndDestroy(headers);
//
//#else
	(void)aStatement;
//#endif

    return columnCount;
    }

// ----------------------------------------------------------------------------
// Prints the table values from a specified SQL query
// ----------------------------------------------------------------------------
//
void CMPXDbManager::PrintTableValuesL(
    RSqlStatement& aStatement)
    {
    TInt columnCount(GetColumnCountL(aStatement));
    TInt err(KErrNone);
    HBufC* tableRow = HBufC::NewLC(255 * columnCount);
    TPtr tableRowPtr = tableRow->Des();

    while ((err = aStatement.Next()) == KSqlAtRow)
        {
        tableRowPtr.Zero();
        TInt error(KErrNone);
        for (TInt index = 0; (error == KErrNone) && (index < columnCount); ++index)
            {
            if (index !=0)
                {
                tableRowPtr.Append(',');
                }
            switch (aStatement.ColumnType(index))
                {
                case ESqlNull:
                    tableRowPtr.Append(_L("<NULL>"));
                    break;

                case ESqlInt:
                    {
                    tableRowPtr.AppendFormat(_L("%u"), aStatement.ColumnInt(index));
                    }
                    break;

                case ESqlInt64:
                    {
                    tableRowPtr.AppendFormat(_L("%lu"), aStatement.ColumnInt64(index));
                    }
                    break;

                case ESqlReal:
                    {
                    tableRowPtr.AppendFormat(_L("%f"), aStatement.ColumnReal(index));
                    }
                    break;

                case ESqlText:
                    {
                    TPtrC columnValue;
                    error = aStatement.ColumnText(index, columnValue);
                    if (error == KErrNone)
                        {
                        tableRowPtr.AppendFormat(_L("%S"), &columnValue);
                        }
                    }
                    break;

                case ESqlBinary:
                    {
                    TPtrC8 columnValue;
                    error = aStatement.ColumnBinary(index, columnValue);
                    if (error == KErrNone)
                        {
                        tableRowPtr.AppendFormat(_L("%S"), &columnValue);
                        }
                    }
                    break;

                default :
                    ASSERT(EFalse);
                }

            if (tableRowPtr.Length() > 255)
                {
                tableRowPtr.SetLength(255);
                MPX_DEBUG2("%S", tableRow);
                tableRowPtr.Zero();
                }
            }
        if (tableRowPtr.Length() > 0)
            {
            tableRowPtr.SetLength(Min(tableRowPtr.Length(), 255));
            MPX_DEBUG2("%S", tableRow);
            }
        }
    CleanupStack::PopAndDestroy(tableRow);
    if (err != KSqlAtEnd)
        {
        User::Leave(err);
        }
    }

// ----------------------------------------------------------------------------
// Finds all the tables on the main or attached drives
// ----------------------------------------------------------------------------
//
void CMPXDbManager::FindAllTablesL(
    const TDesC& aAlias,
    RArray<HBufC*>& aTableName)
    {
    RSqlStatement statement;
    CleanupClosePushL(statement);

    if (aAlias == KNullDesC)
        {
        statement.Prepare(iDatabase, KFindAllCDriveTablesQuery);
        }
    else
        {
        HBufC* query = KFindAllAttachedTablesQuery().AllocL();
        CleanupStack::PushL(query);
        TPtr queryPtr = query->Des();
        ReplaceDriveAlias(queryPtr, aAlias);
        statement.Prepare(iDatabase, queryPtr);
        CleanupStack::PopAndDestroy(query);
        }

    TInt err(KErrNone);

    while ((err = statement.Next()) == KSqlAtRow)
        {
        TPtrC val = statement.ColumnTextL(statement.ColumnIndex(KNameColumn));
        aTableName.AppendL(val.AllocL());
        }
    if (err != KSqlAtEnd)
        {
        User::Leave(err);
        }

    CleanupStack::PopAndDestroy(&statement);
    }

// ----------------------------------------------------------------------------
// Prints the tables on the main or attached drives
// ----------------------------------------------------------------------------
//
void CMPXDbManager::PrintTableL(
    const TDesC& aAlias,
    const TDesC& aTableName)
    {
    RSqlStatement statement;
    CleanupClosePushL(statement);

    if (aAlias == KNullDesC)
        {
        HBufC* selectQuery = HBufC::NewLC(KTableQuery().Length() + aTableName.Length());
        selectQuery->Des().Format(KTableQuery, &aTableName);
        User::LeaveIfError(statement.Prepare(iDatabase, *selectQuery));
        CleanupStack::PopAndDestroy(selectQuery);
        }
    else
        {
        HBufC* selectQuery = HBufC::NewLC(KAttachedTableQuery().Length() + aTableName.Length());
        selectQuery->Des().Format(KAttachedTableQuery, &aTableName);

        TPtr selectQueryPtr(selectQuery->Des());
        ReplaceDriveAlias(selectQueryPtr, aAlias);
        User::LeaveIfError(statement.Prepare(iDatabase, *selectQuery));

        CleanupStack::PopAndDestroy(selectQuery);
        }

    PrintTableValuesL(statement);
    CleanupStack::PopAndDestroy(&statement);
    }

#endif

// ----------------------------------------------------------------------------
// Prints all the tables on the main and attached drives
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::PrintDatabaseL()
    {
#ifdef _DEBUG
    if (iInitialized)
        {
        // C-Drive
        RArray<HBufC*> tableNames;
        FindAllTablesL(KNullDesC(), tableNames);

        MPX_DEBUG1("### Drive C ###");

        TInt tableCount(tableNames.Count());
        for (TInt i = 0; i < tableCount; ++i)
            {
            MPX_DEBUG2("# %S #", tableNames[i]);
            MPX_TRAPD(error, PrintTableL(KNullDesC, *tableNames[i]));
            delete tableNames[i];
            if (error != KErrNone)
                {
                if (error != KErrPermissionDenied)
                    {
                    User::Leave(error);
                    }
                else
                    {
                    MPX_DEBUG1("Unable to print table");
                    }
                }
            }
        tableNames.Close();

        // Each attached drive
        TInt count(iDatabaseHandles.Count());
        for (TInt i = 0; i < count; ++i)
            {
            if (iDatabaseHandles[i].iOpen)
                {
                FindAllTablesL(iDatabaseHandles[i].iAliasname->Des(), tableNames);

                TDriveUnit driveUnit(iDatabaseHandles[i].iDrive);
                const   TDesC& name = driveUnit.Name();
                MPX_DEBUG2("### Drive %S ###", &name);

                for (TInt j = 0; j < tableCount; ++j)
                    {
                    MPX_DEBUG2("# %S #", tableNames[j]);
                    MPX_TRAPD(error, PrintTableL(iDatabaseHandles[i].iAliasname->Des(), *tableNames[j]));
                    delete tableNames[j];
                    if (error != KErrNone)
                        {
                        if (error != KErrPermissionDenied)
                            {
                            User::Leave(error);
                            }
                        else
                            {
                            MPX_DEBUG1("Unable to print table");
                            }
                        }
                    }
                tableNames.Close();
                }
            }
        }
#endif
    }

// ----------------------------------------------------------------------------
// Prints all the tables on the main and attached drives
// ----------------------------------------------------------------------------
//
EXPORT_C RFs& CMPXDbManager::Fs()
    {
    return iFs;
    }



// ---------------------------------------------------------------------------
// CMPXDbManager::IsRemoteDrive
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CMPXDbManager::IsRemoteDrive(TDriveNumber aDrive)
    {
    TDriveInfo driveInfo;
    TBool isRemoteDrive(EFalse);
    if (iFs.Drive(driveInfo, aDrive) == KErrNone)
        {
        isRemoteDrive = driveInfo.iDriveAtt & KDriveAttRemote;
        }
    return isRemoteDrive;
    }

// ---------------------------------------------------------------------------
// CMPXDbManager::DoCreateDatabaseL
// ---------------------------------------------------------------------------
//
void CMPXDbManager::DoCreateDatabaseL( TDriveUnit aDrive )
    {
	MPX_FUNC( "CMPXDbManager::DoCreateDatabaseL" );
	
    RSqlDatabase database;
    CleanupClosePushL(database);

    HBufC* filename = CreateFilenameL(aDrive);
    CleanupStack::PushL(filename);
    
    // remove old databases before creating/replacing new database
    TInt driveNameLen = aDrive.Name().Length();
    
    TFileName dbFileName;

#ifdef __RAMDISK_PERF_ENABLE
    TInt index(GetDatabaseIndex((TInt)aDrive));
    if( index >= 0 && iDatabaseHandles[index].iUseRAMdb )
        {
        dbFileName.Append(iRAMDrive); // RAM
        dbFileName.Append(_L(":")); // RAM
        }
    else 
#endif //__RAMDISK_PERF_ENABLE
        {
        MPX_DEBUG1("CMPXDbManager::CreateDatabaseL - E:");
        dbFileName.Append(aDrive.Name()); //initialise with drive name
        }
        

    MPX_DEBUG2("CMPXDbManager::CreateDatabaseL - dbFileName=%S", &dbFileName);
    dbFileName.Append(KDBFilePath);  // append private path
    
    //append file name
    dbFileName.Append(filename->Right((filename->Length())- driveNameLen));     
    
    // locate the offset position where version info starts in file name
    TInt pos = dbFileName.LocateReverse('v');

    //replace version info with wildcards 
    dbFileName.Replace(pos, (dbFileName.Length()- pos), KDBFilePattern);

    CFileMan* fileManager = CFileMan::NewL(iFs);
    TInt ret = fileManager->Delete(dbFileName);
    delete fileManager;
    fileManager = NULL;
    
    // create the database now
    RSqlSecurityPolicy securityPolicy;
    CleanupClosePushL(securityPolicy);

    TSecurityPolicy policy(TSecurityPolicy::EAlwaysPass);
    securityPolicy.Create(policy);

    TSecurityPolicy schemaPolicy(TSecurityPolicy::EAlwaysPass);
    TSecurityPolicy readPolicy(TSecurityPolicy::EAlwaysPass);
    TSecurityPolicy writePolicy(TSecurityPolicy::EAlwaysPass);

    User::LeaveIfError(securityPolicy.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy, schemaPolicy));
    User::LeaveIfError(securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EReadPolicy, readPolicy));
    User::LeaveIfError(securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, writePolicy));

    
    const TDesC8& config = KMCSqlConfig;
    
    TBool corrupt(EFalse);
    TInt err = database.Create(filename->Des(), securityPolicy, &config);
    if (KErrAlreadyExists == err)
        {
        MPX_DEBUG1("CMPXDbManager::DoCreateDatabaseL - could not create the database");

        // the file already exists and it is corrupted
        // make sure we delete the file
        User::LeaveIfError(database.Delete(*filename));

        MPX_DEBUG1("CMPXDbManager::DoCreateDatabaseL - deleted the database");

        // try again
        err = database.Create(filename->Des(), securityPolicy, &config);

        // the database could not be opened but the file exists
        corrupt = ETrue;
        }
    User::LeaveIfError(err);

    MPX_DEBUG1("CMPXDbManager::DoCreateDatabaseL - created the database");

    CleanupStack::PopAndDestroy(&securityPolicy);

    CreateTablesL(database, corrupt);

    CleanupStack::PopAndDestroy(filename);
    CleanupStack::PopAndDestroy(&database);
    }
	
// ---------------------------------------------------------------------------
// CMPXDbManager::GetRAMDiskPath
// ---------------------------------------------------------------------------
//
TInt CMPXDbManager::GetRAMDiskPath()
    {
    TInt error = KErrNotSupported;
#ifdef __RAMDISK_PERF_ENABLE
    MPX_DEBUG1("-->CMPXDbManager::GetRAMDiskPath");
    TDriveList driveList;
    TBool ramDriveFound = EFalse;
    TInt driveOffset = 'A';
    iRAMFolder.Zero();
    
    error = iFs.DriveList( driveList );
    if ( error == KErrNone )
        {
        for ( TInt i = 0; i < driveList.Length(); i++ )
            {
            if ( driveList[i] != 0 )
                {
                TDriveInfo info;
                TInt err = iFs.Drive( info, i );
                if ( !err && info.iType == EMediaRam )
                    {
                    iRAMDrive = driveOffset + i;
                    iRAMFolder.Append(iRAMDrive);
                    iRAMFolder.Append(_L(":"));
                    iRAMFolder.Append(KDBFilePath);
                    ramDriveFound = ETrue;
                    MPX_DEBUG2("RAMDisk path=%S", &iRAMFolder);
                    break;
                    }
                }
            }
        
        // Check if ram drive is found.
        if ( !ramDriveFound )
            {
            error = KErrNotFound;
            }
        }
    MPX_DEBUG2("CMPXDbManager::GetRAMDiskPath Get DriveList error=%d", error);
    MPX_DEBUG1("<--CMPXDbManager::GetRAMDiskPath");
#endif //__RAMDISK_PERF_ENABLE
    return error;
    }

// ---------------------------------------------------------------------------
// CMPXDbManager::IsRamDiskSpaceAvailable
// ---------------------------------------------------------------------------
//
TBool CMPXDbManager::IsRamDiskSpaceAvailable()
    {

#ifdef __RAMDISK_PERF_ENABLE

    MPX_DEBUG1("-->CMPXDbManager::IsDiskSpaceAvailable" );
    TInt driveIndex;
    RFs::CharToDrive(iRAMDrive, driveIndex);
    TVolumeInfo vol;
    TInt err = iFs.Volume( vol, driveIndex );
    if ( err == KErrNone )
        {
        MPX_DEBUG2("CMPXDbManager::IsRamDiskSpaceAvailable Free in bytes =%Lu", vol.iFree);
        if ( vol.iFree > KMPMinimumRAMSizeToRun )
            {
            MPX_DEBUG1("CMPXDbManager::IsRamDiskSpaceAvailable Ok to copy");
            return ETrue;
            }
        MPX_DEBUG1("CMPXDbManager::IsRamDiskSpaceAvailable NOT Ok to copy");
        return EFalse;
        }
    
    MPX_DEBUG2("CMPXDbManager::IsRamDiskSpaceAvailable Disk Not available to use. %d", err);
    MPX_DEBUG1("<--CMPXDbManager::IsDiskSpaceAvailable");

#endif //__RAMDISK_PERF_ENABLE

    return EFalse;
    }


// ---------------------------------------------------------------------------
// CMPXDbManager::BlockDiskSpaceL
// ---------------------------------------------------------------------------
//
TBool CMPXDbManager::BlockDiskSpace( TDriveUnit aDrive, TInt aOrigDbSize, TBool aIsMTPInUse )
    {
#ifdef __RAMDISK_PERF_ENABLE

    MPX_DEBUG2("-->CMPXDbManager::BlockDiskSpaceL %d", (TInt)aDrive );
    
    // if current DB size can not fit in RAM, abort now
    TInt ramDrive;
    RFs::CharToDrive(iRAMDrive, ramDrive);
    TVolumeInfo vol;
    TInt err = iFs.Volume( vol, ramDrive );
    if ( vol.iFree <= aOrigDbSize + KMPMinimumRAMSizeToRun )
        {
        MPX_DEBUG1("-->CMPXDbManager::BlockDiskSpaceL Not enough even for copy original DB file, leave" );
        return EFalse;
        }

    // ensure you have the disk volume and database
    err = iFs.Volume( vol, (TInt)aDrive );
    if (err != KErrNone) 
        {
        MPX_DEBUG2("CMPXDbManager::BlockDiskSpaceL Volume not available on drive %d", (TInt)aDrive);
        return EFalse;
        }

    TInt index( GetDatabaseIndex((TInt)aDrive) );
    if (index < 0) 
        {
        MPX_DEBUG2("CMPXDbManager::BlockDiskSpaceL Database not available for drive %d", (TInt)aDrive);
        return EFalse;
        }

    // Check if the drive has enough space to block
    MPX_DEBUG2("CMPXDbManager::BlockDiskSpaceL Disk total free space in bytes =%Lu", vol.iFree);
    TInt64 blockingSize( CalculateInitalDummyDBSize( vol, aOrigDbSize, aIsMTPInUse ));
    MPX_DEBUG2("CMPXDbManager::BlockDiskSpaceL Disk blocking size =%Lu", blockingSize);
    if ( vol.iFree <= blockingSize + 1*KMPMegaByte )
        {
        MPX_DEBUG1("CMPXDbManager::BlockDiskSpaceL NOk to block");
        return EFalse;
        }

    // Create and resize the dummy file
    TFileName dummyDbFileName; 
    TChar ch;
    RFs::DriveToChar((TInt)aDrive, ch );
    dummyDbFileName.Format( KDummyDbFile, (TUint)ch);
    RFile dummyDb;
    err = dummyDb.Replace( iFs, dummyDbFileName, EFileWrite );
    if (err != KErrNone) 
        {
        MPX_DEBUG2("CMPXDbManager::BlockDiskSpaceL Can't open dummy file %d", err);
        return EFalse;
        }
    err = dummyDb.SetSize( blockingSize );
    if ( err )
        {
        MPX_DEBUG2("CMPXDbManager::BlockDiskSpaceL Can't resize dummy file %d", err);
        dummyDb.Close();
        return EFalse;
        }

    dummyDb.Close();
    MPX_DEBUG1("CMPXDbManager::BlockDiskSpaceL Ok to block");

    iDatabaseHandles[index].iDummyFilePath.Copy(dummyDbFileName);
    MPX_DEBUG1("<--CMPXDbManager::BlockDiskSpace");

    return ETrue;
#else
    return EFalse;
#endif //__RAMDISK_PERF_ENABLE
    }


// ---------------------------------------------------------------------------
// CMPXDbManager::CalculateInitalDummyDBSizeL
//
//a) MTP case
//-------------
//        totalNumOfSongsCanFit = <disk free space> / 2 MB;
//        metadataSize = totalNumOfSongsCanFit * 3000B 
//        estimatedDBSize = metadataSize + <orig DB size>;
//        dummyDBSize = MIN (iMaximumAllowedRAMDiskSpaceToCopy , estimatedDBSize )

//b) Harvesting case
//-------------------
//        totalNumOfSongsCanFit = <disk total size>/ 2 MB
//        metadataSize = totalNumOfSongsCanFit * 3000B 
//        estimatedSize = metadataSize+ <orig DB size>
//        freeDiskSpace = <disk free space> - 1 MB
//        dummyDBSize = MIN (freeDiskSpace, iMaximumAllowedRAMDiskSpaceToCopy , estimatedSize);
//
// ---------------------------------------------------------------------------
//
TInt64 CMPXDbManager::CalculateInitalDummyDBSize( const TVolumeInfo& aVol, TInt aOrigDbSize, TBool aIsMTPInUse )
    {
#ifdef __RAMDISK_PERF_ENABLE
    MPX_DEBUG1("-->CMPXDbManager::CalculateInitalDummyDBSize");

    if ( aIsMTPInUse )
        {
        TInt64 totalNumOfSongsCanFit = aVol.iFree / KMPEstimatedSongInBytes;
        MPX_DEBUG2("-->CMPXDbManager::CalculateInitalDummyDBSize aVol.iFree=%Lu", aVol.iFree );
        MPX_DEBUG2("-->CMPXDbManager::CalculateInitalDummyDBSize totalNumOfSongsCanFit=%Lu", totalNumOfSongsCanFit );
        TInt64 estimatedSize = totalNumOfSongsCanFit * (TInt64) KMPEstimatedSizePerDBEntry + aOrigDbSize;
        MPX_DEBUG2("-->CMPXDbManager::CalculateInitalDummyDBSize (MTP case) estimated DB size from calculation=%Lu", estimatedSize );
        if ( estimatedSize > iMaximumAllowedRAMDiskSpaceToCopy )
            {
            MPX_DEBUG2("<--CMPXDbManager::CalculateInitalDummyDBSize returned iMaximumAllowedRAMDiskSpaceToCopy %d", iMaximumAllowedRAMDiskSpaceToCopy);
            return iMaximumAllowedRAMDiskSpaceToCopy;
            }
        else
            {
            MPX_DEBUG2("<--CMPXDbManager::CalculateInitalDummyDBSize returned %Lu", estimatedSize );
            return estimatedSize;
            }
        }
    else
        {
        TInt64 totalNumOfSongsCanFit = aVol.iSize / KMPEstimatedSongInBytes;
        TInt64 estimatedSize = totalNumOfSongsCanFit * (TInt64) KMPEstimatedSizePerDBEntry + aOrigDbSize;
        MPX_DEBUG2("-->CMPXDbManager::CalculateInitalDummyDBSize estimated DB size from calculation=%Lu", estimatedSize );
        if ( estimatedSize > iMaximumAllowedRAMDiskSpaceToCopy )
            {
            MPX_DEBUG1("<--CMPXDbManager::CalculateInitalDummyDBSize");
            // If estimated size is larger than expected free RAM size, 
            // and if the RAM size is larger than free disk space,
            // then use free disk space. 1*KMPMegaByte prevent MP to use up all diskspace
            //return iMaximumAllowedRAMDiskSpaceToCopy > aVol.iFree - 1*KMPMegaByte  
            //    ? aVol.iFree - 1*KMPMegaByte : iMaximumAllowedRAMDiskSpaceToCopy;
            return iMaximumAllowedRAMDiskSpaceToCopy;
            }
        else
            {
            MPX_DEBUG1("<--CMPXDbManager::CalculateInitalDummyDBSize");
            // If estimated size is larger than disk free size, use free diskspace size,            
            //return estimatedSize > aVol.iFree - 1*KMPMegaByte
            //    ? aVol.iFree - 1*KMPMegaByte : estimatedSize;
            return estimatedSize;
            }
        }
    
#endif //__RAMDISK_PERF_ENABLE    
    }
    

// ---------------------------------------------------------------------------
// CMPXDbManager::GetDatabaseIndex
// ---------------------------------------------------------------------------
//
TInt CMPXDbManager::GetDatabaseIndex(TInt aDrive) 
    {
#ifdef __RAMDISK_PERF_ENABLE
    MPX_DEBUG2("-->CMPXDbManager::GetDatabaseIndex %d", aDrive );
    TInt count(iDatabaseHandles.Count());
    for (TInt i = 0; i < count; ++i)
        {
        if ( iDatabaseHandles[i].iDrive == aDrive )
            {
            return i;
            }
        }    
#endif //__RAMDISK_PERF_ENABLE    
    MPX_DEBUG1("<--CMPXDbManager::GetDatabaseIndex returned -1");
    return -1;
    }


// ---------------------------------------------------------------------------
// CMPXDbManager::EnsureRamSpaceL
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXDbManager::EnsureRamSpaceL() 
    {
#ifdef __RAMDISK_PERF_ENABLE
    MPX_DEBUG1("-->CMPXDbManager::EnsureRamSpaceL");

    if ( iRAMInUse )
        {
        TVolumeInfo vol;
        TInt driveIndex;
        RFs::CharToDrive( iRAMDrive, driveIndex );

        TInt errRAM = iFs.Volume( vol, driveIndex );
        if ( errRAM == KErrNone && vol.iFree < KMPMinimumRAMSizeToRun )
            {
            // RAM not enough, copy back to normal drive and continue to harvest.
            MPX_DEBUG1("CMPXDbManager::EnsureRamSpaceL RAM diskspace is full, copy dbs back");
            CopyDBsFromRamL();
            }
        else
            {
            TInt size=0;
            TInt err = GetTotalRamDatabasesSize(size);
            if ( err || (size > iMaximumAllowedRAMDiskSpaceToCopy) )
                {
                // Databases using too much RAM space, copy back to normal drive and continue to harvest.
                if ( err )
                    {
                    MPX_DEBUG2("CMPXDbManager::EnsureRamSpaceL Get DBs Size Err = %d, copy dbs back", err);
                    }
                else
                    {
                    MPX_DEBUG2("CMPXDbManager::EnsureRamSpaceL DBs using too much RAM space size = %d, copy dbs back", size);
                    }
                CopyDBsFromRamL();
                }
            }
        }
    MPX_DEBUG1("<--CMPXDbManager::EnsureRamSpaceL");
#endif //__RAMDISK_PERF_ENABLE    
    }


// ---------------------------------------------------------------------------
// CMPXDbManager::EnsureDiskSpaceL
// ---------------------------------------------------------------------------
//
void CMPXDbManager::EnsureDiskSpaceL(TInt aDrive) 
    {
    MPX_DEBUG2("-->CMPXDbManager::EnsureDiskSpaceL for drive %d", aDrive);
    // handle the case of C drive
    TDriveUnit drive(aDrive);
    TDriveUnit cdrive(KRootDrive());

    if(drive == cdrive)
        {
        if (SysUtil::DiskSpaceBelowCriticalLevelL(&iFs, 0, aDrive))
            {
            MPX_DEBUG1("CMPXDbManager::EnsureDiskSpaceL Error diskspace full");
            User::Leave(KErrDiskFull);
            }

        return;
        }

    // handle other drives (eg. removable EDriveE)
    TInt count(iDatabaseHandles.Count());
    for (TInt i = 0; i < count; ++i)
        {
        if (((KDbManagerAllDrives == aDrive) ||
            (aDrive == iDatabaseHandles[i].iDrive)) &&
            iDatabaseHandles[i].iOpen)
            {
            if (SysUtil::DiskSpaceBelowCriticalLevelL(&iFs, 0,
                iDatabaseHandles[i].iDrive))
                {
                MPX_DEBUG1("CMPXDbManager::EnsureDiskSpaceL Error diskspace full");
                User::Leave(KErrDiskFull);
                }
            }

        if (aDrive == iDatabaseHandles[i].iDrive)
            {
            // exit if just one drive to check
            break;
            }
        }
    MPX_DEBUG1("<--CMPXDbManager::EnsureDiskSpaceL");
    }


// ---------------------------------------------------------------------------
// CMPXDbManager::DoBackupDBs
// ---------------------------------------------------------------------------
//
/*EXPORT_C void CMPXDbManager::BackupDBsL()
    {
#ifdef __RAMDISK_PERF_ENABLE
    MPX_DEBUG1("-->CMPXDbManager::BackupDBsL");

    TInt transactionCount = iTransactionCount;
    if (iTransactionCount > 0) 
        {
        iTransactionCount = 0;
        DoCommitL();
        }
    
    TInt count(iDatabaseHandles.Count());
    for (TInt i = 0; i < count && iDatabaseHandles[i].iUseRAMdb ; ++i)
        {
        CloseDatabaseL( iDatabaseHandles[i].iDrive );            

        TInt err= BaflUtils::CopyFile(iFs, 
            iDatabaseHandles[i].iTargetFullFilePath->Des(), 
            iDatabaseHandles[i].iOrigFullFilePath->Des());

        MPX_DEBUG2("CMPXDbManager::BackupDBsL err = %d", err);     

        OpenDatabaseL( iDatabaseHandles[i].iDrive );      
        }
        
    if (transactionCount > 0) 
        {
        DoBeginL();
        iTransactionCount = transactionCount;
        }
    MPX_DEBUG1("<--CMPXDbManager::BackupDBsL");
#endif //__RAMDISK_PERF_ENABLE    
    }*/
    
// ---------------------------------------------------------------------------
// CMPXDbManager::GetTotalDatabasesSize
// ---------------------------------------------------------------------------
//
TInt CMPXDbManager::GetTotalDatabasesSize(TInt& aSize)
    {
    MPX_FUNC("CMPXDbManager::GetTotalDatabasesSize");
    TInt err = KErrNotSupported;
#ifdef __RAMDISK_PERF_ENABLE
    TInt size=0;
    TInt count(iDatabaseHandles.Count());
    err = KErrNone;
    for ( TInt i = 0; i < count ; ++i )
        {
        // Generate database name.
        TFileName dbFilename;
        TDriveUnit drive(iDatabaseHandles[i].iDrive);
        dbFilename.Append(drive.Name());
        dbFilename.Append(KDBFilePath);
        TFileName filename;            
        filename.Format(KSecurePath, User::Identity().iUid, iDbFile); //x:\private\10281e17\[sldfdsf]mpxv2_5.db
        dbFilename.Append(filename);
        MPX_DEBUG2("CMPXDbManager::GetTotalDatabasesSize - Database name = %S", &dbFilename);
        TEntry entry;
        err = iFs.Entry( dbFilename, entry );
        if (err == KErrNotFound || err == KErrNotReady )
            {
            MPX_DEBUG3("CMPXDbManager::GetTotalDatabasesSize - Ignored %S, error = %d", &dbFilename, err);
            err = KErrNone;
            continue;
            }
        if ( err != KErrNone )
            {
            break;
            }
        MPX_DEBUG3("CMPXDbManager::GetTotalDatabasesSize - Size of Db %S = %d", &dbFilename, entry.iSize);
        // sum up size
        size += entry.iSize;
        }
    aSize = size;
    MPX_DEBUG2("CMPXDbManager::GetTotalDatabasesSize - Total Size of Dbs = %d", size);
    
#endif //__RAMDISK_PERF_ENABLE    
    MPX_DEBUG2("CMPXDbManager::GetTotalDatabasesSize - Return err = %d", err);
    return err;
    }
    
// ---------------------------------------------------------------------------
// CMPXDbManager::GetTotalRamDatabasesSize
// ---------------------------------------------------------------------------
//
TInt CMPXDbManager::GetTotalRamDatabasesSize(TInt& aSize)
    {
    MPX_FUNC("CMPXDbManager::GetTotalRamDatabasesSize");
    TInt err = KErrNotSupported;
#ifdef __RAMDISK_PERF_ENABLE
    TInt size=0;
    TInt count(iDatabaseHandles.Count());
    err = KErrNone;
    for ( TInt i = 0; i < count ; ++i )
        {
        // make sure this db is in ram drive.
        if ( !iDatabaseHandles[i].iUseRAMdb )
            {
            continue;
            }
        // Generate database name.
        TFileName dbFilename;
        dbFilename.Append(iRAMFolder);
        TBuf<2> d;
        TDriveUnit drive(iDatabaseHandles[i].iDrive);
        d.Append(drive.Name());
        HBufC* temp = HBufC::NewLC(KMaxFileName);
        temp->Des().Append(d.Left(1));
        temp->Des().Append(iDbFile->Des());
        TFileName filename;            
        filename.Format(KSecurePath, User::Identity().iUid, temp);
        CleanupStack::PopAndDestroy(temp);
        dbFilename.Append(filename);
        MPX_DEBUG2("CMPXDbManager::GetTotalRamDatabasesSize - Database name = %S", &dbFilename);
        TEntry entry;
        err = iFs.Entry( dbFilename, entry );
        if ( (err != KErrNone) && (err != KErrNotFound) )
            {
            break;
            }
        MPX_DEBUG3("CMPXDbManager::GetTotalRamDatabasesSize - Size of Db %S = %d", &dbFilename, entry.iSize);
        // sum up size
        size += entry.iSize;
        }
    aSize = size;
    MPX_DEBUG2("CMPXDbManager::GetTotalRamDatabasesSize - Total Size of Dbs = %d", size);
#endif //__RAMDISK_PERF_ENABLE    
    MPX_DEBUG2("CMPXDbManager::GetTotalRamDatabasesSize - Return err = %d", err);
    return err;
    }
    
// End of File
