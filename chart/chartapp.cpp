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

#include <QDir>
#include <QMutex>
#include <QMessageBox>
#include <QThread>
#include "chartapp.h"
#include "defs.h"
#include "idb.h"


static AppOptions _options;
AppOptions *Application_Options = &_options;

static SQLists _sqLists;
SQLists *ComboItems = &_sqLists;

QMutex *ResourceMutex = nullptr;

int NCORES;


ChartApp::ChartApp (int & argc, char **argv): QApplication(argc, argv)
{
  modmutex = new QMutex;
  ResourceMutex = new QMutex(QMutex::NonRecursive);

#ifdef Q_OS_MAC
  NCORES = 1;
#else
  NCORES = QThread::idealThreadCount();
  if (NCORES == -1)
    NCORES = 1;
#endif
}

ChartApp::~ChartApp ()
{
  delete modmutex;
  delete ResourceMutex;
}


// sqlite3_exec callback for retrieving application options
static int
sqlcb_options (void *classptr, int argc, char **argv, char **column)
{
  AppOptions *options = static_cast <AppOptions *> (classptr);

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString (column[counter]).toUpper ();

    if (colname == QLatin1String ("PAK"))
      options->pak = QString (argv[counter]);
    else
    if (colname == QLatin1String ("AVAPIKEY"))
      options->avapikey = QString (argv[counter]);
    else
    if (colname == QLatin1String ("IEXAPIKEY"))
      options->iexapikey = QString (argv[counter]);
    else
    if (colname == QLatin1String ("SHOWSPLASHSCREEN"))
    {
      if (atoi (argv[counter]) == 1)
        options->showsplashscreen = true;
      else
        options->showsplashscreen = false;
    }
    else
    if (colname == QLatin1String ("AUTOUPDATE"))
    {
      if (atoi (argv[counter]) == 1)
        options->autoupdate = true;
      else
        options->autoupdate = false;
    }
    else
    if (colname == QLatin1String ("DEVMODE"))
    {
      if (atoi (argv[counter]) == 1)
        options->devmode = true;
      else
        options->devmode = false;
    }
    else
    if (colname == QLatin1String ("CHECKNEWVERSION"))
    {
      if (atoi (argv[counter]))
        options->checknewversion = true;
      else
        options->checknewversion = false;
    }
    else
    if (colname == QLatin1String ("LONGBP"))
    {
      if (atoi (argv[counter]))
        options->longbp = true;
      else
        options->longbp = false;
    }
    else
    if (colname == QLatin1String ("ENABLEPROXY"))
    {
      if (atoi (argv[counter]))
        options->enableproxy = true;
      else
        options->enableproxy = false;
    }
    else
    if (colname == QLatin1String ("PROXYHOST"))
      options->proxyhost = QString (argv[counter]);
    else
    if (colname == QLatin1String ("PROXYUSER"))
      options->proxyuser = QString (argv[counter]);
    else
    if (colname == QLatin1String ("PROXYPASS"))
      options->proxypass = QString (argv[counter]);
    else
    if (colname == QLatin1String ("PROXYPORT"))
      options->proxyport = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("NETTIMEOUT"))
      options->nettimeout = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("SCROLLSPEED"))
      options->scrollspeed = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("VOLUME"))
      options->chart.showVolume = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("LINEAR"))
      options->chart.linearScale = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("GRID"))
      options->chart.showGrid = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("CHARTSTYLE"))
      options->chart.style = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("ONLINEPRICE"))
      options->chart.showOnlinePrice = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("LINECOLOR"))
      options->chart.lineColor = QString (argv[counter]).toULongLong ();
    else
    if (colname == QLatin1String ("BARCOLOR"))
      options->chart.barColor = QString (argv[counter]).toULongLong ();
    else
    if (colname == QLatin1String ("FORECOLOR"))
      options->chart.foreColor = QString (argv[counter]).toULongLong ();
    else
    if (colname == QLatin1String ("BACKCOLOR"))
      options->chart.backColor = QString (argv[counter]).toULongLong ();
  }
  return 0;
}


// load application options
CG_ERR_RESULT ChartApp::loadOptions ()
{
  AppOptions *options = &_options;
  QString query, platform = platformString ();
  int rc;

  if (options->platform.size () > 0)
    platform = options->platform;

  query = QStringLiteral ("SELECT * FROM toolchains WHERE platform = '") %
          platform % QStringLiteral ("';");
  rc = selectfromdb(query.toUtf8(), sqlcb_toolchain, options);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  }

  query = QStringLiteral ("SELECT * FROM options WHERE recid = 1;");
  rc = selectfromdb(query.toUtf8(), sqlcb_options, options);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  }

  return CG_ERR_OK;
}


// save application options
CG_ERR_RESULT saveAppOptions (AppOptions *options)
{
  QString query;
  int rc;

  query = QStringLiteral ("UPDATE options SET ");
  query.append ('\n');
  query += QStringLiteral ("pak = '") % options->pak + QStringLiteral ("'");
  query += QStringLiteral (", avapikey = '") % options->avapikey + QStringLiteral ("'");
  query += QStringLiteral (", iexapikey = '") % options->iexapikey + QStringLiteral ("'");

  if (options->showsplashscreen)
    query += QStringLiteral (", showsplashscreen = 1 ");
  else
    query += QStringLiteral (", showsplashscreen = 0 ");

  if (options->autoupdate)
    query += QStringLiteral (", autoupdate = 1 ");
  else
    query += QStringLiteral (", autoupdate = 0 ");

  if (options->devmode)
    query += QStringLiteral (", devmode = 1 ");
  else
    query += QStringLiteral (", devmode = 0 ");

  if (options->checknewversion)
    query += QStringLiteral (", checknewversion = 1 ");
  else
    query += QStringLiteral (", checknewversion = 0 ");

  if (options->enableproxy)
    query += QStringLiteral (", enableproxy = 1 ");
  else
    query += QStringLiteral (", enableproxy = 0 ");

  if (options->longbp)
    query += QStringLiteral (", longbp = 1 ");
  else
    query += QStringLiteral (", longbp = 0 ");

  query += QStringLiteral (", proxyhost = '") % options->proxyhost % QStringLiteral ("'") %
   QStringLiteral (", proxyuser = '") % options->proxyuser % QStringLiteral ("'") %
   QStringLiteral (", proxypass = '") % options->proxypass % QStringLiteral ("'");
  query.append ('\n');
  query += QStringLiteral (", proxyport = ") % QString::number (options->proxyport) %
   QStringLiteral (", nettimeout = ") % QString::number (options->nettimeout) %
   QStringLiteral (", scrollspeed = ") % QString::number (options->scrollspeed) %
   QStringLiteral (", linecolor = ") %
   QString::number ((qreal) options->chart.lineColor, 'f', 0) %
   QStringLiteral (", barcolor = ") %
   QString::number ((qreal) options->chart.barColor, 'f', 0) %
   QStringLiteral (", backcolor = ") %
   QString::number ((qreal) options->chart.backColor, 'f', 0) %
   QStringLiteral (", forecolor = ") %
   QString::number ((qreal) options->chart.foreColor, 'f', 0);

  if (options->chart.showVolume)
    query += QStringLiteral (", volume = 1");
  else
    query += QStringLiteral (", volume = 0");

  if (options->chart.linearScale)
    query += QStringLiteral (", linear = 1");
  else
    query += QStringLiteral (", linear = 0");

  query += QStringLiteral (", chartstyle = ") %
      QString::number (options->chart.style, 'f', 0);

  if (options->chart.showOnlinePrice)
    query += QStringLiteral (", onlineprice = 1");
  else
    query += QStringLiteral (", onlineprice = 0");

  if (options->chart.showGrid)
    query += QStringLiteral (", grid = 1");
  else
    query += QStringLiteral (", grid = 0");

  query += QStringLiteral (" WHERE recid = 1;");

  query.append ('\n');
  query += QStringLiteral ("UPDATE toolchains SET ") %
   QStringLiteral ("compiler = '") % options->compiler % QStringLiteral ("', ") %
   QStringLiteral ("compilerdbg = '") % options->compilerdbg % QStringLiteral ("', ") %
   QStringLiteral ("compilerrel = '") % options->compilerrel % QStringLiteral ("', ") %
   QStringLiteral ("linker = '") % options->linker % QStringLiteral ("', ") %
   QStringLiteral ("linkerdbg = '") % options->linkerdbg % QStringLiteral ("', ") %
   QStringLiteral ("linkerrel = '") % options->linkerrel % QStringLiteral ("' ") %
   QStringLiteral ("WHERE platform = '") % options->platform % QStringLiteral ("';");

  rc = updatedb (query);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  }

  return CG_ERR_OK;
}


bool ChartApp::openDatabase ()
{
  const char* openError;
  QString fn = QDir::homePath() % QDir::separator() %
      QStringLiteral(".config") % QDir::separator() % APPDIR %
      QDir::separator() % DBNAME;

  if( ! idb.openFile( fn, &openError ) )
  {
    showMessage( QString(openError).append(" Application quits.") );
    return false;
  }

  idb.initializeListQueries( _sqLists );
  return true;
}


void ChartApp::moduleLock (QObject *obj)
{
  modmutex->lock ();
  lockholder = obj;
}


void ChartApp::moduleUnlock (QObject *obj)
{
  Q_UNUSED (obj)

  modmutex->unlock ();
  lockholder = nullptr;
}


// show message box
void showMessage (const QString& message, QWidget* parent)
{
  QMessageBox::question(parent, QStringLiteral("Message"),
                        message % QStringLiteral("           "),
                        QMessageBox::Ok);
}

// show Ok/Cancel message box
bool showOkCancel (const QString& message, QWidget* parent)
{
  QMessageBox::StandardButton btn;
  btn = QMessageBox::question(parent, QStringLiteral("Question"),
                            message,
                            QMessageBox::Ok | QMessageBox::Cancel,
                            QMessageBox::Cancel);
  return( btn == QMessageBox::Ok );
}
