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

#ifndef QTCWEBENGINE_H
#define QTCWEBENGINE_H

#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
#include <QWebView>
#else
#include <QDesktopServices>
#include <QWebEngineView>

class QTCWebPage : public QWebEnginePage
{
  Q_OBJECT

public:
  explicit QTCWebPage(QObject* parent = 0) : QWebEnginePage(parent) {}
  bool
  acceptNavigationRequest(const QUrl & url, QWebEnginePage::NavigationType type, bool isMainFrame)
  {
    Q_UNUSED (isMainFrame);
    if (type == QWebEnginePage::NavigationTypeLinkClicked)
    {
      QDesktopServices::openUrl(url);
      return false;
    }
    return true;
  }
};

class QTCWebEngineView : public QWebEngineView
{
  Q_OBJECT
public:
  explicit QTCWebEngineView (QWidget *parent) : QWebEngineView (parent)
  {
    wp = new QTCWebPage (this);
    setPage(wp);
  }

  ~QTCWebEngineView ()
  {

  }

private:
  QTCWebPage *wp;
};

#endif // QT_VERSION < QT_VERSION_CHECK(5, 7, 0)

#endif // QTCWEBENGINE_H
