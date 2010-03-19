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
* Description: Music Player collection container definition - Genres.
*
*/

#ifndef MPCOLLECTIONCONTAINERGENRES_H
#define MPCOLLECTIONCONTAINERGENRES_H

#include "mpcollectionlistcontainer.h"

class MpCollectionContainerGenres : public MpCollectionListContainer
{
    Q_OBJECT

public:

    explicit MpCollectionContainerGenres( HbDocumentLoader *loader, QGraphicsItem *parent=0 );
    virtual ~MpCollectionContainerGenres();

private:

    void setupContainer();

};

#endif  // MPCOLLECTIONCONTAINERGENRES_H

