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
* Description:  Wait Note constructor
*
*/


#ifndef MPX_WAITNOTE_CONSTRUCTOR
#define MPX_WAITNOTE_CONSTRUCTOR

class CMPXWaitNoteDialog;
class MMPXWaitNoteObserver;
/**
* Construct a wait note according to aArg
* @param aArg, TPckgBuf<TInt> argument containing enum of the type
*
* @Since S60, 3.1
*/
class CMPXWaitNoteConstructor : public CBase
    {
public:
    IMPORT_C static CMPXWaitNoteDialog* ConstructWaitNoteL(const TDesC& aArg,
                                                           MMPXWaitNoteObserver* aObs );
    };

#endif // MPX_WAITNOTE_CONSTRUCTOR
