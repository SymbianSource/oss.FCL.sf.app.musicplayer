#include "mpdetailssharedialog.h"
#include "mptrace.h"
#include <QObject>
#include <QGraphicsWebView>
#include <QGraphicsScene>
#include <QWebPage>
#include <QWebFrame>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QDesktopServices>
#include <QNetworkProxyFactory>


// For allowing local caching of javascript files loaded by popup
class MpNetworkAccessManager : public QNetworkAccessManager
{
public:
    MpNetworkAccessManager() { };

private:
    QNetworkReply *createRequest( Operation /*op*/,
                                 const QNetworkRequest &/*request*/,
                                 QIODevice *outgoingData = 0 ) { return 0; }
};


MpDetailsShareDialog::MpDetailsShareDialog(  )
    : mSharedData(  ),
      mShareWebView( 0 ),
      mShareNetAccMan( 0 )
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}

MpDetailsShareDialog::~MpDetailsShareDialog()
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}

void MpDetailsShareDialog::initialize( MpSongData* aSongData )
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}

void MpDetailsShareDialog::updateSharedData()
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}

void MpDetailsShareDialog::reload()
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}

void MpDetailsShareDialog::onIndexLoad( bool aOk )
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}
