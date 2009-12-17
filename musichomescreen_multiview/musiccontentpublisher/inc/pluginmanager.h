/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plugin manager, responsible to load the plugins.
*
*/


#ifndef C_PLUGINMANAGER_H
#define C_PLUGINMANAGER_H

#include <e32base.h>
#include <e32hashtab.h>

class REComSession;
class CMusicContentPublisher;

/**
 *  Plugin manager.
 *  Class used to manage the the ECOM plugins
 *  @since S60 S60 v5.0
 */
class CPluginManager : public CActive
    {
public:

    /**
     * Standard Symbian 2 phase constructor
     */
    static CPluginManager* NewL( TUid aUid, TAny *aParameter , CMusicContentPublisher* aContentPublisher );

    /**
     * Standard Symbian 2 phase constructor
     */
    static CPluginManager* NewLC( TUid aUid, TAny *aParameter , CMusicContentPublisher* aContentPublisher  );

    /**
     * Standard C++ destructor.
     */
    ~CPluginManager();

    /**
     * Gets plugin with provided UID
     *
     * @param aUid required plugin
     * @return pointer to plugin implementation
     */
    TAny* GetImplementation( TUid aUid );

    /**
     * Gets plugin with provided index
     *
     * @param aIndex index of plugin
     * @return pointer to plugin implementation
     */
    TAny* GetImplementation( TInt aIndex );

    /**
     * Gets number of plugins
     *
     * @return number of plugins
     */
    TInt GetCount();

protected:

    /**
     * From CActive, RunL.
     * Handles the active object’s request completion event
     */
    void RunL();

    /**
     * From CActive, DoCancel.
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();

    /**
     * From CActive, RunError.
     * Method called when leave occured in RunL
     */
    TInt RunError( TInt aError );

private:

    /**
     * C++ default constructor.
     */
    CPluginManager( TUid aUid, TAny *aParameter, CMusicContentPublisher* aContentPublisher );

    /**
     * Perform the second phase construction of a CPluginManager object.
     */
    void ConstructL();

    /**
     * Loads ECOM plugins
     */
    void LoadPluginsL();

    /**
     * Cleans plugins table;
     */
    void CleanPluginsTable();

private: //data

    /**
     * ECOM handler
     * Own.
     */
    REComSession* iSession;

    /**
     * Map containing plugins
     * Own.
     */
    RHashMap< TInt32, TAny*> iPluginMap;

    /**
     * UID of the interface
     */
    const TUid iUid;

    /**
     * Parameter to pass to the object creation method.
     */
    TAny* iParameter;
    
    /**
     * Content Publisher / Parent, not owned.
     */
    CMusicContentPublisher* iContentPublisher;

    };

#endif // C_PLUGINMANAGER_H

//  End of File
