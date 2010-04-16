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

#include <xqaiwrequest.h>
#include <QGraphicsLinearLayout>
#include <hbpushbutton.h>
#include <hblineedit.h>
#include <hblabel.h>
#include <qdebug>
#include "mpfetchertestappview.h"


MpFetcherTestAppView::MpFetcherTestAppView(QGraphicsItem *parent) :
    HbView(parent),
    mReq(0),
    mResultEdit(0),
    mErrorCodeEdit(0),
    mErrorEdit(0)
{
    qDebug() << "MpFetcherTestAppView::MpFetcherTestAppView";
    createLayout();
}

MpFetcherTestAppView::~MpFetcherTestAppView()
{
    qDebug() << "MpFetcherTestAppView::~MpFetcherTestAppView";
    delete mReq;
}

void MpFetcherTestAppView::handleOk(const QVariant &result)
{
    qDebug() << "MpFetcherTestAppView::handleOk";

    if (!result.canConvert<QString>())
    {
        mErrorEdit->setText("Corrupt result");
    }
    else
    {
        mResultEdit->setText(result.value<QString>());
        qDebug() << "MpFetcherTestAppView::handleOk" << ": result=" << result.value<QString>();
    }

}

void MpFetcherTestAppView::handleError(int errorCode, const QString& errorMessage)
{
    qDebug() << "MpFetcherTestAppView::handleError" << ": errorCode=" << errorCode << ", msg:" << errorMessage;
    mErrorEdit->setText(errorMessage);
    mErrorCodeEdit->setText(QString::number(errorCode));
}

void MpFetcherTestAppView::createLayout()
{
    qDebug() <<  "MpFetcherTestAppView::createLayout";

    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);

    if (layout)
    {
        QGraphicsLinearLayout *topLayout = new QGraphicsLinearLayout(Qt::Vertical);

        if (topLayout)
        {
            topLayout->addStretch(5);

            mResultEdit = new HbLineEdit;
            HbLabel* label = new HbLabel("Result:");
            if (mResultEdit && label)
            {
                topLayout->addItem(label);
                mResultEdit->setMaxRows(5);
                topLayout->addItem(mResultEdit);
            }

            mErrorCodeEdit = new HbLineEdit;
            label = new HbLabel("Error code:");
            if (mErrorCodeEdit && label)
            {
                topLayout->addItem(label);
                topLayout->addItem(mErrorCodeEdit);
            }

            mErrorEdit = new HbLineEdit;
            label = new HbLabel("Error description:");
            if (mErrorEdit && label)
            {
                topLayout->addItem(label);
                mErrorEdit->setMaxRows(5);
                topLayout->addItem(mErrorEdit);
            }

            layout->addItem(topLayout);
            layout->setStretchFactor(topLayout, 5);
        }

        QGraphicsLinearLayout *bottomLayout = new QGraphicsLinearLayout(Qt::Vertical);

        if (bottomLayout)
        {
            bottomLayout->addStretch();
            HbPushButton* button = new HbPushButton("Fetch song");
            if (button)
            {
                connect(button, SIGNAL(clicked()), SLOT(fetchSong()));
                bottomLayout->addItem(button);
            }
            layout->addItem(bottomLayout);
        }

        setLayout(layout); // Takes ownership of layout
    }
}

void MpFetcherTestAppView::fetchSong()
{
    qDebug() <<  "MpFetcherTestAppView::fetchSong START";

    mResultEdit->setText("");
    mErrorEdit->setText("");
    mErrorCodeEdit->setText("");

    if (!mReq)
    {
        mReq = mAppMgr.create("com.nokia.services.media.Music", "fetch(QString)", true);

        qDebug() <<  "MpFetcherTestAppView::fetchSong: mReq=" << mReq;

        if (!mReq)
        {
            mErrorEdit->setText("Failed to create REQ");
            return;
        }
        else
        {
            connect(mReq, SIGNAL(requestOk(const QVariant&)), SLOT(handleOk(const QVariant&)));
            connect(mReq, SIGNAL(requestError(int,const QString&)), SLOT(handleError(int,const QString&)));
        }
    }

    // Set arguments for request (application title)
    QList<QVariant> args;
    args << QVariant(QString("<app_name>"));
    mReq->setArguments(args);

    // Make the request
    if (!mReq->send())
    {
        mErrorEdit->setText("Failed to send REQ");
        qDebug() <<  "MpFetcherTestAppView::fetchSong: XQAiwRequest::send returned false";
    }

    qDebug() <<  "MpFetcherTestAppView::fetchSong END";
}
