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

#ifndef CHARTAPP_H
#define CHARTAPP_H

#include <QtGui>
#include <QApplication>
#include <QMutex>

class ChartApp : public QApplication
{
  Q_OBJECT

public:
  ChartApp(int& argc, char ** argv) ;
  ~ChartApp();

  void moduleLock (QObject *obj);
  void moduleUnlock (QObject *obj);
  void ioLock ();
  void ioUnlock ();
  bool ioTrylock ();

private:
  QObject *lockholder;  // the object that holds the lock
  QMutex *modmutex;     // mutex for modules
  QMutex *iomutex;      // mutex for multithreaded use of sqlite
};


#include <QProxyStyle>

class AppProxyStyle : public QProxyStyle
{
  Q_OBJECT

public:
  explicit AppProxyStyle (QStyle * style = 0): QProxyStyle (style)
  {
    setBaseStyle (QApplication::style ());
  };
  ~AppProxyStyle () {};
  virtual int
  styleHint(StyleHint hint, const QStyleOption *option = 0,
            const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const
  {
    if (hint == QStyle::SH_ComboBox_Popup)
      return 0;
    return QProxyStyle::styleHint(hint, option, widget, returnData);
  }
};


#endif // CHARTAPP_H
