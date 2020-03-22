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

#include "optsize.h"
#include <QDir>
#include <QTextStream>
#include <QFile>
#include <QTemporaryFile>
#include <QDesktopServices>
#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
#include <QWebFrame>
#include <QWebElement>
#else
#include <QWebEnginePage>
#endif // QT_VERSION_CHECK(5, 7, 0)

#include <QUrl>
#include <QResizeEvent>
#include "ui_manualdialog.h"
#include "manualdialog.h"
#include "defs.h"
#include "appdata.h"
#include "common.h"

// constructor
ManualDialog::ManualDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::ManualDialog)
{
  QFile manualtmpl, manual;
  QString html, manualtmplpath, appwebpage;

  ui->setupUi (this);

#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
  manualView = new QWebView (this);
#else
  manualView = new QTCWebEngineView (this);
#endif

  manualView->setObjectName(QStringLiteral("manualView"));
  manualView->setGeometry(QRect(5, 5, 690, 480));

  manualView->setContextMenuPolicy(Qt::CustomContextMenu);
  setWindowTitle (QStringLiteral ("Manual"));

  appwebpage =
    QStringLiteral ("<a href=\"") %
    APPWEBPAGE %
    QStringLiteral ("\">") %
    APPWEBPAGE %
    QStringLiteral ("</a>");

  manualpath =
    QDir::homePath () % QStringLiteral ("/") %
    QStringLiteral (".config") % QStringLiteral ("/") % APPDIR %
    QStringLiteral ("/") % QStringLiteral ("manual.html");
  manualtmpl.setFileName (":/html/Manual.html");
  manualtmpl.open (QIODevice::ReadOnly|QIODevice::Text);

  manual.setFileName (manualpath);
  manual.open (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);

  QTextStream in (&manualtmpl);
  QTextStream out (&manual);
  html = in.readLine (0);
  while (!in.atEnd ())
  {
    html.replace (QStringLiteral ("APPNAME"), APPNAME);
    html.replace (QStringLiteral ("VERSION_MAJOR"), QString::number(VERSION_MAJOR));
    html.replace (QStringLiteral ("VERSION_MINOR"), QString::number(VERSION_MINOR));
    html.replace (QStringLiteral ("VERSION_PATCH"), QString::number(VERSION_PATCH));
    html.replace (QStringLiteral ("APPWEBPAGE"), appwebpage);
    html.replace (QStringLiteral ("DBNAME"), DBNAME);
    html.replace (QStringLiteral ("APPDIR"), APPDIR);
    out << html << QStringLiteral ("\n");
    html = in.readLine (0);
  }
  manual.close ();
  manualtmpl.close ();

  const QUrl url = QUrl::fromLocalFile (manualpath);

  manualView->load (url);
#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
  manualView->page ()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
  manualView->setTextSizeMultiplier ( manualView->textSizeMultiplier () * 1.2 );
  connect (manualView, SIGNAL (linkClicked ( const QUrl & )), this,
           SLOT (link_clicked (const QUrl &)));
  /*
     #else
     We need to add code for QTCWebEngineView
  */
#endif
  connect(ui->closeButton, SIGNAL(clicked ()), this, SLOT(close_clicked ()));

  // correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);

  correctTitleBar (this);
}

// destructor
ManualDialog::~ManualDialog ()
{
  QFile::remove(manualpath);
  delete manualView;
  delete ui;
}

/// events
// resize
void
ManualDialog::resizeEvent (QResizeEvent * event)
{
  if (event->oldSize () == event->size ())
    return;

  ui->line->setGeometry (0, height () - 45, width (), 1);
  ui->closeButton->move ((width () - 90) / 2, height () - 35);
  manualView->setGeometry (10, 10, width () - 20, height () - 60);
}

// change
void
ManualDialog::changeEvent (QEvent * event)
{
  if (event->type()==QEvent::WindowStateChange && isMinimized())
    hide ();
}

/// slots
// close button
void
ManualDialog::close_clicked ()
{
  hide ();
}

void
ManualDialog::link_clicked (const QUrl &url)
{
  QDesktopServices::openUrl (url);
}

