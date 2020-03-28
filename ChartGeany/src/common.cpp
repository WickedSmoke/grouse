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

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <clocale>
#include <QDesktopServices>
#include <QUrl>
#include <QtGlobal>
#include <QApplication>
#include <QtCore/qmath.h>
#include <QSysInfo>
#include <QTime>
#include <QIcon>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QTextStream>
#include <QPushButton>
#include <QObjectList>
#include <QCursor>

#include "chartapp.h"
#include "common.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include "json.hpp"

using json = nlohmann::json;
using namespace std;
#endif

#ifdef Q_OS_MAC
#include <CoreServices/CoreServices.h>
#endif

#ifdef Q_OS_LINUX
#include <sys/utsname.h>
#endif // Q_OS_LINUX

// delay nsecs
void
delay(int secs)
{
  QTime dieTime= QTime::currentTime().addSecs(secs);
  while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

// show message box
void
showMessage (const QString& message, QWidget* parent)
{
  QMessageBox::question(parent, QStringLiteral("Message"),
                        message % QStringLiteral("           "),
                        QMessageBox::Ok);
}

// show download message box
bool
showDownloadMessage ()
{
  QMessageBox *msgBox;
  QPushButton *downloadBtn, *closeBtn;
  QFont fnt;
  bool result = false;

  msgBox = new QMessageBox;
  fnt = msgBox->font ();
  fnt.setPixelSize (14);
  fnt.setFamily (DEFAULT_FONT_FAMILY);
  fnt.setWeight (QFont::DemiBold);

  msgBox->setWindowTitle (QStringLiteral ("Download new version"));
  msgBox->setWindowIcon (QIcon (QString (":/png/images/icons/PNG/cglogo.png")));
  msgBox->setFont (fnt);
  msgBox->setIcon (QMessageBox::Warning);
  msgBox->setText(QStringLiteral ("There is a new version available for download"));
  downloadBtn = msgBox->addButton ("Download", QMessageBox::AcceptRole);
  closeBtn = msgBox->addButton (QStringLiteral ("Close"), QMessageBox::RejectRole);
  msgBox->setStyleSheet (QStringLiteral ("background: transparent; background-color:white;"));
  correctWidgetFonts (msgBox);
  msgBox->exec ();
  if (msgBox->clickedButton() == static_cast <QAbstractButton *> (downloadBtn))
    result = true;
  else if (msgBox->clickedButton() ==  static_cast <QAbstractButton *>  (closeBtn))
    result = false;

  delete msgBox;
  return result;
}

// show Ok/Cancel message box
bool
showOkCancel (const QString& message, QWidget* parent)
{
  QMessageBox::StandardButton btn;
  btn = QMessageBox::question(parent, QStringLiteral("Question"),
                            message,
                            QMessageBox::Ok | QMessageBox::Cancel,
                            QMessageBox::Cancel);
  return( btn == QMessageBox::Ok );
}

// full operating system description
extern QString
fullOperatingSystemVersion ()
{
  QString full, os, ver = "", desc = "";

#ifdef Q_OS_WIN32
  os = "Microsoft Windows ";
  ver = ">10 ";
  switch (QSysInfo::WindowsVersion)
  {
  case QSysInfo::WV_NT:
    ver = "NT ";
    break;
  case QSysInfo::WV_2000:
    ver = "2000 ";
    break;
  case QSysInfo::WV_XP:
    ver = "XP ";
    break;
  case QSysInfo::WV_2003:
    ver = "2003 ";
    break;
  case QSysInfo::WV_VISTA:
    ver = "Vista ";
    break;
  case QSysInfo::WV_WINDOWS7:
    ver = "7 ";
    break;
  case QSysInfo::WV_WINDOWS8:
    ver = "8 ";
    break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
  case QSysInfo::WV_WINDOWS8_1:
    ver = "8.1 ";
    break;
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
  case QSysInfo::WV_WINDOWS10:
    ver = "10 ";
    break;
#endif
  }
#endif // Q_OS_WIN32

#ifdef Q_OS_MAC
  SInt32 majorVersion,minorVersion,bugFixVersion;

  os = "Mac OS X ";
  switch (QSysInfo::MacintoshVersion)
  {
  case QSysInfo::MV_CHEETAH:
    ver = "10.0 Cheetah ";
    break;
  case QSysInfo::MV_PUMA:
    ver = "10.1 Puma ";
    break;
  case QSysInfo::MV_JAGUAR:
    ver = "10.2 Jaguar ";
    break;
  case QSysInfo::MV_PANTHER:
    ver = "10.3 Panther ";
    break;
  case QSysInfo::MV_TIGER:
    ver = "10.4 Tiger ";
    break;
  case QSysInfo::MV_LEOPARD:
    ver = "10.5 Leopard ";
    break;
  case QSysInfo::MV_SNOWLEOPARD:
    ver = "10.6 Snow Leopard ";
    break;
  case QSysInfo::MV_LION:
    ver = "10.7 Lion ";
    break;
  case QSysInfo::MV_MOUNTAINLION:
    ver = "10.8 Mountain Lion ";
    break;
  default:
    Gestalt(gestaltSystemVersionMajor, &majorVersion);
    Gestalt(gestaltSystemVersionMinor, &minorVersion);
    Gestalt(gestaltSystemVersionBugFix, &bugFixVersion);
    ver = QString::number ((int) majorVersion) % "." %
          QString::number ((int) minorVersion) % " ";
  }
#endif // Q_OS_MAC

#ifdef Q_OS_LINUX
  struct utsname unixname;
  if (uname (&unixname) >= 0)
  {
    os = QString (unixname.sysname) % " ";
    ver = QString (unixname.release) % " ";
  }
#endif // Q_OS_LINUX

  full = os + ver + desc;
  return full;
}

// symbol's data frames callback
int
sqlcb_dataframes (void *vectorptr, int argc, char **argv, char **column)
{
  static const char
  open[16] = "OPEN",
  high[16] = "HIGH",
  low[16] = "LOW",
  close[16] = "CLOSE",
  adjclose[16] = "ADJCLOSE",
  volume[16] = "VOLUME",
  date[16] = "DATE",
  time[16] = "TIME";

  QTAChartFrame Frame;
  FrameVector *VFrame;

  if (vectorptr == nullptr)
    return 1;

  setlocale(LC_ALL, "C");
  VFrame = static_cast <FrameVector *> (vectorptr);

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    char *colname = strtoupper (column[counter]);
    if (!strcmp (colname, open))
      Frame.Open = strtod (argv[counter], nullptr);
    else if (!strcmp (colname, high))
      Frame.High = strtod (argv[counter], nullptr);
    else if (!strcmp (colname, low))
      Frame.Low = strtod (argv[counter], nullptr);
    else if (!strcmp (colname, close))
      Frame.Close = strtod (argv[counter], nullptr);
    else if (!strcmp (colname, adjclose))
      Frame.AdjClose = strtod (argv[counter], nullptr);
    else if (!strcmp (colname, volume))
      Frame.Volume = strtod (argv[counter], nullptr);
    else if (!strcmp (colname, date))
      strcpy (Frame.Date, argv[counter]);
    else if (!strcmp (colname, time))
      strcpy (Frame.Time, argv[counter]);
  }

  *VFrame << Frame;
  return 0;
}

// symbol's fundamentals callback
int
sqlcb_fundamentals (void *data, int argc, char **argv, char **column)
{
  QTAChartData *Data;

  Data = static_cast <QTAChartData*> (data);

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString (column[counter]).toLower ();

    if (colname == QLatin1String ("bv"))
      Data->bv = QString (argv[counter]);
    else if (colname == QLatin1String ("mc"))
      Data->mc = QString (argv[counter]);
    else if (colname == QLatin1String ("ebitda"))
      Data->ebitda = QString (argv[counter]);
    else if (colname == QLatin1String ("pe"))
      Data->pe = QString (argv[counter]);
    else if (colname == QLatin1String ("peg"))
      Data->peg = QString (argv[counter]);
    else if (colname == QLatin1String ("dy"))
      Data->dy = QString (argv[counter]);
    else if (colname == QLatin1String ("epscurrent"))
      Data->epscurrent = QString (argv[counter]);
    else if (colname == QLatin1String ("epsnext"))
      Data->epsnext = QString (argv[counter]);
    else if (colname == QLatin1String ("es"))
      Data->es = QString (argv[counter]);
    else if (colname == QLatin1String ("ps"))
      Data->ps = QString (argv[counter]);
    else if (colname == QLatin1String ("pbv"))
      Data->pbv = QString (argv[counter]);
  }

  return 0;
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
      options->showvolume = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("LINEAR"))
      options->linear = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("GRID"))
      options->showgrid = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("CHARTSTYLE"))
      options->chartstyle = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("ONLINEPRICE"))
      options->showonlineprice = QString (argv[counter]).toShort ();
    else
    if (colname == QLatin1String ("LINECOLOR"))
      options->linecolor =
        QColor ((QRgb) QString (argv[counter]).toULongLong ());
    else
    if (colname == QLatin1String ("BARCOLOR"))
      options->barcolor =
        QColor ((QRgb) QString (argv[counter]).toULongLong ());
    else
    if (colname == QLatin1String ("FORECOLOR"))
      options->forecolor =
        QColor ((QRgb) QString (argv[counter]).toULongLong ());
    else
    if (colname == QLatin1String ("BACKCOLOR"))
      options->backcolor =
        QColor ((QRgb) QString (argv[counter]).toULongLong ());
  }
  return 0;
}

// nsymbols callback
int
sqlcb_nsymbols(void *nsymptr, int argc, char **argv, char **column)
{
  Q_UNUSED (argc)
  Q_UNUSED (column)

  int nsymbols;

  nsymbols = QString (argv[0]).toInt ();
  *(int *) nsymptr = nsymbols;

  return 0;
}

// ticker symbols callback
int
sqlcb_tickersymbols (void *data, int argc, char **argv, char **column)
{
  QStringList *symbols, s;
  QString colname;

  symbols = static_cast <QStringList *> (data);

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString (column[counter]).toLower ();

    if (colname == QLatin1String ("symbol"))
      s.append (QString (argv[counter]));
  }

  *symbols += s;
  return 0;
}

// ticker feed call back
int
sqlcb_tickerfeed (void *data, int argc, char **argv, char **column)
{
  QStringList *feed;
  QString colname;

  feed = static_cast <QStringList *> (data);

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString (column[counter]).toLower ();

    if (colname == QLatin1String ("feed") || colname == QLatin1String ("datafeed"))
      (*feed).append (QString (argv[counter]));
  }

  return 0;
}

// load ticker symbols
CG_ERR_RESULT
loadTickerSymbols (QStringList & symbol, QStringList & feed)
{
  QString query;
  int rc;

  query = QStringLiteral ("SELECT SYMBOL FROM TICKER_SYMBOLS ORDER BY SYMBOL;");
  rc = selectfromdb(query.toUtf8(), sqlcb_tickersymbols,
                    static_cast <void *> (&symbol));
  if (rc == SQLITE_OK)
  {
    query = QStringLiteral ("SELECT FEED FROM TICKER_SYMBOLS ORDER BY SYMBOL;");
    rc = selectfromdb(query.toUtf8(), sqlcb_tickerfeed,
                      static_cast <void *> (&feed));
    if (rc != SQLITE_OK)
    {
      symbol.clear ();
      return CG_ERR_DBACCESS;
    }
  }
  else
    return CG_ERR_DBACCESS;

  return CG_ERR_OK;
}

// load portfolio symbols
CG_ERR_RESULT
loadPortfolioSymbols (QStringList &symbol, QStringList & feed, int pfid)
{
  QString query, table;
  QStringList wsymbol, wfeed;
  int rc;

  table = QStringLiteral ("pftrans_") % QString::number (pfid) % QStringLiteral ("summary");
  query = QStringLiteral ("SELECT SYMBOL FROM ") % table % QStringLiteral (" WHERE QUANTITY <> 0 ORDER BY SYMBOL;");
  rc = selectfromdb(query.toUtf8(), sqlcb_tickersymbols,
                    static_cast <void *> (&wsymbol));
  if (rc == SQLITE_OK)
  {
    query += QStringLiteral ("SELECT DATAFEED FROM ") % table % QStringLiteral (" WHERE QUANTITY <> 0 ORDER BY SYMBOL;");
    rc = selectfromdb(query.toUtf8(), sqlcb_tickerfeed,
                      static_cast <void *> (&wfeed));
    if (rc != SQLITE_OK)
    {
      symbol.clear ();
      return CG_ERR_DBACCESS;
    }
  }
  else
    return CG_ERR_DBACCESS;

  symbol = wsymbol;
  feed = wfeed;

  return CG_ERR_OK;
}


// load ticker symbols
CG_ERR_RESULT
saveTickerSymbols (QStringList & symbol, QStringList & feed)
{
  QString query;
  int rc;

  query = QStringLiteral ("DELETE FROM TICKER_SYMBOLS;");
  for (qint32 counter = 0; counter < symbol.size (); counter ++)
  {
    query += QStringLiteral ("INSERT INTO TICKER_SYMBOLS (SYMBOL, FEED) VALUES ('") %
             symbol[counter] % QStringLiteral ("','") % feed[counter] % QStringLiteral ("');");
  }
  query.append ('\n');

  rc = updatedb (query);
  if (rc != SQLITE_OK)
    return CG_ERR_DBACCESS;

  return CG_ERR_OK;
}

// load application options
CG_ERR_RESULT
loadAppOptions (AppOptions *options)
{
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
CG_ERR_RESULT
saveAppOptions (AppOptions *options)
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
   QStringLiteral (", linecolor = ") % QString::number ((qreal) options->linecolor.rgb (), 'f', 0) %
   QStringLiteral (", barcolor = ") % QString::number ((qreal) options->barcolor.rgb (), 'f', 0) %
   QStringLiteral (", backcolor = ") % QString::number ((qreal) options->backcolor.rgb (), 'f', 0) %
   QStringLiteral (", forecolor = ") % QString::number ((qreal) options->forecolor.rgb (), 'f', 0);

  if (options->showvolume)
    query += QStringLiteral (", volume = 1");
  else
    query += QStringLiteral (", volume = 0");

  if (options->linear)
    query += QStringLiteral (", linear = 1");
  else
    query += QStringLiteral (", linear = 0");

  query += QStringLiteral (", chartstyle = ") % QString::number (options->chartstyle, 'f', 0);

  if (options->showonlineprice)
    query += QStringLiteral (", onlineprice = 1");
  else
    query += QStringLiteral (", onlineprice = 0");

  if (options->showgrid)
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

/*
 * unused. keep it here for possible future use
 *
static unsigned short
checksum16 (const char *data, int len)
{
  unsigned int sum = 0;
  if ((len & 1) == 0)
    len = len >> 1;
  else
    len = (len >> 1) + 1;
  while (len > 0)
  {
    sum += *((unsigned short *) data);
    data += sizeof (unsigned short);
    len--;
  }
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return (~sum);
}
*/

// corrent font size
static void
correctFontSize (QWidget *widget)
{
  QFont fnt;

  fnt = widget->font ();

  fnt.setFamily (DEFAULT_FONT_FAMILY);
  widget->setFont (fnt);
  fnt = widget->font ();
  if (fnt.pointSize () != -1)
    fnt.setPointSize (fnt.pointSize () + FONT_POINTSIZE_PAD);
  else
    fnt.setPixelSize (fnt.pixelSize () + FONT_PIXELSIZE_PAD);

  widget->setFont (fnt);
}

// correct font size for widget and children
void
correctWidgetFonts (QWidget *widget)
{
  QList<QWidget *> allWidgets = widget->findChildren<QWidget *> ();

  foreach (QWidget *wid, allWidgets)
    correctFontSize (wid);

  correctFontSize (widget);
}

void
correctWidgetFonts (QDialog *widget)
{
  QList<QWidget *> allWidgets = widget->findChildren<QWidget *> ();

  foreach (QWidget *wid, allWidgets)
    correctFontSize (wid);

  correctFontSize (widget);
}

void
correctWidgetFonts (QMessageBox *widget)
{
  QList<QWidget *> allWidgets = widget->findChildren<QWidget *> ();

  foreach (QWidget *wid, allWidgets)
    correctFontSize (wid);

  correctFontSize (widget);
}

// correct the fonts of a button in a button box
void
correctButtonBoxFonts (QDialogButtonBox *box,
                       QDialogButtonBox::StandardButton button)
{
  QFont font;
  QPushButton *btn;

  btn = box->button (button);
  btn->setFocusPolicy (Qt::NoFocus);
  // correctFontSize (btn);
  font = btn->font ();
  font.setWeight (QFont::DemiBold);
  btn->setFont (font);

  return;
}

// corect title bar of QDialog
void
correctTitleBar (QDialog *dialog)
{
  Qt::WindowFlags flags = dialog->windowFlags();
  Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
  flags = flags & (~helpFlag);
  dialog->setWindowFlags(flags);
  dialog->setWindowFlags (((dialog->windowFlags() | Qt::CustomizeWindowHint)
                           & ~Qt::WindowCloseButtonHint));
}

// native http header
QString
nativeHttpHeader ()
{
  QString header;

  header.reserve (4096);
  header = QString (APPNAME) % QStringLiteral ("/") %
           QString::number (VERSION_MAJOR) % QStringLiteral (".") %
           QString::number (VERSION_MINOR) % QStringLiteral (".") %
           QString::number (VERSION_PATCH) % QStringLiteral (" ") %
           fullOperatingSystemVersion () % QStringLiteral (" ") %
           QString::number (QT_POINTER_SIZE*8) % QStringLiteral (" ") %
           RunCounter % QStringLiteral (" ") % UID % QStringLiteral (" ") %
           Application_Options->pak % QStringLiteral (" ");

  return header;
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

static bool
json_parse_qt5 (QString jsonstr, QStringList *node, QStringList *value, void *n1)
{
  QJsonObject *n = nullptr;
  bool result = true, allocated = false;

  if (n1 == nullptr)
  {
    QJsonDocument doc;
    QJsonParseError err;

    node->clear ();
    value->clear ();

    jsonstr = jsonstr.trimmed ();
    doc = QJsonDocument::fromJson(jsonstr.toUtf8(), &err);
    if (err.error !=  QJsonParseError::NoError)
    {
      result = false;
      goto json_parse_qt5_end;
    }

    n = new QJsonObject;
    allocated = true;
    *n = doc.object ();
  }
  else
    n = static_cast <QJsonObject *> (n1);

  for (qint32 counter = 0; counter < (*n).keys ().size (); counter ++)
  {
    if ((*n).value ((*n).keys ().at (counter)).type () == QJsonValue::Array)
    {
      QJsonArray arr = (*n).value ((*n).keys ().at (counter)).toArray ();
      foreach (const QJsonValue it, arr)
      {
        QJsonObject n2 = it.toObject ();
        json_parse_qt5 (jsonstr, node, value, static_cast <void *> (&n2));
      }
    }

    if ((*n).value ((*n).keys ().at (counter)).type () == QJsonValue::Object)
    {
      QJsonObject n2 = (*n).value ((*n).keys ().at (counter)).toObject ();

      if (!json_parse_qt5 (jsonstr, node, value, static_cast <void *> (&n2)))
      {
        result = false;
        goto json_parse_qt5_end;
      }
    }

    node->append ((*n).keys ().at (counter));

    QString nodeval = "";
    if ((*n).value ((*n).keys ().at (counter)).type () == QJsonValue::Double)
    {
      double d = (*n).value ((*n).keys ().at (counter)).toDouble ();
      nodeval = QString("%1").arg(d, 0, 'f', -1).simplified ();
    }
    else
      nodeval = (*n).value ((*n).keys ().at (counter)).toString ();

    value->append (nodeval);
  }

json_parse_qt5_end:
  if (allocated) delete n;
  return result;
}
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

bool
json_parse (QString jsonstr, QStringList *node, QStringList *value, void *n1)
{
  Q_UNUSED (n1)

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  return json_parse_qt5 (jsonstr, node, value, nullptr);
#else // QT_VERSION < QT_VERSION_CHECK(5, 0, 0)

  json j;
  try
  {
    j = json::parse(jsonstr.toUtf8());
  }
  catch(json::parse_error)
  {
    return false;
  }

  string cpp_string = j.dump(0);
  QString qjs = QString::fromStdString (cpp_string);
  qjs.replace ("{", "");
  qjs.replace ("}", "");
  qjs.replace ("[", "");
  qjs.replace ("]", "");
  qjs.replace (QChar ('"'), "");
  qjs.replace (",", " ");
  QStringList Nodes = qjs.split(QRegExp("\\n"));
  foreach (QString Node, Nodes)
  {
    Node = Node.trimmed ();
    if (Node.contains (":"))
    {
      QStringList nvpair = Node.split (":");
      node->append (nvpair[0].trimmed ());
      value->append (nvpair[1].trimmed ());
    }
  }

  return true;
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
}

// object's family tree of descendants
QObjectList
familyTree (QObject *obj)
{
  QObjectList list;

  foreach (QObject *object, obj->children ())
  {
    list += object;
    if (object->children ().size () > 0)
      list += familyTree (object);
  }

  return list;
}

// return number of significant digits after the decimal point: Qt/C++ implementation
/*
GNUFASTCALL qint32 MSVCFASTCALL
fracdig (qreal r) GNUHOT
{
  double f, i;
  char *dot;
  qint32 frac;
  char fs[128];

  f = modf ((double) r, &i);
  if (f == 0)
    return 0;

  sprintf (fs, "%.6g", f);
  dot = &fs[2];

  frac = 0;
  while (*dot++ != 0)
    frac ++;

  if (r > 1)
  {
    if (frac < 3)
      return frac;

    return 2;
  }

  if (frac < 5)
    return frac;

  return 4;
}
*/

/*
GNUFASTCALL qint32 MSVCFASTCALL
fracdig (qreal r)
{
  char *dot;
  qint32 frac;
  char fs[128];

  sprintf (fs, "%.10g", r);
  dot = fs;

  while (*dot != 0 && *dot != '.')
    dot ++;

  if (*dot == 0)
    return 0;

  frac = 0;
  while (*++dot != 0)
    frac ++;

  if (r > 1)
  {
    if (frac < 3)
      return frac;

    return 2;
  }

  if (frac < 5)
    return frac;

  return 4;
}
*/

// convert nullptr terminated string to upper case
GNUFASTCALL char * MSVCFASTCALL
strtoupper (char *str)
{
  static char *s;
  static const char d = 32;

  s = str;
  while (*s)
  {
    if ((*s >= 'a' ) && (*s <= 'z'))
      *s -= d;
    s++;
  }

  return str;
}

// override and restore cursor
void
appSetOverrideCursor (const QWidget *wid, const QCursor & cursor)
{
  // ((QWidget *) wid)->setCursor (cursor);
  Q_UNUSED (wid)
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  QApplication::setOverrideCursor (cursor);
#else
  QGuiApplication::setOverrideCursor (cursor);
#endif
}

// restore application's cursor
void
appRestoreOverrideCursor (const QWidget *wid)
{
  // ((QWidget *) wid)->setCursor (QCursor(Qt::ArrowCursor));
  Q_UNUSED (wid)
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  QApplication::restoreOverrideCursor ();
#else
  QGuiApplication::restoreOverrideCursor ();
#endif
}

#ifdef Q_CC_MSVC
#include <windows.h>
#include <assert.h>

typedef BOOL (WINAPI *SetProcessDPIAwarePtr)(VOID);

INT APIENTRY DllMain(HMODULE hDLL, DWORD reason, LPVOID reserved)
{
  Q_UNUSED (reserved);
  Q_UNUSED (hDLL);
  if (reason == DLL_PROCESS_ATTACH )
  {
    // Make sure we're not already DPI aware
    assert( !IsProcessDPIAware() );

    // First get the DPIAware function pointer
    SetProcessDPIAwarePtr lpDPIAwarePointer = (SetProcessDPIAwarePtr)
        GetProcAddress(GetModuleHandle((LPCWSTR) "user32.dll"),
                       "SetProcessDPIAware");

    // Next make the page writeable so that we can change the function assembley
    DWORD oldProtect;
    VirtualProtect((LPVOID)lpDPIAwarePointer, 1, PAGE_EXECUTE_READWRITE, &oldProtect);

    // write "ret" as first assembly instruction to avoid actually setting HighDPI
    BYTE newAssembly[] = {0xC3};
    memcpy(lpDPIAwarePointer, newAssembly, sizeof(newAssembly));

    // change protection back to previous setting.
    VirtualProtect((LPVOID)lpDPIAwarePointer, 1, oldProtect, nullptr);
  }
  return TRUE;
}
#endif // Q_CC_MSVC

#include "create_portfolio_views.cpp"
