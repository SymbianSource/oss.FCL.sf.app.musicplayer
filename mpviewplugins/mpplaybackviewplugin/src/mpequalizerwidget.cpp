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
* Description: Equalizer widget for Music Player playback view.
*
*/

// System includes
#include <QStringList>
#include <hblabel.h>
#include <hbaction.h>
#include <hbradiobuttonlist.h>

// User includes
#include "mpequalizerwidget.h"
#include "mpequalizerwrapper.h"
#include "mptrace.h"

// CONSTANTS
const int KEqualizerPresetNone = -1;

/*!
    \class MpEqualizerWidget
    \brief Provide radio button list to select a predefined eq. preset
*/

/*!
 Constructs the Equalizer widget.
 */
MpEqualizerWidget::MpEqualizerWidget() :
    mEqualizerWrapper(0),
    mPresetsList(0),
    mOriginalPreset(KEqualizerPresetNone),
    mEqualizerReady(false)
{
    TX_ENTRY
    
    initialize();
    
    TX_EXIT
}

/*!
 Destructs the widget.
 */
MpEqualizerWidget::~MpEqualizerWidget()
{
    TX_ENTRY
    
    delete mEqualizerWrapper;
    
    TX_EXIT
}

/*!
 Initialize dialog by loading the EqualizerUtility.
 */
void MpEqualizerWidget::initialize()
{
    TX_ENTRY
    
    mEqualizerWrapper = new MpEqualizerWrapper( this );

    connect( mEqualizerWrapper,
            SIGNAL( equalizerReady() ),
            this,
            SLOT( equalizerReady() ) );

    TX_EXIT
}

/*!
 Prepare dialog by loading the presets list.
 */
void MpEqualizerWidget::prepareDialog()
{
    TX_ENTRY
    
    setTimeout(NoTimeout);
    setDismissPolicy(HbDialog::NoDismiss);
    setFrameType(HbPopup::Strong);
    setHeadingWidget(new HbLabel(hbTrId("txt_mus_title_select_preset")));    
    setPrimaryAction(new HbAction(hbTrId("txt_common_button_ok")));
    setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel")));
    
    if ( mEqualizerReady ) {
        mOriginalPreset = mEqualizerWrapper->activePreset();
        QStringList listItems = mEqualizerWrapper->presetNames();
        listItems.prepend(hbTrId("txt_mus_list_off"));
    
        int presetSelected;
        if (mOriginalPreset == KEqualizerPresetNone) {
            presetSelected = 0;     //First list item corresponds to "OFF"
        }
        else {
            presetSelected = mOriginalPreset;
        }
    
        mPresetsList = new HbRadioButtonList(listItems, presetSelected, HbRadioButtonList::NoPreview, this);
        setContentWidget( mPresetsList );  //mPresetsList now owned by HbDialog
    
        connect( mPresetsList, 
               SIGNAL(itemSelected(int)), 
               this, 
               SLOT(presetSelected(int)));
    }
    
    connect( secondaryAction(),
            SIGNAL( triggered(bool) ),
            this,
            SLOT( cancelSelected(bool) ) );
    
    connect( primaryAction(),
            SIGNAL( triggered(bool) ),
            this,
            SLOT( okSelected(bool) ) );
    
    TX_EXIT
}

/*!
 Slot to handle when a preset has been selected.
 */
void MpEqualizerWidget::presetSelected(int index)
{
    TX_ENTRY
    
    if (mEqualizerWrapper) {
        if (index == 0) {
            mEqualizerWrapper->disableEqualizer();
        }
        else{
            mEqualizerWrapper->applyPreset( index );
        }
    }
    
    TX_EXIT
}

/*!
 Slot to handle when Cancel is selected
 */
void MpEqualizerWidget::cancelSelected(bool checked)
{
    TX_ENTRY
    Q_UNUSED(checked);
    
    if (mEqualizerWrapper && mPresetsList) {
        // Return to the original Preset
        if (mOriginalPreset != mEqualizerWrapper->activePreset()) {
            if (mOriginalPreset == KEqualizerPresetNone) {
                mEqualizerWrapper->disableEqualizer();
                mPresetsList->setSelected( 0 );     //First list item corresponds to "OFF"
            }
            else {
                mEqualizerWrapper->applyPreset( mOriginalPreset );
                mPresetsList->setSelected( mOriginalPreset );
            }            
        }
    }
    
    TX_EXIT
}

/*!
 Slot to handle when Ok is selected
 */
void MpEqualizerWidget::okSelected(bool checked)
{
    TX_ENTRY
    Q_UNUSED(checked);    
    
    //Update original preset
    if (mEqualizerWrapper) {
        mOriginalPreset = mEqualizerWrapper->activePreset();
    }
    
    TX_EXIT
}

/*!
 Equalizer utility is ready
 */
void MpEqualizerWidget::equalizerReady()
{
    TX_ENTRY
    
    mEqualizerReady = true;
    
    TX_EXIT
}

//End of File
