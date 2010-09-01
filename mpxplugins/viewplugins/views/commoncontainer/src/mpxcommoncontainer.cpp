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
* Description:  Factory function to create the common container
*
*/



#include <e32base.h>
#include "mpxcommoncontainer.h"
#include "mpxcommoncontaineravkonimp.h"

#ifdef __HG_COLLECTIONVIEW
#include "mpxcommoncontainerhgimp.h"
#endif //__HG_COLLECTIONVIEW

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two Phased Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C MMPXCommonContainer* CMPXCommonContainerFactory::NewL()
    {
#ifdef __HG_COLLECTIONVIEW
    return new (ELeave) CMPXCommonContainerHgImp();
#else
    return new (ELeave) CMPXCommonContainerAvkonImp();
#endif // __ENABLE_HITCHCOCK_UI
    }


//  End of File
