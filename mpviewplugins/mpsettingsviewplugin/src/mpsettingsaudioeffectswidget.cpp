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

// System includes
#include <hbslider.h>
#include <hbstackedlayout.h>
#include <hbdataformmodel.h>

// User includes
#include "mpsettingsaudioeffectswidget.h"
#include "mpmpxasframeworkwrapper.h"
#include "mptrace.h"



/*!
    \class MpSettingsAudioEffectsWidget
    \brief This class to set up UI widgets for audio effect settings view.

    Audio Effects widgets allow user to configure settings for Dolby,
    Balance and Loudness.

*/


/*!
    Constructs a new MpSettingsAudioEffectsWidget with \a parent.
*/
MpSettingsAudioEffectsWidget::MpSettingsAudioEffectsWidget( QGraphicsItem *parent  )
    : HbWidget( parent ),
      mFrameworkWrapper( 0 )
{
    TX_LOG
}

/*!
    Destructs the widget.
 */
MpSettingsAudioEffectsWidget::~MpSettingsAudioEffectsWidget()
{
    delete mFrameworkWrapper;
    delete mModel;
    TX_LOG
}

/*!
    Initialize the widget.
*/
void MpSettingsAudioEffectsWidget::initialize()
{
    TX_ENTRY

    mFrameworkWrapper = new MpMpxAsFrameworkWrapper();

    //create data form
    HbDataForm *dataForm = new HbDataForm();

    //create a model class
    mModel = new HbDataFormModel();
    HbDataFormModelItem * slider;
    HbDataFormModelItem *loudnessItem;
    slider = mModel->appendDataFormItem( HbDataFormModelItem::SliderItem, hbTrId( "txt_mus_setlabel_balance" ), mModel->invisibleRootItem() );
    loudnessItem = mModel->appendDataFormItem( HbDataFormModelItem::ToggleValueItem, hbTrId( "txt_mus_setlabel_loudness" ), mModel->invisibleRootItem() );
    slider->setContentWidgetData( QString( "minimum" ),-100 );
    slider->setContentWidgetData( QString( "maximum" ),100 );
    slider->setContentWidgetData( QString( "majorTickInterval" ),100 );
    slider->setContentWidgetData( QString( "minorTickInterval" ),25 );
    slider->setContentWidgetData( QString( "singleStep" ),25 );
    slider->setContentWidgetData( QString( "toolTipVisible" ), false );
    slider->setContentWidgetData( QString( "tickPosition" ), Hb::SliderTicksRight );
    slider->setContentWidgetData( QString( "sliderPosition" ),mFrameworkWrapper->balance() );
    if ( mFrameworkWrapper->loudness() == true ) {
        loudnessItem->setContentWidgetData( QString( "text" ), hbTrId( "txt_mus_setlabel_loudness_val_on" ) );
        loudnessItem->setContentWidgetData( QString( "additionalText" ), hbTrId( "txt_mus_setlabel_loudness_val_off" ) );
    }
    else {
        loudnessItem->setContentWidgetData( QString( "text" ), hbTrId( "txt_mus_setlabel_loudness_val_off" ) );
        loudnessItem->setContentWidgetData( QString( "additionalText" ), hbTrId( "txt_mus_setlabel_loudness_val_on" ) );
    }

    dataForm->setModel( mModel );
    dataForm->setParentItem( this );
    HbStackedLayout *mylayout;
    mylayout = new HbStackedLayout( this );
    mylayout->setContentsMargins( 0.0, 0.0, 0.0, 0.0  );
    mylayout->setMinimumSize( 0.0, 0.0  );
    mylayout->addItem( dataForm  );
    setLayout( mylayout  );
    dataForm->addConnection( slider, SIGNAL( valueChanged( int  ) ),
            mFrameworkWrapper, SLOT( setBalance( int  ) ) );
    connect( mModel, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( filterSignals( QModelIndex, QModelIndex ) ) );

    TX_EXIT
}

void  MpSettingsAudioEffectsWidget::filterSignals( QModelIndex startIn, QModelIndex endIn )
{
    Q_UNUSED( endIn );
    HbDataFormModelItem *itm = mModel->itemFromIndex( startIn );
    if ( itm->type() == HbDataFormModelItem::ToggleValueItem ) {
        if( itm->data( HbDataFormModelItem::LabelRole ).toString() == hbTrId( "txt_mus_setlabel_loudness" ) ) {
            if( itm->contentWidgetData( QString( "text" ) ).toString() == hbTrId( "txt_mus_setlabel_loudness_val_on" ) ) {
                mFrameworkWrapper->setLoudness( true );
            }
            else if( itm->contentWidgetData( QString( "text" ) ).toString() == hbTrId( "txt_mus_setlabel_loudness_val_off" ) ) {
                mFrameworkWrapper->setLoudness( false );
            }
            else {
                TX_LOG_ARGS( "Error: should never get here." );
            }
        }
    }
}


