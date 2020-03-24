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

#ifndef COMMON_H
#define COMMON_H

// Qt, third party and system headers
#include <math.h>
#include <limits>
#include <QtGlobal>
#include <QMutex>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QMainWindow>

#include "defs.h"
#include "debug.h"

// extern classes
#include "loadcsvdialog.h"
#include "datamanagerdialog.h"
#include "modulemanagerdialog.h"
#include "debugdialog.h"
#include "templatemanagerdialog.h"
#include "downloaddatadialog.h"
#include "progressdialog.h"
#include "waitdialog.h"
#include "qtachart.h"

// extern variables
extern QProgressBar *GlobalProgressBar;         // progress bar proxy
extern QTAChart *chartwidget;                   // widget of the chart
extern LoadCSVDialog *loadcsvdialog;            // dialog to load CSV file
extern DownloadDataDialog *downloaddatadialog;  // dialog to download data
extern ProgressDialog *progressdialog;          // dialog to show progress
extern DebugDialog *debugdialog;                // debug console
extern TemplateManagerDialog *templatemanager;  // template manager dialog
extern QMutex *ResourceMutex;                   // mutex to protect shared resources
extern SQLists *ComboItems;             // QStringLists used as combo box items and more
extern QString installationPath;        // the path ChartGeany binary is installed
extern int NCORES;                      // number of active cores
extern const char DEFAULT_FONT_FAMILY[];
extern const int  FONT_POINTSIZE_PAD;
extern const int  FONT_PIXELSIZE_PAD;
extern const int  CHART_FONT_SIZE_PAD;
extern bool showlicense;
extern bool WinStore;
extern "C" void * CGScriptFunctionRegistry_ptr ();
extern size_t CGScriptFunctionRegistrySize;

// extern functions

// show a message box
extern void
showMessage (const QString& message, QWidget* parent = nullptr);

// show an Ok/Cancel question box
extern bool
showOkCancel (const QString& message, QWidget* parent = nullptr);

// show download message box
extern bool
showDownloadMessage (void);

// delay nsecs
extern void
delay(int secs);

// returns full operating system description
extern QString
fullOperatingSystemVersion (void);

// sqlite3_exec callback for retrieving supported data formats
extern int
sqlcb_formats(void *dummy, int argc, char **argv, char **column);

// sqlite3_exec callback for retrieving supported timeframes
extern int
sqlcb_timeframes(void *dummy, int argc, char **argv, char **column);

// sqlite3_exec callback for retrieving supported currencies
extern int
sqlcb_currencies(void *dummy, int argc, char **argv, char **column);

// sqlite3_exec callback for retrieving markets
extern int
sqlcb_markets(void *dummy, int argc, char **argv, char **column);

// sqlite3_exec callback for retrieving datafeeds
extern int
sqlcb_datafeeds(void *dummy, int argc, char **argv, char **column);

// sqlite3_exec callback for retrieving symbol's data frames
extern int
sqlcb_dataframes (void *vectorptr, int argc, char **argv, char **column);

// sqlite3_exec callback for retrieving symbol's fundamental data
extern int
sqlcb_fundamentals (void *data, int argc, char **argv, char **column);

// sqlite3_exec callback for retrieving nsymbols
extern int
sqlcb_nsymbols(void *nsymptr, int argc, char **argv, char **column);

// sqlite3_exec callback for ticker symbols
extern int
sqlcb_tickersymbols (void *data, int argc, char **argv, char **column);

// sqlite3_exec callback for ticker feed
extern int
sqlcb_tickerfeed (void *data, int argc, char **argv, char **column);

// sqlite3_exec callback for transaction types
extern int
sqlcb_transactiontypes (void *data, int argc, char **argv, char **column);

// sqlite3_exec callback for commission types
extern int
sqlcb_commissiontypes (void *data, int argc, char **argv, char **column);

// correct font size for widget and children
extern void
correctWidgetFonts (QWidget *widget);

extern void
correctWidgetFonts (QDialog *widget);

extern void
correctWidgetFonts (QMessageBox *widget);

// correct the fonts of a button in a button box
extern void
correctButtonBoxFonts (QDialogButtonBox *box,
                       QDialogButtonBox::StandardButton button);

// corect title bar of QDialog
extern void
correctTitleBar (QDialog *dialog);

// native http header
extern QString
nativeHttpHeader (void);

// json parse
extern bool
json_parse (QString jsonstr, QStringList *node, QStringList *value, void *n1);

// object's family tree of descendants
extern QObjectList
familyTree (QObject *obj);

// roundf(3) for Windows
#ifdef Q_OS_WIN32
inline float
roundf(float x)
{
  return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
#endif // Q_OS_WIN32

// returns the number of decimal digits
inline qint32
fracdig (qreal r) NOEXCEPT
{
  const float eps = 0.0001f;
  float inp = (float) r, x;
  qint32 fdigits=0, mfdigits;

  if (r >= 1.0)
    mfdigits = 2;
  else
    mfdigits = 4;

  x = fabsf(roundf(inp) - inp);
  while(x > eps && fdigits < mfdigits)
  {
    inp = inp * 10;
    x = fabsf(roundf(inp) - inp);
    fdigits ++;
  }

  return fdigits;
}

// convert null terminated string to upper case
extern GNUFASTCALL char * MSVCFASTCALL
strtoupper (char *str);

// override cursor
extern void
appSetOverrideCursor (const QWidget *widget, const QCursor & cursor);

// restore overrided cursor
extern void
appRestoreOverrideCursor (const QWidget *widget);

// create portfolio views
extern QString
createportfolioviews (QString viewname);

#if defined (Q_OS_LINUX) || defined (Q_OS_MAC)
// install signal handlers
extern void
install_signal_handlers ();

// get last signal
extern int
get_last_signal ();
#endif

#endif // COMMON_H
