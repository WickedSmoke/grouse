/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#include <QApplication>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkProxy>
#include <QTemporaryFile>
#include <QSslConfiguration>
#include "netservice.h"
#include "common.h"


void NetService::applyProxyOptions( const AppOptions* opt )
{
  QNetworkProxy proxy;
  proxy.setType(opt->enableproxy ? QNetworkProxy::HttpProxy
                                 : QNetworkProxy::NoProxy);
  proxy.setHostName(opt->proxyhost);
  proxy.setPort(opt->proxyport);
  proxy.setUser(opt->proxyuser);
  proxy.setPassword(opt->proxypass);
  QNetworkProxy::setApplicationProxy (proxy);
}


// constructor
NetService::NetService (qint16  nettimeout, const QByteArray& httpHeader, QObject *parent)
{
  timeout = nettimeout;
  header = httpHeader;
  cookie = "";
  Request.reserve (20);

  if (parent != nullptr)
    setParent (parent);
}

// destructor
NetService::~NetService ()
{

}

// add request in vector
bool
NetService::addRequest (QString url, void * replyBuffer)
{
  NetRequestClass *request;

  foreach (request, Request)
    if (request->url == url)
      return false;

  foreach (request, Request)
    if (request->actualUrl == url)
      return false;

  request = new NetRequestClass;
  request->actualUrl = request->url = url.trimmed ().simplified ();
  request->finished = 0;
  request->error = CG_ERR_OK;
  request->replyBytes = replyBuffer;
  Request += request;

  return true;
}

// check request's status
int
NetService::checkRequestStatus (QString url, int & error)
{
  url = url.trimmed ().simplified ();
  foreach (NetRequestClass *request, Request)
    if (request->url == url)
    {
      error = request->error;
      return request->finished.fetchAndAddAcquire (0);
    }

  foreach (NetRequestClass *request, Request)
    if (request->actualUrl == url)
    {
      error = request->error;
      return request->finished.fetchAndAddAcquire (0);
    }

  return 0;
}

// get reply buffer
void *
NetService::getReplyBuffer (QString url)
{
  url = url.trimmed ().simplified ();
  foreach (const NetRequestClass *request, Request)
  {
    if (request->url == url || request->actualUrl == url)
      return request->replyBytes;
  }

  return nullptr;
}

// set the status of a request
void
NetService::setRequestStatus (QString url, int status, int error)
{
  url = url.trimmed ().simplified ();
  foreach (NetRequestClass *request, Request)
    if (request->url == url)
    {
      request->error = error;
      request->finished = status;
      return;
    }

  foreach (NetRequestClass *request, Request)
    if (request->actualUrl == url)
    {
      request->error = error;
      request->finished = status;
      return;
    }
}

// set actual url
void
NetService::setActualUrl (QString url, QString aurl)
{
  url = url.trimmed ().simplified ();
  aurl = aurl.trimmed ().simplified ();
  foreach (NetRequestClass *request, Request)
    if (request->url == url)
      request->actualUrl = aurl;
}

// remove request from vector
void
NetService::delRequest (QString url)
{
  int reqid = -1;

  if (Request.size () < 1)
    return;

  url = url.trimmed ().simplified ();
  for (int counter = 0; counter < Request.size (); counter ++)
    if (Request[counter]->url == url)
      reqid = counter;

  delete Request[reqid];
  Request.removeAt (reqid);
}

// downloads the url and returns a string with a temporary filename
CG_ERR_RESULT
NetService::httpGET (QString url, QFile & tempFile, Cookies *cookies)
{
  QUrl qurl;
  QNetworkRequest request;
  QByteArray *replyBytes;
  QNetworkAccessManager *httpAccessManager = nullptr;
  QNetworkReply *reply = nullptr;
  qint32 tcounter = 0, invl = 200;
  CG_ERR_RESULT result = -1;

  replyBytes = new QByteArray;
  qurl = QUrl::fromEncoded(url.toLatin1 ());
  if (!addRequest (qurl.toString (), static_cast <void *> (replyBytes)))
  {
    result = CG_ERR_REQUEST_PENDING;
    goto httpGET_end;
  }

  tempFile.resize (0);
  request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
  request.setUrl(qurl);
  request.setRawHeader("User-Agent", header);
  if (cookie.size () > 0)
    request.setRawHeader("cookie: ", cookie.toLatin1 ());

  httpAccessManager = new QNetworkAccessManager (this);
  connect (httpAccessManager, SIGNAL (finished (QNetworkReply *)), this,
           SLOT (httpFinished (QNetworkReply *)));

  reply = httpAccessManager->get (request);
  connect (reply, SIGNAL (error (QNetworkReply::NetworkError)), this,
           SLOT (netError (QNetworkReply::NetworkError)));

  while (tcounter < (timeout * 1000))
  {
    Sleeper::msleep(static_cast <unsigned long> (invl));
    qApp->sendPostedEvents ();
    if (checkRequestStatus (qurl.toString (), result))
    {
      delRequest (qurl.toString ());
      tcounter = timeout * 1000;
    }
    tcounter += invl;
  }

  if (result != CG_ERR_OK)
    goto httpGET_end;

  // get the cookies
  if (cookies != nullptr)
  {
    QVariant cookieVar = reply->header(QNetworkRequest::SetCookieHeader);
    if (cookieVar.isValid())
      *cookies = cookieVar.value<QList<QNetworkCookie> > ();
  }

  if (tempFile.write (*replyBytes) == -1)
    result = CG_ERR_WRITE_FILE;

httpGET_end:
  delete replyBytes;

  if (reply != nullptr)
    reply->deleteLater();

  if (httpAccessManager != nullptr)
    delete httpAccessManager;

  delRequest (qurl.toString ());

  return result;
}

// callback of access manager
void
NetService::httpFinished(QNetworkReply *reply)
{
  QNetworkRequest request;
  QVariant replyStatus;
  QString redirectUrl;

  replyStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if (replyStatus.isNull() == true)
  {
    setRequestStatus (reply->url ().toString (), 1, CG_ERR_NO_DATA);
    return;
  }

  if (replyStatus.isValid () == false)
  {
    setRequestStatus (reply->url ().toString (), 1, CG_ERR_INVALID_DATA);
    return;
  }

  if (replyStatus == 200) // Ok
  {

    QByteArray *replyBytes = static_cast <QByteArray *> (getReplyBuffer (reply->url ().toString ()));

    if (replyBytes != nullptr)
      *replyBytes = reply->readAll ();
    else
    {
      setRequestStatus (reply->url ().toString (), 1,
                        CG_ERR_BUFFER_NOTFOUND);
      return;
    }
  }
  else if (replyStatus == 301) // redirect
  {
    QString url = reply->url ().toString ();

    redirectUrl =
      reply->attribute (QNetworkRequest::RedirectionTargetAttribute).toString();
    setActualUrl (url, redirectUrl);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setUrl (QUrl(redirectUrl));
    request.setRawHeader("User-Agent", header);
    reply->manager ()->get (request);

    return;
  }

  setRequestStatus (reply->url ().toString (), 1, CG_ERR_OK);
}

// receive network error code
void
NetService::netError (QNetworkReply::NetworkError code)
{
  QNetworkReply *reply = qobject_cast <QNetworkReply *> (QObject::sender());

  if (reply == nullptr)
    return;

  if (code == QNetworkReply::TimeoutError)
    setRequestStatus (reply->url ().toString (), 1, CG_ERR_NETWORK_TIMEOUT);
  else
    setRequestStatus (reply->url ().toString (), 1, CG_ERR_NETWORK);
}
