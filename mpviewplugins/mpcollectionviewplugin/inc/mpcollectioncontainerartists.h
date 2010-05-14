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
* Description: Music Player collection container definition - Artists.
*
*/

#ifndef MPCOLLECTIONCONTAINERARTISTS_H
#define MPCOLLECTIONCONTAINERARTISTS_H

#include "mpcollectionlistcontainer.h"

class HbGroupBox;

class MpCollectionContainerArtists : public MpCollectionListContainer
{
    Q_OBJECT

public:

    explicit MpCollectionContainerArtists( HbDocumentLoader *loader, QGraphicsItem *parent=0 );
    virtual ~MpCollectionContainerArtists();

public slots:

    void itemActivated( const QModelIndex &index );

private:

    void setupContainer();

private:

    HbGroupBox      *mInfoBar;          // Own

    QString         mArtist;
    QString         mAlbum;

};

#endif  // MPCOLLECTIONCONTAINERARTISTS_H

