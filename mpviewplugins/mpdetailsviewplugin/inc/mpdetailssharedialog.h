#ifdef SHARE_FUNC_ENABLED
#ifndef MPDETAILSSHAREDIALOG_H
#define MPDETAILSSHAREDIALOG_H

#include <HbDialog>
#include "mpsharedata.h"

class QGraphicsWebView;
class QNetworkAccessManager;
class MpSongData;

/*!
    Implements Share Player dialog for music player details view.
    First, construct an instance of the class, then call initialize method.
*/
class MpDetailsShareDialog : public HbDialog
{
    Q_OBJECT

public:
    MpDetailsShareDialog();

    /*!
      Initializes an instance of the class with given MpSongData.
      MpSongData object ownership is not transferred,
      Music Player Engine retains the ownership.
    */
    void initialize( MpSongData* aSongData );
    virtual ~MpDetailsShareDialog();
    void logoutPlayer();

public slots:
    void addContext();
    void updateSharedData();
    void onIndexLoad( bool ok );
    void debugJs( QString s );
    void errorHandler( QString error, QString message );
    void clearCache();

signals:
    void closeShareDialog();

private:
    void initShareData( MpSongData* aSongData );
    void initNetworkAccessManager();
    void initWebView();
    void initSignalSlots();
    bool initUser();

private:
    MpShareData            mShareData;
    QGraphicsWebView*      mShareWebView;   // Owned by HbDialog
    QNetworkAccessManager* mShareNetAccMan; // Owned
};

#endif // MPDETAILSSHAREDIALOG_H
#endif // SHARE_FUNC_ENABLED
