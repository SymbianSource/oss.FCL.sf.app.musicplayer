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
#ifdef SHARE_FUNC_ENABLED

#ifndef MPSHAREDATA_H
#define MPSHAREDATA_H

#include <QObject>
#include <QString>
#include <QPixmap>

class MpSongData;
class HbIcon;

/*!
  MpShareData represents the information that is
  used for sharing purposes. This information includes
  e.g. song information and user information.
*/
class MpShareData : public QObject
{
    Q_OBJECT

public:
    MpShareData();
    virtual ~MpShareData();

    void setOwner( QObject* aOwner );
    void setSongData( MpSongData* aSongData );
    void setErrorMessage( const QString& s );
	void setLanguage( const QString& s );

public slots:
    QObject* owner() const;
    MpSongData* songData() const;
    QString errorMessage() const;
    void setUsername( const QString& s );
    QString username() const;
    void setPassword( const QString& s );
    QString password() const;
    QString objectType() const;
    QString objectContent() const;
    int objectReservedLength() const;
	QString language() const;
    QString title() const;
    QString album() const;
    QString artist() const;
    QString albumArtBase64() const;

private:
    QObject*    mOwner;    // NOT owned.
    MpSongData* mSongData; // NOT owned.
    QString     mErrorMessage;
    QString     mUsername;
    QString     mPassword;
	QString     mLanguage;
};

#endif // MPSHAREDATA_H
#endif // SHARE_FUNC_ENABLED
