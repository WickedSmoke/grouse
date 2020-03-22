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

#include <QtGlobal>
#include <QDir>
#include <QDateTime>
#include <QCryptographicHash>
#include <QTextStream>
#include <QFileInfo>
#include <QScrollBar>

#include "ui_editorwidget.h"
#include "mainwindow.h"
#include "editorwidget.h"
#include "compile.h"
#include "common.h"

static const int HPAD = 100;

// constructor
EditorWidget::EditorWidget (QWidget * parent):
  QWidget (parent), ui (new Ui::EditorWidget)
{
  QStringList fontsizes;
  const QString stylesheet =
    QStringLiteral ("background: transparent; background-color: white; color:black");

  fontsizes
      << QStringLiteral ("7")
      << QStringLiteral ("8")
      << QStringLiteral ("9")
      << QStringLiteral ("10")
      << QStringLiteral ("11")
      << QStringLiteral ("12")
      << QStringLiteral ("13")
      << QStringLiteral ("14");

  ui->setupUi (this);
  ui->openButton->setStyleSheet (stylesheet);
  ui->saveButton->setStyleSheet (stylesheet);
  ui->saveAsButton->setStyleSheet (stylesheet);
  ui->compileButton->setStyleSheet (stylesheet);
  ui->expandButton->setStyleSheet (stylesheet);
  ui->messagesButton->setStyleSheet (stylesheet);
  ui->debugButton->setStyleSheet (stylesheet);
  ui->messagePlainTextEdit->setStyleSheet ("border: 1px solid black; background-color:lightgray; QMenu {background: transparent; background-color:lightgray; border: 1px solid black;}");
  ui->messagePlainTextEdit->move (5, height () - 205);
  ui->messagePlainTextEdit->setVisible (true);
  ui->sizeComboBox->addItems (fontsizes);

#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
  ceditor = new CodeEditor (this);
#else
  ceditor = new QCodeEditor (this);
  highlighter = new QCXXHighlighter;
  ceditor->setHighlighter (highlighter);
#endif

  ceditor->verticalScrollBar ()->setStyleSheet ("background: transparent; background-color:lightgray;");
  ceditor->horizontalScrollBar ()->setStyleSheet ("background: transparent; background-color:lightgray;");

  ceditor->move (5, 50);
  fileName = QStringLiteral ("");
  debug = true;
  firstshow = true;
  hpad = HPAD;
  openFileDialog = new QFileDialog (this);
  saveFileDialog = new QFileDialog (this);

  connect (ui->openButton, SIGNAL (clicked ()), this,
           SLOT (openButton_clicked ()));
  connect (ui->saveButton, SIGNAL (clicked ()), this,
           SLOT (saveButton_clicked ()));
  connect (ui->saveAsButton, SIGNAL (clicked ()), this,
           SLOT (saveAsButton_clicked ()));
  connect (ui->compileButton, SIGNAL (clicked ()), this,
           SLOT (compileButton_clicked ()));
  connect (ui->expandButton, SIGNAL (clicked ()), this,
           SLOT (expandButton_clicked ()));
  connect (ui->messagesButton, SIGNAL (clicked ()), this,
           SLOT (messagesButton_clicked ()));
  connect (ui->debugButton, SIGNAL (clicked ()), this,
           SLOT (debugButton_clicked ()));
  connect(ui->sizeComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(fontsize_changed(int)));
  connect(ui->fontComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(fontfamily_changed(int)));

  correctWidgetFonts (this);

  QString modid;
  QByteArray ba;
  ba = (QString::number(QDateTime::currentMSecsSinceEpoch ())).toLatin1 ();
  modid = QString(QCryptographicHash::hash((ba),
                  QCryptographicHash::Md5).toHex());
  modid = modid.left (6) + modid.right (2);

  QFile file;

  file.setFileName (QStringLiteral (":/source/cgscript/include/newscript.h"));
#ifdef DEBUG
#ifdef Q_OS_LINUX
  file.setFileName (QStringLiteral ("ChartGeany/cgscript/include/newscript.h"));
#endif // Q_OS_LINUX
#endif // DEBUG


  if (file.open(QIODevice::ReadOnly))
  {
    QTextStream ReadFile(&file);
    QString newscript;
    newscript = ReadFile.readAll();
    newscript.replace ("__MODID__", modid);
    ceditor->setPlainText(newscript);
    file.close ();
  }

  QFont previewfnt = ceditor->font ();

  int max = ui->fontComboBox->count ();
  for (int counter = 0; counter < max; counter ++)
    if (ui->fontComboBox->itemText ( counter ) == previewfnt.family ())
      ui->fontComboBox->setCurrentIndex (counter);

  bool ok;
  max = ui->sizeComboBox->count ();
  for (int counter = 0; counter < max; counter ++)
    if (ui->sizeComboBox->itemText ( counter ).toInt(&ok, 10) == previewfnt.pointSize ())
      ui->sizeComboBox->setCurrentIndex (counter);

  CompileHighlighter *compilehighlighter;
  compilehighlighter = new CompileHighlighter(ui->messagePlainTextEdit->document ());
  
  Q_UNUSED (compilehighlighter)
}

// destructor
EditorWidget::~EditorWidget ()
{
  if (fileName == "")
    fileName = "New Module.cgs";
  saveButton_clicked ();
  delete ui;

#if QT_VERSION > QT_VERSION_CHECK(5, 6, 0)
  delete highlighter;
#endif
}


/// Events and signals
// expand button clicked
void
EditorWidget::expandButton_clicked (void)
{
  MainWindow *mainwindow;

  mainwindow = (qobject_cast <MainWindow *>
                (this->parentWidget ()->parentWidget ()->parentWidget ()->parentWidget ()));

  if (mainwindow->expandedChart ())
    mainwindow->setExpandChart (false);
  else
    mainwindow->setExpandChart (true);

  return;
}

// resize
void
EditorWidget::resizeEvent (QResizeEvent * event)
{
  QSize newsize;

  if (event->oldSize () == event->size ())
    return;

  newsize = event->size ();
  ceditor->resize (newsize.width () - 10, newsize.height () - (ceditor->y() + hpad + 10));
  ui->messagePlainTextEdit->move (5, newsize.height () - (hpad + 5));
  ui->messagePlainTextEdit->resize (newsize.width () - 10, hpad);
}

// show
void
EditorWidget::showEvent (QShowEvent * event)
{
  Q_UNUSED (event);
  if (firstshow)
  {
    firstshow = false;
    ui->messagePlainTextEdit->setVisible (true);
  }
}

/// slots
// open button clicked
void
EditorWidget::openButton_clicked (void)
{
  QTabWidget *tw;
  QString selfilter = tr("(*.cgs)"), fname;
  int twidx;

  fname = openFileDialog->getOpenFileName(this, "Open file",
             fileName, "(*.cgs)", &selfilter);

  if (fname == "")
    return;

  if (fname == fileName || fileName == "")
  {
    QFile file(fname);
    if (file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
      QTextStream ReadFile(&file);
      ceditor->setPlainText(ReadFile.readAll());
      tw = (QTabWidget *) parent ()->parent ();
      twidx = tw->indexOf (this);
      tw->setTabText (twidx, QFileInfo(fname).fileName());
      fileName = fname;
    }
    else
      showMessage ("Cannot open file ");
  }
  else
    showMessage ("Open a new tab please ");
}

// save button clicked
void
EditorWidget::saveButton_clicked (void)
{
  if (fileName == "")
  {
    saveAsButton_clicked ();
    return;
  }

  QFile file(fileName);
  if (file.open(QIODevice::WriteOnly|QIODevice::Text))
  {
    file.resize (0);
    QTextStream stream(&file);
    stream << ceditor->toPlainText();
    file.flush();
    file.close();
  }
  else
    showMessage ("File cannot be saved");
}

// save as button clicked
void
EditorWidget::saveAsButton_clicked (void)
{
  QTabWidget *tw;
  QString selfilter = tr("(*.cgs)");
  int twidx;

  fileName = saveFileDialog->getSaveFileName(this, "Save file", fileName,
             "(*.cgs)", &selfilter);

  if (fileName == "")
    return;

  QFile file(fileName);
  if (file.open(QIODevice::WriteOnly|QIODevice::Text))
  {
    file.resize (0);
    QTextStream stream(&file);
    stream << ceditor->toPlainText();
    file.flush();
    file.close();
  }
  else
    showMessage ("File cannot be saved");


  tw = (QTabWidget *) parent ()->parent ();
  twidx = tw->indexOf (this);
  tw->setTabText (twidx, QFileInfo(fileName).fileName());
}

// messages button clicked
void
EditorWidget::messagesButton_clicked (void)
{
  if (hpad == 0)
  {
    hpad = HPAD;
    ui->messagePlainTextEdit->setVisible (true);
    ui->messagesButton->setIcon (QIcon (QString::fromUtf8(":/png/images/icons/EDITOR/lamp_active.png")));
  }
  else if (hpad == HPAD)
  {
    hpad = 0;
    ui->messagePlainTextEdit->setVisible (false);
    ui->messagesButton->setIcon (QIcon (QString::fromUtf8(":/png/images/icons/EDITOR/lamp_inactive.png")));
  }

  ceditor->resize (width () - 10, height () - (ceditor->y() + hpad + 10));
  ui->messagePlainTextEdit->move (5, height () - (hpad + 5));
  ui->messagePlainTextEdit->resize (width () - 10, hpad);
}

// compile button clicked
void
EditorWidget::compileButton_clicked (void)
{
  Compile *program = new Compile;
  CG_ERR_RESULT result;

  ui->compileButton->setEnabled (false);
  QFileInfo fi(fileName);

  saveButton_clicked ();

  QString sourcecode = ceditor->toPlainText(),
          bname = fi.baseName ();
  result = program->CGSCript (sourcecode, bname,
                              output, debug);
  if (result != CG_ERR_OK && result != CG_ERR_COMPILATION)
    showMessage (errorMessage (result));

  ui->messagePlainTextEdit->setPlainText (output);
  ui->compileButton->setEnabled (true);
  ui->messagePlainTextEdit->ensureCursorVisible();
  qApp->processEvents();
  delete program;
}

// debug button clicked
void
EditorWidget::debugButton_clicked (void)
{
  if (debug)
  {
    debug = false;
    ui->debugButton->setIcon (QIcon (QString::fromUtf8(":/png/images/icons/EDITOR/check-64.png")));
  }
  else
  {
    debug = true;
    ui->debugButton->setIcon (QIcon (QString::fromUtf8(":/png/images/icons/EDITOR/bug.png")));
  }
}

void
EditorWidget::fontsize_changed (int idx)
{
  QFont previewfnt;
  previewfnt = ceditor->font ();
  previewfnt.setPointSize (idx + 7);
  ceditor->setFont (previewfnt);
}

void
EditorWidget::fontfamily_changed (int idx)
{
  QFont previewfnt;
  previewfnt = ceditor->font ();
  previewfnt.setFamily (ui->fontComboBox->itemText ( idx ));
  ceditor->setFont (previewfnt);
}
