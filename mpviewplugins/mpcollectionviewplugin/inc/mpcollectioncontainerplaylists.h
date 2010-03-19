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
* Description: Music Player collection container definition - Playlists.
*
*/

#ifndef MPCOLLECTIONCONTAINERPLAYLISTS_H
#define MPCOLLECTIONCONTAINERPLAYLISTS_H

#include "mpcollectionlistcontainer.h"

class MpCollectionInfoBar;

class MpCollectionContainerPlaylists : public MpCollectionListContainer
{
    Q_OBJECT

public:

    explicit MpCollectionContainerPlaylists( HbDocumentLoader *loader, QGraphicsItem *parent=0 );
    virtual ~MpCollectionContainerPlaylists();

private:

    void setupContainer();

private:

    MpCollectionInfoBar     *mInfoBar;
    bool                    mListInitialized;

};

#endif  // MPCOLLECTIONCONTAINERPLAYLISTS_H

