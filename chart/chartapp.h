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

#include <QApplication>
#include "QTAChartData.h"
#include "idb.h"

// Application options structure
struct AppOptions
{
  QTAChartProperties chart;
  QString pak;                  // program activation key
  QString avapikey;             // alpha vantage api key
  QString iexapikey;            // iex api key
  QString proxyhost;            // proxy host name or IP
  QString proxyuser;            // proxy user name
  QString proxypass;            // proxy password
  QString platform;             // platform string
  QString compiler;             // compiler path
  QString compilerdbg;          // compiler debug options
  QString compilerrel;          // compiler release options
  QString linker;               // linker path
  QString linkerdbg;            // linker debug options
  QString linkerrel;            // linker release options
  qint16  nettimeout;           // network timeout in seconds
  qint16  proxyport;            // proxy port
  qint16  scrollspeed;          // ticker's scroll speed
  bool showsplashscreen;        // show splash screen
  bool checknewversion;         // check new version
  bool enableproxy;             // enable proxy
  bool longbp;                  // convert london prices to gbp (divide by 100)
  bool autoupdate;              // default setting for auto update quotes on chart opening
  bool devmode;                 // default setting for developer mode
};


class QMutex;

class ChartApp : public QApplication
{
  Q_OBJECT

public:
  ChartApp(int& argc, char ** argv) ;
  ~ChartApp();

  bool openDatabase ();
  void moduleLock (QObject *obj);
  void moduleUnlock (QObject *obj);
  CG_ERR_RESULT loadOptions ();

private:
  InstrumentDatabase idb;
  QObject *lockholder;  // the object that holds the lock
  QMutex *modmutex;     // mutex for modules
};

extern AppOptions *Application_Options;
extern SQLists *ComboItems;     // QStringLists used as combo box items and more
extern QMutex *ResourceMutex;   // mutex to protect shared resources
extern int NCORES;              // number of active cores

CG_ERR_RESULT saveAppOptions (AppOptions *options);

// show a message box
extern void showMessage (const QString& message, QWidget* parent = nullptr);

// show an Ok/Cancel question box
extern bool showOkCancel (const QString& message, QWidget* parent = nullptr);


#endif // CHARTAPP_H
