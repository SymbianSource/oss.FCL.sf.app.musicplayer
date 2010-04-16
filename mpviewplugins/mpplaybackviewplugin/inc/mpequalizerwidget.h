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
* Description: Equalizer dialog widget.
*
*/

#ifndef MPEQUALIZERWIDGET_H
#define MPEQUALIZERWIDGET_H

//includes
#include <hbdialog.h>

//forward declartions
class HbRadioButtonList;
class MpEqualizerWrapper;

//class declaration
class MpEqualizerWidget : public HbDialog
{

    Q_OBJECT

public:
    explicit MpEqualizerWidget();
    virtual ~MpEqualizerWidget();

public:
    void initialize();
    void prepareDialog();
    
public slots:
    void presetSelected(int index);
    void equalizerReady();
    void cancelSelected(bool checked);
    void okSelected(bool checked);

private:
    
    MpEqualizerWrapper *mEqualizerWrapper;  //owned    
    HbRadioButtonList  *mPresetsList;  //owned
    
    int  mOriginalPreset;
    bool mEqualizerReady;

    Q_DISABLE_COPY(MpEqualizerWidget)
    
};

#endif //MPEQUALIZERWIDGET_H

//End of File
