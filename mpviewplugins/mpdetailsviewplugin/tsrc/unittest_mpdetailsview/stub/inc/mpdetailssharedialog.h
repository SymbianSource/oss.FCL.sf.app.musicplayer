#ifndef MPDETAILSSHAREDIALOG_H
#define MPDETAILSSHAREDIALOG_H

#include <HbDialog>

class QGraphicsWebView;
class QNetworkAccessManager;
class MpSongData;

class MpDetailsShareDialog : public HbDialog
{
    Q_OBJECT

public:
    MpDetailsShareDialog();
    void initialize( MpSongData* aSongData );
    virtual ~MpDetailsShareDialog();
    void reload();

public slots:
    void updateSharedData();
    void onIndexLoad( bool ok );

private:
    QObject*               mSharedData;     // NOT owned.
    QGraphicsWebView*      mShareWebView;   // Owned by HbDialog
    QNetworkAccessManager* mShareNetAccMan; // Owned
};

#endif // MPDETAILSSHAREDIALOG_H
