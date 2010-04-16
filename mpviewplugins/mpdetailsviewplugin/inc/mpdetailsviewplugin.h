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
* Description: Plugin interface for Music Player details view.
*
*/

#ifndef MPDETAILSVIEWPLUGIN_H
#define MPDETAILSVIEWPLUGIN_H

#include <qobject>
#include <mpxviewpluginqt.h>

//forward declartions
class MpDetailsView;

//class declaration
class MpDetailsViewPlugin : public MpxViewPlugin
{
    Q_OBJECT

public:
    explicit MpDetailsViewPlugin();
    virtual ~MpDetailsViewPlugin();

    void createView();
    void destroyView();
    void activateView();
    void deactivateView();
    QGraphicsWidget *getView();
    
signals:
    void command( int command );

public slots:
    void orientationChange( Qt::Orientation orientation );
    void back();

private:
    enum State { NullView, Created, Initialized, Activated };
    MpDetailsView    *mView; //owned
    State             mState;
};

#endif  // MPPLAYBACKVIEWPLUGIN_H

