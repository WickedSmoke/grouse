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

#include "ta_libc.h"

#include <memory>

#include <QtGlobal>

#include "segvcatch.h"
#include "chartapp.h"

using std::unique_ptr;

#ifdef DEBUG
#if QT_VERSION > QT_VERSION_CHECK(5, 4, 0)
#include <QFile>
#include <QTextStream>

void CGMessageHandler (QtMsgType type, const QMessageLogContext &context,
                       const QString &msg)
{
  QByteArray localMsg = msg.toLocal8Bit();
  switch (type)
  {
  case QtDebugMsg:
    fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    break;
  case QtInfoMsg:
    fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    break;
  case QtWarningMsg:
    fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    break;
  case QtCriticalMsg:
    fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    break;
  case QtFatalMsg:
    fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    abort();
  }
}
#endif // QT_VERSION
#endif // DEBUG

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QTextCodec>
#else
#include "natevents.h"
#endif

#include <QTimer>
#include <QLocale>
#include <QFileInfo>

#include "appdata.h"
#include "mainwindow.h"

QString installationPath;

int
main (int argc, char *argv[])
{
  const QString argv0(argv[0]);
  QFileInfo fi(argv0);
  installationPath = fi.absolutePath();

#if defined (Q_OS_LINUX)
  install_signal_handlers ();
#endif

#ifdef Q_OS_WIN32
  segvcatch::init_segv ();
  segvcatch::init_fpe ();

  QByteArray path = installationPath.toLatin1 ();
  if (platformString ().contains (QStringLiteral ("gcc")))
    path += QByteArray ("/gcc/bin;%path%");
  else
  if (platformString ().contains (QStringLiteral ("pcc")))
    path +=  QByteArray ("/pcc/bin;/pcc/libexec;%path%");

  qputenv ("PATH", path);
#endif

  TA_RetCode retCode;
  int argc1 = argc;
  char **argv1 = argv;

  retCode = TA_Initialize( );
  if( retCode != TA_SUCCESS )
  {
    printf( "Cannot initialize TA-Lib (%d)!\n", retCode );
#if defined (Q_OS_WIN) || defined (Q_OS_MAC)
    exit (1);
#else
    quick_exit (1);
#endif
  }

// manipulate command line arguments
// -platform windows:fontengine=freetype
#ifdef Q_OS_WIN32
  char arg1[64], arg2[64];

  argc1 = argc + 2;
  argv1 = (char **) malloc((argc1) * sizeof(*argv1));
  for (int counter = 0; counter < argc; counter ++)
    argv1[counter] = argv[counter];

  strcpy (arg1, "-platform");
  argv1[argc] = arg1;

  strcpy (arg2, "windows:fontengine=freetype");
  argv1[argc + 1] = arg2;
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  ChartApp::setStyle("plastique");
  ChartApp::setGraphicsSystem ("native");
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#else
#ifdef Q_OS_WIN32
  QStringList libpaths;
  libpaths << ".";
  QCoreApplication::setLibraryPaths (libpaths);
#endif // QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  ChartApp::setStyle("fusion");
#if QT_VERSION > QT_VERSION_CHECK(5, 6, 0)
  ChartApp::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support
#endif
#endif

  ChartApp::setApplicationName (APPNAME);
  ChartApp::setOrganizationName (APPNAME);
  ChartApp App (argc1, argv1);

  AppProxyStyle *appstyle = new AppProxyStyle;
  App.setStyle(appstyle);

#ifdef DEBUG
#if QT_VERSION > QT_VERSION_CHECK(5, 4, 0)
  QFile file ("Application.log");
  file.remove ();
  qInstallMessageHandler(CGMessageHandler);
#endif // QT_VERSION
#endif // DEBUG

  QLocale::setDefault(QLocale (QLocale::English, QLocale::UnitedStates));
  unique_ptr <MainWindow> ApplicationMain (new MainWindow);

#ifdef Q_OS_WIN32
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  NativeEventFilter filter;
  App.installNativeEventFilter(&filter);
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#endif // Q_OS_WIN32

  appSetOverrideCursor (ApplicationMain.get (), QCursor(Qt::ArrowCursor));
  int retcode = App.exec ();

  // delete ApplicationMain;

  if (argv1 != argv && argv1 != nullptr)
    free (argv1);

  return retcode;
}
