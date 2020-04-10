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

#ifndef NETSERVICE_H
#define NETSERVICE_H

#include <QObject>
#include <QAtomicInt>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkCookie>
#include "chartapp.h"

// network request structure
struct NetRequestClass
{
  QString url;			// the url
  QString actualUrl;	// the actual url (eg after redirect)
  QAtomicInt finished;	// operation status: 0 false/finished, 1 true/running
  void *replyBytes; // network reply buffer
  CG_ERR_RESULT error;	// error
};

Q_DECLARE_TYPEINFO (NetRequestClass, Q_MOVABLE_TYPE);
typedef QList <NetRequestClass *> NetRequestVector;
typedef QList <QNetworkCookie> Cookies;

namespace Ui
{
  class NetService;
}

class NetService: public QObject
{
  Q_OBJECT
  
public:
  static void applyProxyOptions (const AppOptions*);

  explicit NetService (qint16  nettimeout, const QByteArray& httpHeader, QObject *parent = 0); // constructor
  ~NetService (void);	     				 // destructor

  CG_ERR_RESULT httpGET (QString url, QFile & tempFile, Cookies *cookies); // downloads the url content
  // in tempFile
  
  void setCookie (QString s) { cookie = s; }; // set's the cookie for http header

private:
  NetRequestVector Request;			// list of all requests
  QByteArray header;				// http header
  QString cookie;					// cookie for http header
  qint16  timeout;					// network timeout in seconds

  bool addRequest (QString url, void *replyBuffer); // add request in vector
  // returns true on success, false on failure
  int  checkRequestStatus (QString url, int & error);  // check request's status
  void *getReplyBuffer (QString url); // get the reply buffer;
  void setRequestStatus (QString url, int status, int error); // set the status of a request
  void setActualUrl (QString url, QString aurl); // set the actual url
  void delRequest (QString url); // remove request from vector

private slots:
  void httpFinished (QNetworkReply *checkreply); // callback of access manager
  void netError (QNetworkReply::NetworkError code); // receive network error code
};

#endif
