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
* Description: Unit test for mpcollectioncontainers
*
*/

#ifndef TESTMPCOLLECTIONCONTAINERS_H
#define TESTMPCOLLECTIONCONTAINERS_H

#include <QtTest/QtTest>

//Forward declarations
class MpCollectionContainer;
class MpCollectionContainerFactory;
class MpCollectionDocumentLoader;
class MpCollectionView;
class MpMpxCollectionData;
class MpCollectionDataModel;

class TestMpCollectionContainers : public QObject
{
    Q_OBJECT

public:

    TestMpCollectionContainers();
    virtual ~TestMpCollectionContainers();

signals:


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testConstructors();
    void testSetupContainers();
    void testItemActivated();
    void testOnLongPressed();
    
private:

    MpCollectionContainer           *mTest;
    MpCollectionView                *mView;
    MpCollectionDataModel           *mCollectionDataModel;
    MpMpxCollectionData             *mCollectionData;

};

#endif  // TESTMPCOLLECTIONCONTAINERS_H
