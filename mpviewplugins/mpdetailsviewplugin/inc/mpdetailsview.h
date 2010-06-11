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
* Description: Music Player details view.
*
*/

#ifndef MPDETAILSVIEW_H
#define MPDETAILSVIEW_H

#include <QPointer>
#include <QNetworkReply>
#include <QDomDocument>
#include <QMap>

#include "mpviewbase.h"

class QGraphicsWebView;
class QNetworkAccessManager;

class HbMainWindow;
class HbAction;
class HbLabel;
class HbWidget;
class HbGroupBox;
class HbPushButton;
class HbDocumentLoader;
class HbListWidget;

class QTranslator;
class QGraphicsLinearLayout;
class HbProgressBar;

class MpEngine;
class MpSongData;
class MpQueryManager;

#ifdef SHARE_FUNC_ENABLED
#include <qnetworkconfigmanager.h>
QTM_USE_NAMESPACE
class MpDetailsShareDialog;
#endif

//class declaration
/*!
  Details view is the "flipside view" of Music Player.
  In addition to displaying song details, details view
  also provides user the possibility to comment a
  track (by initiating sharedialog), and to get
  recommendations based on his playlist (by displaying
  a list of favourites, fetched from OVI music server).

  DetailsView's song information will not be updated
  if the track changes while detailsview is active.
*/
class MpDetailsView : public MpViewBase
{
    Q_OBJECT
    
public:

    MpDetailsView();
    virtual ~MpDetailsView();

    void initializeView();
    void activateView();
    void deactivateView();

signals:
    void command( int aCommand );
    
public slots:
    void back();    
    void albumArtChanged();
    
private slots:
    void handlePlaybackInfoChanged();
    void songDetailInfoChanged();
    
    void handleDetailsGroupBoxToggled( bool state );
    void handleInspireMeGroupBoxToggled( bool state );
    void RenderInspireMeGroupBox();          
    void handleNetworkError();    
    void updateSharedData(const QString& url);

#ifdef SHARE_FUNC_ENABLED
    void share();
    void closeShareDialog();
#endif

private:
    bool canQueryRecommendations() const;
    bool canQuerySharePlayerLink() const;   
    void setupMenu();
    void clearInspireMe();    

#ifdef SHARE_FUNC_ENABLED
    void createShareDialog();
    void preloadShareDialog();
#endif

private:    
    MpEngine                *mMpEngine;         // Own
    
    MpSongData              *mSongData;
    
    bool                    mActivated;
    HbAction                *mSoftKeyBack;
    
    HbLabel                 *mSongText; //owned
    HbLabel                 *mAlbumText; //owned
    HbLabel                 *mArtistText; //owned
    HbLabel                 *mAlbumArt; //owned
    HbGroupBox              *mSongDetailsGroupBox; //owned
    HbGroupBox              *mInspireMeGroupBox; //owned
    HbPushButton            *mShareButton; //owned
    HbDocumentLoader        *mDocumentLoader;//owned
    HbListWidget            *mDetailList; // owned by mSongDetailsGroupBox
    HbListWidget            *mInspireList; // owned by mInspireMeGroupBox
    HbPushButton            *mButton;
    HbProgressBar           *mInspireMeProgressBar;

    
    QTranslator             *mMpTranslator;         // Own
    QTranslator             *mCommonTranslator;     // Own
    
    MpQueryManager*         mMpQueryManager;     // Own
	
    bool                    mInspireMeQueryOngoing;
    bool                    mInspireMeQueryRendered;
    bool                    mInspireMeOpen;
    bool                    mSongDetailsGbOpen;
    
#ifdef SHARE_FUNC_ENABLED
    MpDetailsShareDialog*   mSharePopup;            // Own
#endif

    Q_DISABLE_COPY(MpDetailsView)
};


#endif //MPDETAILSVIEW_H
