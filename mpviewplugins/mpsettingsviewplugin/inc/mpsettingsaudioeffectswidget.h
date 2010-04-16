/*
* Copyright ( c ) 2009 Nokia Corporation and/or its subsidiary( -ies ).
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
* Description: Music Player audio effect view.
*
*/

#ifndef MPSETTINGSAUDIOEFFECTSWIDGET_H
#define MPSETTINGSAUDIOEFFECTSWIDGET_H


// INCLUDES
#include <qobject>
#include <hbwidget.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>

// CLASS DECLARATION
class MpMpxAsFrameworkWrapper;
class HbSlider;

/**
 *  Audio Effects widget definition.
 *
 */
class MpSettingsAudioEffectsWidget : public HbWidget
    {
    Q_OBJECT
    
public:
    explicit MpSettingsAudioEffectsWidget( QGraphicsItem *parent=0  );
    virtual ~MpSettingsAudioEffectsWidget();
    
    void initialize();

public slots:
    
    void filterSignals( QModelIndex startIn, QModelIndex endIn );
    
private:

    MpMpxAsFrameworkWrapper *mFrameworkWrapper;
    HbDataFormModel         *mModel;
    };

#endif  // MPSETTINGSAUDIOEFFECTSWIDGET_H

// End of File
