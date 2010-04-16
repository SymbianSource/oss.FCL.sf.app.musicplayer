/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Tracing macros for clipdownloader.
*
*/


#ifndef LOGGER_H
#define LOGGER_H

#ifndef _DEBUG
    
    #define TRACE( AA )
    
    #define TRACE2( AA, BB )
    
    #define TRACE3( AA, BB, CC )

    #define TRACE4( AA, BB, CC, DD )

    #define TRACE5( AA, BB, CC, DD, EE )

    #define DUMP8( AA )

    #define TRACE_DOWNLOADER_STATE( AA )

    #define TRACE_AND_PANIC( AA )

    #define PANIC_ON_ERROR(AA, BB)

#else
    
    // INCLUDES
    #include <e32debug.h>

    #define TRACE_AND_PANIC( AA ) \
        { \
        RDebug::Printf( "ERROR: " AA ); \
        ASSERT(EFalse); \
        }

    #define PANIC_ON_ERROR(AA, BB) \
        { \
        if(AA) \
            { \
            RDebug::Printf( "ERROR: " BB, AA ); \
            ASSERT(EFalse); \
            } \
        }
        
    #define TRACE( AA ) do { RDebug::Printf( AA ); } while( 0 )
    
    #define TRACE2( AA, BB ) do { RDebug::Printf( AA, BB ); } while( 0 )
    
    #define TRACE3( AA, BB, CC ) do { RDebug::Printf( AA, BB, CC ); } while( 0 )

    #define TRACE4( AA, BB, CC, DD ) do { RDebug::Printf( AA, BB, CC, DD ); } while( 0 )

    #define TRACE5( AA, BB, CC, DD, EE ) do { RDebug::Printf( AA, BB, CC, DD, EE ); } while( 0 )

    #define DUMP8( AA ) \
            TRAP_IGNORE( \
                { \
                HBufC* conversion = HBufC::NewLC( AA.Length() ); \
                TPtr convPtr( conversion->Des() ); \
                convPtr.Copy( AA ); \
                const TInt KMaxLine = 60; \
                for ( TInt index = convPtr.Length(); index > 0; \
                    index -= KMaxLine ) \
                    { \
                    if ( index <= KMaxLine ) \
                        { \
                        RDebug::Print( convPtr ); \
                        break; \
                        } \
                    else  \
                        { \
                        RDebug::Print( convPtr.Left( KMaxLine ) ); \
                        convPtr.Copy( convPtr.Mid( KMaxLine ) ); \
                        } \
                    } \
                CleanupStack::PopAndDestroy( conversion ); \
                } );

                  
#endif // _LOGGER

#endif // _LOGGER

// End of File
