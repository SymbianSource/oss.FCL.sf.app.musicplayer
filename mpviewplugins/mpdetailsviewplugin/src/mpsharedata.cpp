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

#include "mpsharedata.h"
#include "mpsongdata.h"

// TODO OVI_URL needs to come from cenrep
const QString OVI_URL = "http://music.ovi.com";

// The music note symbol that we post if we do not have music store URL.
const QString MUSIC_NOTE_SYMBOL = "&#9835; ";

MpShareData::MpShareData()
    : mOwner( 0 ),
      mSongData( 0 )
{
}

MpShareData::~MpShareData()
{
}

void MpShareData::setOwner( QObject* aOwner )
{
    mOwner = aOwner;
}

QObject* MpShareData::owner() const
{
    return mOwner;
}

void MpShareData::setSongData( MpSongData* aSongData )
{
    mSongData = aSongData;
}

MpSongData* MpShareData::songData() const
{
    return mSongData;
}

void MpShareData::setErrorMessage( const QString& s )
{
    mErrorMessage = s;
}

QString MpShareData::errorMessage() const
{
    return mErrorMessage;
}

void MpShareData::setUsername( const QString& s )
{
    mUsername = s;
}

QString MpShareData::username() const
{
    return mUsername;
}

void MpShareData::setPassword( const QString& s )
{
    mPassword = s;
}

QString MpShareData::password() const
{
    return mPassword;
}

void MpShareData::setLanguage( const QString& s )
{
	mLanguage = s;
}

QString MpShareData::language() const
{
	return mLanguage;
}

QString MpShareData::objectType() const
{
    if ( !mSongData || mSongData->link().isEmpty() )
    {
        return "NOTE-APPEND";
    }
    else
    {
        // No link, append artist-song
        return "URI";
    }
}

int MpShareData::objectReservedLength() const
{
    return objectContent().length();
}

QString MpShareData::objectContent() const
{
	QString result = "";
    if ( mSongData )
    {
        if ( !mSongData->link().isEmpty() )
        {
            return mSongData->link();
        }
        else if ( !mSongData->artist().isEmpty() && !mSongData->title().isEmpty() )
        {
            // TODO: do we need to have right-to-left text direction here,
            // i.e. putting the title before the artist in such a case?
            result = MUSIC_NOTE_SYMBOL + mSongData->artist() + ": " + mSongData->title() + " " + OVI_URL;
        }
        else if ( !mSongData->artist().isEmpty() )
        {
            result = MUSIC_NOTE_SYMBOL + mSongData->artist() + " " + OVI_URL;
        }
        else if ( !mSongData->title().isEmpty() )
        {
            result = MUSIC_NOTE_SYMBOL + mSongData->title() + " " + OVI_URL;
        }
        else
        {
            result = MUSIC_NOTE_SYMBOL + OVI_URL;
        }
    }
    else
    {
        result = MUSIC_NOTE_SYMBOL + OVI_URL;
    }
    return result;
}

QString MpShareData::title() const
{
    return mSongData ? mSongData->title() : "";
}

QString MpShareData::album() const
{
    return mSongData ? mSongData->album() : "";
}

QString MpShareData::artist() const
{
    return mSongData ? mSongData->artist() : "";
}

QString MpShareData::albumArtBase64() const
{
    return mSongData ? mSongData->albumArtBase64() : "";
}

#endif // SHARE_FUNC_ENABLED
