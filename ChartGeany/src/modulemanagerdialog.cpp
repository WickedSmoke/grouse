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

#include <stdexcept>
#include <QLibrary>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QScrollBar>
#include <QResizeEvent>
#include <QDateTime>
#include <QTemporaryFile>
#include <QTextStream>
#include "ui_modulemanagerdialog.h"
#include "modulemanagerdialog.h"
#include "mainwindow.h"
#include "qtachart_core.h"
#include "compile.h"
#include "simplecrypt.h"

static const int NCOLUMNS = 8;
extern int sqlcb_modules (void *classptr, int argc,
                          char **argv, char **column);

// constructor
ModuleManagerDialog::ModuleManagerDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::ModuleManagerDialog)
{
  const QString
  stylesheet = QStringLiteral ("background: transparent; background-color: white;"),
  stylesheet2 = QStringLiteral ("background: transparent; background-color: white; color:black");
  QStringList cheadersList; // list of columns' headers

  cheadersList << QStringLiteral ("  Name  ")
               << QStringLiteral ("  Source ")
               << QStringLiteral ("  Binary  ")
               << QStringLiteral ("  Type  ")
               << QStringLiteral ("  Author  ")
               << QStringLiteral ("  Version  ")
               << QStringLiteral ("  Status  ")
               << QStringLiteral ("  ID  ");

  ui->setupUi (this);
  this->setStyleSheet (stylesheet);

  ui->tableWidget->setColumnCount (NCOLUMNS);
  ui->tableWidget->setHorizontalHeaderLabels (cheadersList);
  ui->addButton->setStyleSheet (stylesheet2);
  ui->importButton->setStyleSheet (stylesheet2);
  ui->exportButton->setStyleSheet (stylesheet2);
  ui->deleteButton->setStyleSheet (stylesheet2);
  ui->exitButton->setStyleSheet (stylesheet2);
  ui->runButton->setStyleSheet (stylesheet2);
  ui->tableWidget->setStyleSheet (stylesheet);
  ui->tableWidget->verticalScrollBar ()->setStyleSheet (QStringLiteral ("background: transparent; background-color:lightgray;"));
  ui->tableWidget->horizontalScrollBar ()->setStyleSheet (QStringLiteral ("background: transparent; background-color:lightgray;"));
  ui->tableWidget->setColumnHidden (1, true);
  ui->tableWidget->setColumnHidden (2, true);
#ifndef DEBUG
  ui->tableWidget->setColumnHidden (6, true);
#endif
  ui->tableWidget->setSelectionMode (QAbstractItemView::SingleSelection);
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

  // connect to signals
  connect (ui->addButton, SIGNAL (clicked ()), this,
           SLOT (addButton_clicked ()));
  connect (ui->importButton, SIGNAL (clicked ()), this,
           SLOT (importButton_clicked ()));
  connect (ui->exportButton, SIGNAL (clicked ()), this,
           SLOT (exportButton_clicked ()));
  connect (ui->runButton, SIGNAL (clicked ()), this,
           SLOT (runButton_clicked ()));
  connect (ui->deleteButton, SIGNAL (clicked ()), this,
           SLOT (deleteButton_clicked ()));
  connect (ui->exitButton, SIGNAL (clicked ()), this,
           SLOT (exitButton_clicked ()));
  connect (ui->downToolButton, SIGNAL (clicked ()), this,
           SLOT (downButton_clicked ()));
  connect (ui->upToolButton, SIGNAL (clicked ()), this,
           SLOT (upButton_clicked ()));
  connect(ui->tableWidget, SIGNAL(doubleClicked(const QModelIndex &)), this,
          SLOT(module_double_clicked ()));

  // correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);

  // correctTitleBar (this);



}

// destructor
ModuleManagerDialog::~ModuleManagerDialog ()
{
  cleartable ();
  delete ui;
}

// clear table
void
ModuleManagerDialog::cleartable ()
{

  int row, nrows, col, ncols = NCOLUMNS;

  nrows = ui->tableWidget->rowCount ();
  for (row = 0; row < nrows; row ++)
    for (col = 0; col < ncols; col ++)
      delete ui->tableWidget->itemAt (row,col);


  ui->tableWidget->clearContents ();
  ui->tableWidget->setRowCount (0);
  ui->tableWidget->setSortingEnabled (false);
}

// reload the modules
void
ModuleManagerDialog::reloadModules ()
{
  QString SQL;
  int rc, count;

  SQL = QStringLiteral ("SELECT count (*) FROM modules");
  count = selectcount (SQL);
  if (count == -1)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return;
  }

  cleartable ();

  ui->tableWidget->setRowCount(count);
  ui->tableWidget->setColumnCount(NCOLUMNS);

  for (int row = 0; row < count; row ++)
    for (int col = 0; col < 8; col++)
  {
    QTableWidgetItem *item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText ("Empty");
    ui->tableWidget->setItem (row, col, item);
  }

  SQL = QStringLiteral ("SELECT * FROM modules");
  rc = selectfromdb(SQL.toUtf8(), sqlcb_modules, static_cast <void *> (ui->tableWidget));
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return;
  }

  ui->tableWidget->resizeColumnToContents (0);
  ui->tableWidget->resizeRowsToContents ();
  ui->tableWidget->setSortingEnabled (true);
  ui->tableWidget->viewport()->update();
}

// attach a module on chart
void *
ModuleManagerDialog::attachModule (void *data, QString modid)
{
  QString binary = "", id = "", modname = "", status = "", source = "",
          name = "", messages;

  for (int row = 0; row < ui->tableWidget->rowCount (); row ++)
  {
    if (ui->tableWidget->item (row, 7)->text () == modid)
    {
      name = ui->tableWidget->item (row, 0)->text ();
      binary = ui->tableWidget->item (row, 2)->text ();
      id = ui->tableWidget->item (row, 7)->text ();
      modname = ui->tableWidget->item (row, 0)->text ();
      status = ui->tableWidget->item (row, 6)->text ();
      source = ui->tableWidget->item (row, 1)->text ();
    }
  }

  if (status != "ENABLED" || platformString ().startsWith ("darwin"))
  {
    Compile program;
    CG_ERR_RESULT result = program.CGSCript (source, name, messages, false);
    if (result == CG_ERR_OK)
    {
      reloadModules ();
      for (int row = 0; row < ui->tableWidget->rowCount (); row ++)
      {
        if (ui->tableWidget->item (row, 7)->text () == modid)
        {
          name = ui->tableWidget->item (row, 0)->text ();
          binary = ui->tableWidget->item (row, 2)->text ();
          id = ui->tableWidget->item (row, 7)->text ();
          modname = ui->tableWidget->item (row, 0)->text ();
          status = ui->tableWidget->item (row, 6)->text ();
          source = ui->tableWidget->item (row, 1)->text ();
          /* Add binary load code here */
        }
      }
    }
    else
    {
      showMessage (errorMessage (result), this);
      debugdialog->appendText ("Compilation output for module " % name % ":");
      debugdialog->appendText (messages);
      return NULL;
    }
  }

  QByteArray bin = QByteArray::fromHex (binary.toStdString().c_str());
  QByteArray ba = (QString::number(QDateTime::currentMSecsSinceEpoch ())).toLatin1 ();
  QString tmpmodname = QString(QCryptographicHash::hash((ba),
                               QCryptographicHash::Md5).toHex());

  QFile tempFile (QDir::tempPath () % QDir::separator()  % tmpmodname % SOEXT);
  if (!tempFile.open (QIODevice::WriteOnly))
  {
    debugdialog->appendText ("Cannot open module: " % id);
    return NULL;
  }

  tempFile.resize (0);
  if( tempFile.write( bin ) < 0 )
  {
    debugdialog->appendText ("Cannot open module: " % id);
    tempFile.close ();
    return NULL;
  }
  tempFile.close ();

  return static_cast <void *> (new QTACObject (data,
                              tempFile.fileName (),
                              modname));
}

/// Events
///
// resize
void
ModuleManagerDialog::resizeEvent (QResizeEvent * event)
{
  QSize newsize;

  if (event->oldSize () == event->size ())
    return;

  newsize = event->size ();
  ui->exitButton->move (newsize.width () - 50, ui->exitButton->y ());
  ui->upToolButton->move (newsize.width () - 50, ui->upToolButton->y ());
  ui->downToolButton->move (newsize.width () - 50, ui->downToolButton->y ());
  ui->tableWidget->resize (newsize.width () - 60, newsize.height () - 90);
}

// show
void
ModuleManagerDialog::showEvent (QShowEvent * event)
{
  if (!event->spontaneous ())
    ui->tableWidget->clearSelection ();
}

// change
void
ModuleManagerDialog::changeEvent (QEvent * event)
{
  if (!event->spontaneous ())
    ui->tableWidget->clearSelection ();

  reloadModules ();
}

// delete key
void
ModuleManagerDialog::keyPressEvent (QKeyEvent * event)
{
  if (event->key () == Qt::Key_Delete)
    deleteButton_clicked ();
}

/// Slots
// down
void
ModuleManagerDialog::downButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
  ui->tableWidget->setFocus (Qt::MouseFocusReason);
}

// up
void
ModuleManagerDialog::upButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
  ui->tableWidget->setFocus (Qt::MouseFocusReason);
}

// addButton_clicked ()
void
ModuleManagerDialog::addButton_clicked ()
{
  QWidget *wid;
  wid = (qobject_cast <MainWindow*> (parent ()))->tabWidget->currentWidget ();

  if (wid == NULL)
  {
    showMessage ("Open a chart first please.", this);
    return;
  }

  if (wid->objectName () != QLatin1String ("Chart"))
  {
    showMessage ("Current tab is not a chart", this);
    return;
  }

  QTAChart *chart = qobject_cast <QTAChart *> (wid);
  QTAChartCore *core =  static_cast <QTAChartCore *> (getData (chart));

  if (core->events_enabled == false)
  {
    showMessage ("Chart not in chart view", this);
    return;
  }

  QString id = "";

  for (int row = 0; row < ui->tableWidget->rowCount (); row ++)
  {
    if (ui->tableWidget->item (row, 0)->isSelected ())
      id = ui->tableWidget->item (row, 7)->text ();
  }

  attachModule (getData (chart), id);
  this->hide ();
}

// importButton_clicked ()
void
ModuleManagerDialog::importButton_clicked ()
{
  Compile program;
  SimpleCrypt crypto(ENCKEY);
  QString fileName, SQL, messages = "", modname;
  CG_ERRORS localError = CG_ERR_OK;

  QFileDialog *fileDialog = new QFileDialog;
  correctWidgetFonts (fileDialog);
  fileName = fileDialog->getOpenFileName(this, "Import module", fileName, "(*.cgm)");
  delete fileDialog;

  if (fileName == "")
    return;

  QFileInfo fi(fileName);
  modname = fi.baseName ();

  QTemporaryFile *tempfile = new QTemporaryFile;
  if (tempfile->open())
  {
    QFile modscript (fileName);
    if (modscript.open (QIODevice::ReadOnly|QIODevice::Text))
    {
      QString encstr;
      QTextStream in (&modscript);
      encstr = in.readLine (0);
      SQL = crypto.decryptToString (encstr);
      tempfile->write(SQL.toUtf8 ());
      modscript.close ();
      int rc = updatedb (SQL);
      if (rc != SQLITE_OK)
      {
        setGlobalError(CG_ERR_WRITE_FILE, __FILE__, __LINE__);
        showMessage ("Module import failed.", this);
        goto importButton_clicked_end;
      }
      reloadModules ();
    }
    else
    {
      localError = CG_ERR_OPEN_FILE;
      goto importButton_clicked_end;
    }
  }
  else
  {
    localError = CG_ERR_OPEN_FILE;
    goto importButton_clicked_end;
  }
  tempfile->close ();

importButton_clicked_end:
  delete tempfile;

  if (localError != CG_ERR_OK)
  {
    setGlobalError(localError, __FILE__, __LINE__);
    showMessage (errorMessage (localError), this);
  }

  ui->tableWidget->clearSelection ();
}

// exportButton_clicked ()
void
ModuleManagerDialog::exportButton_clicked ()
{
  SimpleCrypt crypto(ENCKEY);
  QString source = "", name = "", SQL = "", id = "",
          author = "", type = "", version = "";


  for (int row = 0; row < ui->tableWidget->rowCount (); row ++)
  {
    if (ui->tableWidget->item (row, 0)->isSelected ())
    {
      source = ui->tableWidget->item (row, 1)->text ();
      name = ui->tableWidget->item (row, 0)->text ();
      id = ui->tableWidget->item (row, 7)->text ();
      author = ui->tableWidget->item (row, 4)->text ();
      type = ui->tableWidget->item (row, 3)->text ();
      version = ui->tableWidget->item (row, 5)->text ();
    }
  }

  if (name == "")
  {
    showMessage ("Select a module first please.", this);
    return;
  }

  SQL += "DELETE FROM modules WHERE id = '" % id % "';";
  SQL.append ("\n");
  SQL +=
    "INSERT INTO modules (id, name, source, author, type, version, status, platform, binary)\
   VALUES ('" % id % "','"\
    + name % "','"\
    + source % "','"\
    + author % "','"\
    + type % "','"\
    + version % "','"\
    % "DISABLED', '', '');";

  QFileDialog *fileDialog = new QFileDialog;
  correctTitleBar (fileDialog);
  correctWidgetFonts (fileDialog);
  QString fileName = fileDialog->getSaveFileName(this, "Export module", name % ".cgm", "(*.cgm)");
  delete fileDialog;

  if (fileName == "")
    return;

  if (fileName.mid (fileName.size () - 4).toLower () != ".cgm")
    fileName += ".cgm";

  QFile modscript (fileName);
  modscript.open(QIODevice::WriteOnly | QIODevice::Text);
  QString encstr = crypto.encryptToString (SQL);
  modscript.write(encstr.toUtf8 ());
  modscript.close();
  showMessage ("Export complete.", this);
  ui->tableWidget->clearSelection ();
}

// deleteButton_clicked ()
void
ModuleManagerDialog::deleteButton_clicked ()
{
  QString id = "", SQL;
  int rc;

  for (int row = 0; row < ui->tableWidget->rowCount (); row ++)
  {
    if (ui->tableWidget->item (row, 0)->isSelected ())
      id = ui->tableWidget->item (row, 7)->text ();
  }

  if (id == "")
  {
    showMessage ("Select a module first please.", this);
    return;
  }

  if (showOkCancel ("Delete selected module?", this) == false)
    return;

  SQL = "DELETE FROM modules WHERE id = '" % id % "';";
  rc = updatedb (SQL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_TRANSACTION, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_TRANSACTION), this);
    return;
  }

  reloadModules ();
}

// runButton_clicked ()
void
ModuleManagerDialog::runButton_clicked ()
{
  QString binary = "", id = "";

  for (int row = 0; row < ui->tableWidget->rowCount (); row ++)
  {
    if (ui->tableWidget->item (row, 0)->isSelected ())
    {
      binary = ui->tableWidget->item (row, 2)->text ();
      id = ui->tableWidget->item (row, 7)->text ();
    }
  }

  if (id == "")
  {
    showMessage ("Select a module first please.", this);
    return;
  }

  QLibrary module (QDir::tempPath () % QDir::separator()  % id);
  if (module.isLoaded ())
  {
    if (!module.unload ())
    {
      showMessage ("Cannot unload module", this);
      return;
    }
  }

  QByteArray bin = QByteArray::fromHex (binary.toStdString().c_str());
  QFile tempFile (QDir::tempPath () % QDir::separator()  % id % SOEXT);
  if (!tempFile.open (QIODevice::WriteOnly))
  {
    debugdialog->appendText ("Cannot open module: " % id);
    return;
  }

  tempFile.resize (0);
  if( tempFile.write( bin ) < 0 )
  {
    debugdialog->appendText ("Cannot open module: " % id);
    tempFile.close ();
    return;
  }
  tempFile.close ();

  module.setFileName (QDir::tempPath () % QDir::separator()  % id);
  if (module.isLoaded ())
  {
    if (!module.unload ())
    {
      debugdialog->appendText ("Cannot unload module: " % id);
      return;
    }
  }

  this->hide ();
  debugdialog->show ();

  ModuleInit modinit = (ModuleInit) module.resolve("_CGModuleInitObject");
  if (modinit == NULL)
  {
    debugdialog->appendText ("Cannot use module: " % id);
    return;
  }

  ModuleFinish modfinish = (ModuleFinish) module.resolve("_CGModuleFinish");
  if (modfinish == NULL)
  {
    debugdialog->appendText ("Cannot clear module: " % id);
    return;
  }

  int mrslt = modinit (CGScriptFunctionRegistry_ptr (), CGScriptFunctionRegistrySize,
                       NULL, NULL, NULL, NULL);
  Q_UNUSED (mrslt);
  modfinish ();

  if (module.isLoaded ())
  {
    if (!module.unload ())
    {
      debugdialog->appendText ("Cannot reset module: " % id);
      return;
    }
  }

  QDir dir;
  dir.remove (module.fileName ());

  return;
}

// exitButton_clicked ()
void
ModuleManagerDialog::exitButton_clicked ()
{
  ui->tableWidget->clearSelection ();
  this->hide ();
}

// module_double_clicked ()
void
ModuleManagerDialog::module_double_clicked ()
{
  addButton_clicked ();
}
