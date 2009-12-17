/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wrapper class for Search Field. Handles search field .
*                activation, deactivation and size. Wrapper is a window owning
*                control.
*
*/


#ifndef CMPXCOMMONCONTAINERSEARCHFIELDWRAPPER_H
#define CMPXCOMMONCONTAINERSEARCHFIELDWRAPPER_H

#include <coecntrl.h>

class CMPXCommonContainerSearchField;

/**
 * Class for handling Search Field events.
 */
NONSHARABLE_CLASS( MMPXCommonContainerSearchFieldObserver )
    {

    public:

    /**
     * Call when filtering word changes. Causes filtering to be updated.
     * Calling with zero-length filtering word stops filtering.
     *
     * @param aWord New filtering word.
     */
    virtual void NewFilteringWordL( const TDesC& aWord ) = 0;
    };

/**
 * Window owning control for wrapping CMPXCommonContainerSearchField.
 *
 */
NONSHARABLE_CLASS( CMPXCommonContainerSearchFieldWrapper ) : public CCoeControl
    {
public:
    /**
     * Creates Search Field Wrapper.
     *
     * @param aObserver Observer
     */
    static CMPXCommonContainerSearchFieldWrapper* NewL( MMPXCommonContainerSearchFieldObserver* aObserver );

    /**
     * Destructor.
     */
    virtual ~CMPXCommonContainerSearchFieldWrapper();

    /**
     * Checks if Search Field is active.
     *
     * @return ETrue if Search Field is active / visible.
     */
    TBool IsSearchFieldActive();

    /**
     * Activates Search Field.
     */
    void ActivateSearchFieldL();

    /**
     * Deactivates Search Field, if active / visible.
     */
    void DeactivateSearchField();

    /**
     * Returns rectangle that list (widget) should use.
     *
     * @param aSearchVisible ETrue if Search Field is visible, list's
     *    rectangle is smaller in this case.
     * @param aClientRect component's view's client rectangle.
     * @return Rectangle for list (widget).
     */
    TRect GetListRect( TBool aSearchVisible, TRect aClientRect );

    /**
     * Returns rectangle that Search Field should use.
     *
     * @param aSearchVisible ETrue if Search Field is visible.
     * @param aClientRect component's view's client rectangle.
     * @return Rectangle for Search Field.
     */
    TRect GetSearchFieldRect( TBool aSearchVisible, TRect aClientRect );

    /**
     * Called by CMPXCommonContainerSearchField when Find key word changes.
     *
     * @param aFindWord New Find key word.
     */
    void FindKeywordL( const TDesC& aFindWord );

    /**
     * Returns view's client rectangle.
     *
     * @return View's client rectangle.
     */
    TRect& ClientRect();

    /**
     * Call when view's client rectangle changes.
     *
     * @param aClientRect New size of view's client rectangle.
     */
    void SetClientRect( const TRect& aClientRect );

public: // From CCoeControl

    /**
     * From CCoeControl
     */
    CCoeControl *ComponentControl( TInt aIndex ) const;

    /**
     * From CCoeControl
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl
     */
    void SizeChanged();

protected:

        /**
         * Constructor.
         *
         * @param aObserver Observer
         */
        CMPXCommonContainerSearchFieldWrapper( MMPXCommonContainerSearchFieldObserver* aObserver );

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

protected:

    /**
     * Search field bserver.
     * Not own.
     */
    MMPXCommonContainerSearchFieldObserver* iObserver;

    /**
     * Pointer to the actual Search Field component.
     * Own.
     */
    CMPXCommonContainerSearchField* iSearchField;

    /**
     * View's client rectangle.
     */
    TRect iClientRect;
    };

#endif // CMPXCOMMONCONTAINERSEARCHFIELDWRAPPER_H
