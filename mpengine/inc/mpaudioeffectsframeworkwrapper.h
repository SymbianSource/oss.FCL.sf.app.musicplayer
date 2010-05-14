/*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Framework wraper for Audio Effects
*
*/

#ifndef MPAUDIOEFFECTSFRAMEWORKWRAPPER_H
#define MPAUDIOEFFECTSFRAMEWORKWRAPPER_H

#include <QObject>

class MpAudioEffectsFrameworkWrapperPrivate;

class MpAudioEffectsFrameworkWrapper : public QObject
{
    Q_OBJECT
    friend class MpAudioEffectsFrameworkWrapperPrivate;

public:
    
    explicit MpAudioEffectsFrameworkWrapper();
    virtual ~MpAudioEffectsFrameworkWrapper();
    
    int balance();
    bool loudness();
    void setBalance( int balance );
    void setLoudness( bool mode );
    
private: // data
    
    MpAudioEffectsFrameworkWrapperPrivate *d_ptr; //owned
   
};

#endif /*MPAUDIOEFFECTSFRAMEWORKWRAPPER_H*/
