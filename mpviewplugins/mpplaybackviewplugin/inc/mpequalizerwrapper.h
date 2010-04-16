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
* Description: Wrapper for equalizer utility
*
*/

#ifndef MPEQUALIZERWRAPPER_H
#define MPEQUALIZERWRAPPER_H

// System includes
#include <qobject>
#include <QStringList>

class MpEqualizerWrapperPrivate;

/**
 *  Wrapper for equalizer utilities
 */
class MpEqualizerWrapper : public QObject
    {
    Q_OBJECT
    Q_PROPERTY(int activePreset READ activePreset)
    Q_PROPERTY(QStringList presetNames READ presetNames)
    
public:

    explicit MpEqualizerWrapper( QObject *parent = 0 );
    virtual ~MpEqualizerWrapper();

    void applyPreset( int preset );
    void disableEqualizer();

    int activePreset();

    QStringList presetNames();

signals:
    void equalizerReady();

private:

    Q_DISABLE_COPY(MpEqualizerWrapper)
    MpEqualizerWrapperPrivate *d_ptr;

    friend class MpEqualizerWrapperPrivate;
    };

#endif // MPEQUALIZERWRAPPER_H

//End of File
