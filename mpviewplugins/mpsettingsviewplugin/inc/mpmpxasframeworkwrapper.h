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
* Description:  Framework wraper for Audio Settings
*
*/
/*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Framework wraper for Audio Settings
*
*/

#ifndef MPMPXASFRAMEWORKWRAPPER_H
#define MPMPXASFRAMEWORKWRAPPER_H

#include <QObject>

class MpMpxAsFrameworkWrapperPrivate;

class MpMpxAsFrameworkWrapper : public QObject
{
    Q_OBJECT
    friend class MpMpxAsFrameworkWrapperPrivate;

public:
    explicit MpMpxAsFrameworkWrapper();
    virtual ~MpMpxAsFrameworkWrapper();
    
    int balance();
    bool loudness();
    
public slots:
    void setBalance( int balance );
    void setLoudness( bool mode );
    
private:
    MpMpxAsFrameworkWrapperPrivate *d_ptr; //owned
   
};

#endif /*MPMPXASFRAMEWORKWRAPPER_H*/
