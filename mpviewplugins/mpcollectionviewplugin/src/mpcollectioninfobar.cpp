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
* Description: Music Player collection container information bar.
*
*/

#include <QPainter>
#include <QColor>

#include <hbstackedlayout.h>
#include <hblabel.h>
#include <hbdocumentloader.h>
#include <hbinstance.h>
#include <hbmainwindow.h>

#include "mpcollectioninfobar.h"
#include "mptrace.h"


const char*MUSIC_COLLECTION_INFOBAR_DOCML = ":/docml/musiccollection_infobar.docml";


/*!
    \class MpCollectionInfoBar
    \brief Music Player collection container information bar.

    Collection Info Bar is a custom widget,similar to HbWidget that provides
    up to two text labels, one aligned to left and the other to the right.
*/

/*!
 Constructs the information bar widget.
 */
MpCollectionInfoBar::MpCollectionInfoBar( QGraphicsItem *parent )
    : HbWidget(parent),
      mContainer(0),
      mTextLeft(0),
      mTextRight(0)
{
    TX_ENTRY
    mDocumentLoader = new HbDocumentLoader();
    bool ok = false;
    mDocumentLoader->load( MUSIC_COLLECTION_INFOBAR_DOCML, &ok );
    if ( ok ) {
        QGraphicsWidget *widget;
        widget = mDocumentLoader->findWidget(QString("infoBarContainer"));
        mContainer = qobject_cast<HbWidget*>(widget);

        widget = mDocumentLoader->findWidget(QString("leftText"));
        mTextLeft = qobject_cast<HbLabel*>(widget);

        widget = mDocumentLoader->findWidget(QString("rightText"));
        mTextRight = qobject_cast<HbLabel*>(widget);
    }
    else {
        TX_LOG_ARGS("Error: invalid xml file.");
        Q_ASSERT_X(ok, "MpCollectionInfoBar", "invalid xml file");
    }

    HbStackedLayout *layout = new HbStackedLayout(this);
    layout->addItem(mContainer);

	HbMainWindow *mainWindow = hbInstance->allMainWindows()[0];
    mOrientation = mainWindow->orientation();
    connect( mainWindow, SIGNAL( orientationChanged(Qt::Orientation) ),
             this, SLOT( orientationChange(Qt::Orientation) ) );
    TX_EXIT
}

/*!
 Destructs the widget.
 */
MpCollectionInfoBar::~MpCollectionInfoBar()
{
    TX_ENTRY
    delete mContainer;
    delete mDocumentLoader;
    TX_EXIT
}

/*!
 Sets the left label text to \a textLeft.
 */
void MpCollectionInfoBar::setText(const QString &textLeft)
{
    TX_ENTRY_ARGS("textLeft=" << textLeft);

    mTextLeft->setPlainText(textLeft);
    mTextRight->setPlainText(QString::null);

    bool ok1 = false;
    bool ok2 = false;
    mDocumentLoader->load(MUSIC_COLLECTION_INFOBAR_DOCML, "single", &ok1);
    if ( mOrientation == Qt::Vertical ) {
        mDocumentLoader->load(MUSIC_COLLECTION_INFOBAR_DOCML, "single singlePortrait", &ok2);
    }
    else {
        mDocumentLoader->load(MUSIC_COLLECTION_INFOBAR_DOCML, "single singleLandscape", &ok2);
    }
    if ( !ok1 || !ok2 ) {
        TX_LOG_ARGS("Error: invalid xml file.");
        Q_ASSERT_X((ok1 || ok2), "MpCollectionInfoBar::setText 1", "invalid xml file");
    }
    mSingleText = true;
    TX_EXIT
}

/*!
 Sets the left label text to \a textLeft and the right label text to \a textRight.
 */
void MpCollectionInfoBar::setText(const QString &textLeft, const QString &textRight)
{
    TX_ENTRY_ARGS("textLeft=" << textLeft << ", textRight=" << textRight);

    mTextLeft->setPlainText(textLeft);
    mTextRight->setPlainText(textRight);

    bool ok1 = false;
    bool ok2 = false;
    mDocumentLoader->load(MUSIC_COLLECTION_INFOBAR_DOCML, "double", &ok1);
    if ( mOrientation == Qt::Vertical ) {
        mDocumentLoader->load(MUSIC_COLLECTION_INFOBAR_DOCML, "double doublePortrait", &ok2);
    }
    else {
        mDocumentLoader->load(MUSIC_COLLECTION_INFOBAR_DOCML, "double doubleLandscape", &ok2);
    }
    if ( !ok1 || !ok2 ) {
        TX_LOG_ARGS("Error: invalid xml file.");
        Q_ASSERT_X((ok1 || ok2), "MpCollectionInfoBar::setText 2", "invalid xml file");
    }
    mSingleText = false;
    TX_EXIT
}

/*!
    \reimp
 */
void MpCollectionInfoBar::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    TX_ENTRY
    HbWidget::paint(painter, option, widget);
    QColor color(105,105,105,255);
    painter->fillRect(rect(), color);
    TX_EXIT
}

/*!
 Slot to be called when orientation is changed.
 Loads the correct layout based on the \a orientation.
 */
void MpCollectionInfoBar::orientationChange( Qt::Orientation orientation )
{
    TX_ENTRY
    mOrientation = orientation;
    if ( mSingleText ) {
        setText( mTextLeft->plainText() );
    }
    else {
        setText( mTextLeft->plainText(), mTextRight->plainText() );
    }
    TX_EXIT
}
