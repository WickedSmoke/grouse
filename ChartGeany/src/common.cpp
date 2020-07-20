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

#ifdef Q_OS_MAC
#include <CoreServices/CoreServices.h>
#endif

#ifdef Q_OS_LINUX
#include <sys/utsname.h>
#endif // Q_OS_LINUX

const char DEFAULT_FONT_FAMILY[] = "Tahoma";
#ifdef Q_OS_MAC
const int  FONT_POINTSIZE_PAD = 3;
const int  FONT_PIXELSIZE_PAD = 3;
#else
const int  FONT_POINTSIZE_PAD = 1;
const int  FONT_PIXELSIZE_PAD = 1;
#endif
const int  CHART_FONT_SIZE_PAD = 3;

// delay nsecs
void
delay(int secs)
{
  QTime dieTime= QTime::currentTime().addSecs(secs);
  while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
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

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

bool
json_parse (QString jsonstr, QStringList *node, QStringList *value, void *n1)
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
        json_parse (jsonstr, node, value, static_cast <void *> (&n2));
      }
    }

    if ((*n).value ((*n).keys ().at (counter)).type () == QJsonValue::Object)
    {
      QJsonObject n2 = (*n).value ((*n).keys ().at (counter)).toObject ();

      if (!json_parse (jsonstr, node, value, static_cast <void *> (&n2)))
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
