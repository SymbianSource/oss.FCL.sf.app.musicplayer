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
* Description: Unit Test for mpdetailsview.
*
*/

#ifndef UNITTEST_MPDETAILSVIEW_H_
#define UNITTEST_MPDETAILSVIEW_H_

#include <QtTest/QtTest>
#include <hbview.h>
#include <QNetworkReply>

//Forward Declaration
class MpDetailsView;



class TestMpDetailsView : public HbView
{
    Q_OBJECT

public:

    TestMpDetailsView();
    virtual ~TestMpDetailsView();
    

signals:
    void back ();    
    void albumArtChanged();
    void share();
    void webViewLoaded( bool ok );
    void playbackInfoChanged();
    void songDetailInfoChanged();
    
    void toggleDetailsGroupBox( bool state );
    void toggleInspireMeGroupBox( bool state );
      
    void retrieveInformationFinished( QNetworkReply* reply );
    void retrieveInformationNetworkError( QNetworkReply::NetworkError error );
    void retrieveInformationSslErrors( const QList<QSslError> &error );
    
    void DownloadFinished( QNetworkReply* reply );
    
    void addContext();
    void close();
    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
private slots:
  //Test Cases  
    void testactivateView();
    void testdeactivateView();
    void testbackSlot();
    
private:
    MpDetailsView *mTest;
    
};

#endif /* UNITTEST_MPDETAILSVIEW_H_ */
