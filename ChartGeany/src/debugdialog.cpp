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

#include <QScrollBar>
#include "ui_debugdialog.h"
#include "debugdialog.h"
#include "common.h"
#include "cgscript.h"

DebugDialog::DebugDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::DebugDialog)
{
  Q_UNUSED (QTACastFromConstVoid)

  ui->setupUi (this);
  ui->debugPlainTextEdit->verticalScrollBar ()->
    setStyleSheet (QStringLiteral ("background: transparent; background-color:lightgray;"));
  ui->debugPlainTextEdit->
    setStyleSheet (QStringLiteral ("QMenu{background: transparent; background-color:lightgray;selection-background-color: blue; border: 1px solid black;}"));


  // connect to signals
  connect (ui->hideButton, SIGNAL (clicked ()), this,
           SLOT (hideButton_clicked ()));
  connect (ui->clearButton, SIGNAL (clicked ()), this,
           SLOT (clearButton_clicked ()));

  if (parent != NULL)
    setParent (parent);

  // correctTitleBar (this); <-- this is not suitable for debug console
  // We need it to stay on top

  Qt::WindowFlags flags = windowFlags();
  Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
  flags = flags & (~helpFlag);
  setWindowFlags(flags);
  setWindowFlags (((windowFlags() | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint)
                   & ~Qt::WindowCloseButtonHint));
  clearButton_clicked ();
}

DebugDialog::~DebugDialog ()
{
  delete ui;
}

void
DebugDialog::appendText (QString str)
{
  QTextCursor cursor = ui->debugPlainTextEdit->textCursor();
  int cursorLine = cursor.blockNumber();
  if (cursorLine == 2000) clearButton_clicked ();
  ui->debugPlainTextEdit->appendPlainText (str);
}

/// Events
///
// resize
void
DebugDialog::resizeEvent (QResizeEvent * event)
{
  Q_UNUSED (event);
  ui->debugPlainTextEdit->resize (width () - 4, height () - 50);
  ui->buttonFrame->move ((width () - 320) / 2, height () - 45);
}

// change
void
DebugDialog::changeEvent (QEvent * event)
{
  if (event->type() == QEvent::WindowStateChange && isMinimized())
    hide ();
}

// clear button clicked
void
DebugDialog::clearButton_clicked (void)
{
  ui->debugPlainTextEdit->clear ();
  ui->debugPlainTextEdit->appendHtml
  (QStringLiteral ("<p><b><h3>CGScript version ") % QString (_CGSCRIPT_VERSION_STR) % QStringLiteral (" </h2></b></p>"));
  appendText ("");
}

// hide button clicked
void
DebugDialog::hideButton_clicked (void)
{
  hide ();
}
