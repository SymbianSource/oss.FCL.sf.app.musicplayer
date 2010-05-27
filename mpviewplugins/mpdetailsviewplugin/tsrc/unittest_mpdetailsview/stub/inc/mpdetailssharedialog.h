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
* Description: 
*
*/
#ifndef MPDETAILSSHAREDIALOG_H
#define MPDETAILSSHAREDIALOG_H

#include <HbDialog>

class QGraphicsWebView;
class QNetworkAccessManager;
class MpSongData;

class MpDetailsShareDialog : public HbDialog
{
    Q_OBJECT

public:
    MpDetailsShareDialog();
    void initialize( MpSongData* aSongData );
    virtual ~MpDetailsShareDialog();
    void reload();

public slots:
    void updateSharedData();
    void onIndexLoad( bool ok );

private:
    QObject*               mSharedData;     // NOT owned.
    QGraphicsWebView*      mShareWebView;   // Owned by HbDialog
    QNetworkAccessManager* mShareNetAccMan; // Owned
};

#endif // MPDETAILSSHAREDIALOG_H
