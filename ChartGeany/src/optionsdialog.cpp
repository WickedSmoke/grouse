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
#include <QDesktopServices>
#include <QCloseEvent>
#include <QShowEvent>
#include <QScrollBar>
#include <QHideEvent>
#include "ui_optionsdialog.h"
#include "optionsdialog.h"
#include "common.h"
#include "mainwindow.h"
#include "feedyahoo.h"
#include "feediex.h"
#include "netservice.h"
#include "top.h"

// constructor
OptionsDialog::OptionsDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::OptionsDialog)
{
  ui->setupUi (this);

  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Save);
  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Cancel);
  correctButtonBoxFonts (ui->symbolButtonBox, QDialogButtonBox::Save);
  correctButtonBoxFonts (ui->symbolButtonBox, QDialogButtonBox::Cancel);

  ui->tabWidget->setStyleSheet(QStringLiteral ("QTabBar::tab { height: 25px;}"));
  ui->tabWidget->setCurrentIndex (0);
  connect(ui->buttonBox, SIGNAL(accepted ()), this, SLOT(ok_clicked ()));
  connect(ui->buttonBox, SIGNAL(rejected ()), this, SLOT(cancel_clicked ()));
  connect(ui->newVersionButton,
          SIGNAL(clicked ()), this, SLOT(newVersionBtn_clicked ()));
  connect(ui->addButton,
          SIGNAL(clicked ()), this, SLOT(addBtn_clicked ()));
  connect(ui->minusButton,
          SIGNAL(clicked ()), this, SLOT(minusBtn_clicked ()));
  connect(ui->symbolButtonBox,
          SIGNAL(accepted ()), this, SLOT(symboladd_accepted ()));
  connect(ui->symbolButtonBox,
          SIGNAL(rejected ()), this, SLOT(symboladd_rejected ()));
  connect(ui->googleCheckBox,
          SIGNAL(stateChanged (int)), this, SLOT(googlefeed_clicked (int)));
  connect(ui->yahooCheckBox,
          SIGNAL(stateChanged (int)), this, SLOT(yahoofeed_clicked (int)));
  connect (ui->downToolButton, SIGNAL (clicked ()), this,
           SLOT (downButton_clicked ()));
  connect (ui->upToolButton, SIGNAL (clicked ()), this,
           SLOT (upButton_clicked ()));
  connect(ui->applyPlatformButton,
          SIGNAL(clicked ()), this, SLOT(applyPlatformBtn_clicked ()));

  showVolumes = ui->showVolumes;
  showGrid = ui->showGrid;
  lineChart = ui->lineChart;
  candleChart = ui->candleChart;
  barChart = ui->barChart;
  heikinAshi = ui->heikinAshi;
  lineColorButton = ui->lineColorButton;
  barColorButton = ui->barColorButton;
  foreColorButton = ui->foreColorButton;
  backColorButton = ui->backColorButton;
  linearScale = ui->linearScale;
  onlinePrice = ui->onlinePrice;

  linecolor = Qt::green;
  barcolor = Qt::green;
  forecolor = Qt::white;
  backcolor = Qt::black;
  lineicon = NULL;
  baricon = NULL;
  foreicon = NULL;
  backicon = NULL;
  linepixmap = NULL;
  barpixmap = NULL;
  forepixmap = NULL;
  backpixmap = NULL;

  styleGroup = new QButtonGroup (this);
  styleGroup->addButton (candleChart);
  styleGroup->addButton (lineChart);
  styleGroup->addButton (heikinAshi);
  styleGroup->addButton (barChart);
  styleGroup->setExclusive (true);

  connect (lineColorButton, SIGNAL (clicked ()), this,
           SLOT (lineColorButton_clicked ()));
  connect (barColorButton, SIGNAL (clicked ()), this,
           SLOT (barColorButton_clicked ()));
  connect (foreColorButton, SIGNAL (clicked ()), this,
           SLOT (foreColorButton_clicked ()));
  connect (backColorButton, SIGNAL (clicked ()), this,
           SLOT (backColorButton_clicked ()));

  if (parent != NULL)
    setParent (parent);

  correctTitleBar (this);

  QStringList cheadersList;
  cheadersList << QStringLiteral ("Symbol") << QStringLiteral ("Feed");

  ui->addSymbolFrame->setEnabled (false);
  ui->tickerTableWidget->setColumnCount (2);
  ui->tickerTableWidget->setHorizontalHeaderLabels (cheadersList);
  ui->tickerTableWidget->verticalScrollBar ()->setStyleSheet (QStringLiteral ("background: transparent; background-color:lightgray;"));
  ui->tickerTableWidget->horizontalScrollBar ()->setStyleSheet (QStringLiteral ("background: transparent; background-color:lightgray;"));
  ui->tickerTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui->InstallationPathEdit->setText (installationPath);

  lineColorDialog = new appColorDialog;
  lineColorDialog->setModal (true);
  linepixmap = new QPixmap (16, 16);
  lineicon = new QIcon;
  linepixmap->fill (linecolor);
  lineicon->addPixmap (*linepixmap, QIcon::Normal, QIcon::On);
  lineColorButton->setIcon (*lineicon);
  // lineColorDialog->setStyleSheet ("background-color: lightgray; color: black");
  connect (lineColorDialog, SIGNAL (finished (int)), this,
           SLOT (lineColorDialog_finished ()));

  barColorDialog = new appColorDialog;
  barColorDialog->setModal (true);
  barpixmap = new QPixmap (16, 16);
  baricon = new QIcon;
  barpixmap->fill (barcolor);
  baricon->addPixmap (*barpixmap, QIcon::Normal, QIcon::On);
  barColorButton->setIcon (*baricon);
  // barColorDialog->setStyleSheet ("background-color: lightgray; color: black");
  connect (barColorDialog, SIGNAL (finished (int)), this,
           SLOT (barColorDialog_finished ()));


  foreColorDialog = new appColorDialog;
  foreColorDialog->setModal (true);
  forepixmap = new QPixmap (16, 16);
  foreicon = new QIcon;
  forepixmap->fill (forecolor);
  foreicon->addPixmap (*forepixmap, QIcon::Normal, QIcon::On);
  foreColorButton->setIcon (*foreicon);
  // foreColorDialog->setStyleSheet ("background-color: lightgray; color: black");
  connect (foreColorDialog, SIGNAL (finished (int)), this,
           SLOT (foreColorDialog_finished ()));


  backColorDialog = new appColorDialog;
  backColorDialog->setModal (true);
  backpixmap = new QPixmap (16, 16);
  backicon = new QIcon;
  backpixmap->fill (backcolor);
  backicon->addPixmap (*backpixmap, QIcon::Normal, QIcon::On);
  backColorButton->setIcon (*backicon);
  // backColorDialog->setStyleSheet ("background-color: lightgray; color: black");
  connect (backColorDialog, SIGNAL (finished (int)), this,
           SLOT (backColorDialog_finished ()));

  loadPlatforms ();
  loadOptions ();
  // correctWidgetFonts (this);

  ui->platformEdit->setReadOnly (true);
  ui->compilerEdit->setReadOnly (true);
  ui->linkerEdit->setReadOnly (true);
  ui->CGScriptEdit->setReadOnly (true);
  ui->InstallationPathEdit->setReadOnly (true);
}

// destructor
OptionsDialog::~OptionsDialog ()
{
  // saveOptions ();
  delete styleGroup;

  if (lineicon != NULL)
    delete lineicon;

  if (linepixmap != NULL)
    delete linepixmap;

  if (baricon != NULL)
    delete baricon;

  if (barpixmap != NULL)
    delete barpixmap;

  if (forepixmap != NULL)
    delete forepixmap;

  if (backpixmap != NULL)
    delete backpixmap;

  if (lineColorDialog != NULL)
    delete lineColorDialog;

  if (barColorDialog != NULL)
    delete barColorDialog;

  if (foreColorDialog != NULL)
    delete foreColorDialog;

  if (backColorDialog != NULL)
    delete backColorDialog;

  if (backicon != NULL)
    delete backicon;

  if (foreicon != NULL)
    delete foreicon;

  delete ui;

  foreach (const ToolchainRec *rec, toolchains)
    delete rec;
}

// set chart's style
void
OptionsDialog::setChartStyle (int style)
{
  if (style == QTACHART_CANDLE)
  {
    lineChart->setCheckState (Qt::Unchecked);
    candleChart->setCheckState (Qt::Checked);
    heikinAshi->setCheckState (Qt::Unchecked);
    barChart->setCheckState (Qt::Unchecked);
  }

  if (style == QTACHART_HEIKINASHI)
  {
    lineChart->setCheckState (Qt::Unchecked);
    candleChart->setCheckState (Qt::Unchecked);
    heikinAshi->setCheckState (Qt::Checked);
    barChart->setCheckState (Qt::Unchecked);
  }

  if (style == QTACHART_LINE)
  {
    lineChart->setCheckState (Qt::Checked);
    candleChart->setCheckState (Qt::Unchecked);
    heikinAshi->setCheckState (Qt::Unchecked);
    barChart->setCheckState (Qt::Unchecked);
  }

  if (style == QTACHART_BAR)
  {
    lineChart->setCheckState (Qt::Unchecked);
    candleChart->setCheckState (Qt::Unchecked);
    heikinAshi->setCheckState (Qt::Unchecked);
    barChart->setCheckState (Qt::Checked);
  }
}

// get chart style
int
OptionsDialog::ChartStyle (void)
{
  if (lineChart->checkState () == Qt::Checked)
    return QTACHART_LINE;

  if (candleChart->checkState () == Qt::Checked)
    return QTACHART_CANDLE;

  if (heikinAshi->checkState () == Qt::Checked)
    return QTACHART_HEIKINASHI;

  if (barChart->checkState () == Qt::Checked)
    return QTACHART_BAR;

  return QTACHART_CANDLE;

}

// set grid on/off
void
OptionsDialog::setGrid (bool boolean)
{
  if (boolean)
    showGrid->setCheckState (Qt::Checked);
  else
    showGrid->setCheckState (Qt::Unchecked);
}

// get grid
bool OptionsDialog::Grid (void)
{
  if (showGrid->checkState () == Qt::Checked)
    return true;

  return false;
}

// set line color
void
OptionsDialog::setLineColor (QColor color)
{
  linecolor = color;
  lineColorDialog->setCurrentColor (color);
  linepixmap->fill (color);
  lineicon->addPixmap (*linepixmap, QIcon::Normal, QIcon::On);
  lineColorButton->setIcon (*lineicon);
}

// set bar color
void
OptionsDialog::setBarColor (QColor color)
{
  barcolor = color;
  barColorDialog->setCurrentColor (color);
  barpixmap->fill (color);
  baricon->addPixmap (*barpixmap, QIcon::Normal, QIcon::On);
  barColorButton->setIcon (*baricon);
}

// set foreground color
void
OptionsDialog::setForeColor (QColor color)
{
  forecolor = color;
  foreColorDialog->setCurrentColor (color);
  forepixmap->fill (color);
  foreicon->addPixmap (*forepixmap, QIcon::Normal, QIcon::On);
  foreColorButton->setIcon (*foreicon);
}

// set background color
void
OptionsDialog::setBackColor (QColor color)
{
  backcolor = color;
  backColorDialog->setCurrentColor (backcolor);
  backpixmap->fill (backcolor);
  backicon->addPixmap (*backpixmap, QIcon::Normal, QIcon::On);
  backColorButton->setIcon (*backicon);
}

// set volumes on/off
void
OptionsDialog::setVolumes (bool boolean)
{
  if (boolean)
    showVolumes->setCheckState (Qt::Checked);
  else
    showVolumes->setCheckState (Qt::Unchecked);
}

// set linear scale
void
OptionsDialog::setLinearScale (bool boolean)
{
  if (boolean)
    linearScale->setCheckState (Qt::Checked);
  else
    linearScale->setCheckState (Qt::Unchecked);
}

// get linear scale
bool
OptionsDialog::LinearScale (void)
{
  if (linearScale->checkState () == Qt::Checked)
    return true;

  return false;
}

// set online price
void
OptionsDialog::setOnlinePrice (bool boolean)
{
  if (boolean)
    onlinePrice->setCheckState (Qt::Checked);
  else
    onlinePrice->setCheckState (Qt::Unchecked);
}

// get online price
bool
OptionsDialog::OnlinePrice (void)
{
  if (onlinePrice->checkState () == Qt::Checked)
    return true;

  return false;
}

// get volumes
bool
OptionsDialog::Volumes (void)
{
  if (showVolumes->checkState () == Qt::Checked)
    return true;

  return false;
}

// load supported os/compiler combinations for cgscript
void
OptionsDialog::loadPlatforms ()
{
  QString SQL,
          OS = QStringLiteral ("unknown"),
          BITS = QString::number (sizeof (void *) * 8);
  int rc;

#ifdef Q_OS_WIN
  OS = QStringLiteral ("win");
#endif

#ifdef Q_OS_LINUX
  OS = QStringLiteral ("linux");
#endif

#ifdef Q_OS_MAC
  OS = QStringLiteral ("darwin");
#endif

  SQL = QStringLiteral ("SELECT * FROM toolchains WHERE platform LIKE '") %
                        OS % BITS % QStringLiteral ("%' ORDER BY platform;");

  rc = selectfromdb (SQL.toUtf8 (), sqlcb_toolchains,
                     static_cast <void *> (&toolchains));
  if (rc != SQLITE_OK)
    return; // don't do anything if SELECT fails, let the flow to continue

  ui->platformComboBox->clear ();
  foreach (const ToolchainRec *rec, toolchains)
    ui->platformComboBox->addItem (rec->platform);
}

// load application options
void
OptionsDialog::loadOptions (void)
{
  QStringList symbol, feed;
  int row, nrows, col, ncols;
  CG_ERR_RESULT result;

#if 0
  result = loadAppOptions (Application_Options);
  if (result != CG_ERR_OK)
  {
    showMessage (errorMessage (result), this);
    this->hide ();
    return;
  }
#endif

  result = gDatabase->loadTickerSymbols (symbol, feed);
  if (result != CG_ERR_OK)
  {
    showMessage (errorMessage (result), this);
    this->hide ();
    return;
  }

  nrows = ui->tickerTableWidget->rowCount ();
  ncols = ui->tickerTableWidget->columnCount ();
  for (row = 0; row < nrows; row ++)
    for (col = 0; col < ncols; col ++)
      delete ui->tickerTableWidget->takeItem(row,col);

  ui->tickerTableWidget->setSortingEnabled (false);
  ui->tickerTableWidget->setRowCount (symbol.size ());

  for (qint32 counter = 0; counter < symbol.size (); counter++)
  {
    QTableWidgetItem *item;

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (symbol[counter]);
    ui->tickerTableWidget->setItem(counter,0,item);

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (feed[counter]);
    ui->tickerTableWidget->setItem(counter,1,item);
  }

  ui->tickerTableWidget->resizeColumnToContents (0);
  ui->tickerTableWidget->resizeColumnToContents (1);

  for (qint32 counter = 0; counter <ui->tabWidget->count (); counter ++)
  {
    QObjectList objectlist;

    // objectlist = ui->tabWidget->widget (counter)->children ();
    objectlist = familyTree (ui->tabWidget->widget (counter));
    foreach (const QObject *object, objectlist)
    {
      QString objName = object->objectName ();

      if (objName == QLatin1String ("avapikeyEdit"))
        ((QLineEdit *) object)->setText (Application_Options->avapikey);

      if (objName == QLatin1String ("iexapikeyEdit"))
        ((QLineEdit *) object)->setText (Application_Options->iexapikey);

      if (objName == QLatin1String ("splashCheckBox"))
      {
        if (Application_Options->showsplashscreen)
          ((QCheckBox *) object)->setCheckState (Qt::Checked);
        else
          ((QCheckBox *) object)->setCheckState (Qt::Unchecked);
      }

      if (objName == QLatin1String ("newVersionCheckBox"))
      {
        if (Application_Options->checknewversion)
          ((QCheckBox *) object)->setCheckState (Qt::Checked);
        else
          ((QCheckBox *) object)->setCheckState (Qt::Unchecked);
      }

      if (objName == QLatin1String ("londonPricesCheckBox"))
      {
        if (Application_Options->longbp)
          ((QCheckBox *) object)->setCheckState (Qt::Checked);
        else
          ((QCheckBox *) object)->setCheckState (Qt::Unchecked);
      }

      if (objName == QLatin1String ("autoUpdateCheckBox"))
      {
        if (Application_Options->autoupdate)
          ((QCheckBox *) object)->setCheckState (Qt::Checked);
        else
          ((QCheckBox *) object)->setCheckState (Qt::Unchecked);
      }

      if (objName == QLatin1String ("devModeCheckBox"))
      {
        if (Application_Options->devmode)
          ((QCheckBox *) object)->setCheckState (Qt::Checked);
        else
          ((QCheckBox *) object)->setCheckState (Qt::Unchecked);
      }

      if (objName == QLatin1String ("enableProxyBox"))
      {
        if (Application_Options->enableproxy)
          ((QCheckBox *) object)->setCheckState (Qt::Checked);
        else
          ((QCheckBox *) object)->setCheckState (Qt::Unchecked);
      }

      if (objName == QLatin1String ("proxyHostEdit"))
        ((QLineEdit *) object)->setText (Application_Options->proxyhost);

      if (objName == QLatin1String ("proxyUserEdit"))
        ((QLineEdit *) object)->setText (Application_Options->proxyuser);

      if (objName == QLatin1String ("proxyPasswordEdit"))
        ((QLineEdit *) object)->setText (Application_Options->proxypass);

      if (objName == QLatin1String ("platformEdit"))
      {
        ((QLineEdit *) object)->setText (Application_Options->platform);
        ((QLineEdit *) object)->setCursorPosition (0);
      }

      if (objName == QLatin1String ("compilerEdit"))
      {
        ((QLineEdit *) object)->setText (Application_Options->compiler);
        ((QLineEdit *) object)->setCursorPosition (0);
      }

      if (objName == QLatin1String ("compilerDbgEdit"))
      {
        ((QLineEdit *) object)->setText (Application_Options->compilerdbg);
        ((QLineEdit *) object)->setCursorPosition (0);
      }

      if (objName == QLatin1String ("compilerRelEdit"))
      {
        ((QLineEdit *) object)->setText (Application_Options->compilerrel);
        ((QLineEdit *) object)->setCursorPosition (0);
      }

      if (objName == QLatin1String ("linkerEdit"))
      {
        ((QLineEdit *) object)->setText (Application_Options->linker);
        ((QLineEdit *) object)->setCursorPosition (0);
      }

      if (objName == QLatin1String ("CGScriptEdit"))
      {
        ((QLineEdit *) object)->setText (_CGSCRIPT_VERSION_STR);
        ((QLineEdit *) object)->setCursorPosition (0);
      }

      if (objName == QLatin1String ("linkerDbgEdit"))
        ((QLineEdit *) object)->setText (Application_Options->linkerdbg);

      if (objName == QLatin1String ("linkerRelEdit"))
      {
        ((QLineEdit *) object)->setText (Application_Options->linkerrel);
        ((QLineEdit *) object)->setCursorPosition (0);
      }

      if (objName == QLatin1String ("proxyPortSpinBox"))
        ((QSpinBox *) object)->setValue (Application_Options->proxyport);

      if (objName == QLatin1String ("netTimeOutSpinBox"))
        ((QSpinBox *) object)->setValue (Application_Options->nettimeout);

      if (objName == QLatin1String ("scrollSpeedSpinBox"))
        ((QSpinBox *) object)->setValue (Application_Options->scrollspeed);
    }
  }

  NetService::applyProxyOptions(Application_Options);

  setChartStyle (Application_Options->chart.style);
  setLineColor (QRgb (Application_Options->chart.lineColor));
  setForeColor (QRgb (Application_Options->chart.foreColor));
  setBarColor (QRgb (Application_Options->chart.barColor));
  setBackColor (QRgb (Application_Options->chart.backColor));
  setGrid (Application_Options->chart.showGrid);
  setVolumes (Application_Options->chart.showVolume);
  setOnlinePrice (Application_Options->chart.showOnlinePrice);
  setLinearScale (Application_Options->chart.linearScale);

  int idx = ui->platformComboBox->findText (ui->platformEdit->text ());
  ui->platformComboBox->setCurrentIndex (idx);
}

// save application options
void
OptionsDialog::saveOptions ()
{
  QStringList symbol, feed;
  QObjectList objectlist;
  CG_ERR_RESULT result;
  int maxrow;

  for (qint32 counter = 0; counter <ui->tabWidget->count (); counter ++)
  {
    // objectlist = ui->tabWidget->widget (counter)->children ();
    objectlist = familyTree (ui->tabWidget->widget (counter));
    foreach (const QObject *object, objectlist)
    {
      if (object->objectName () == QLatin1String ("avapikeyEdit"))
        Application_Options->avapikey = ((QLineEdit *) object)->text ().trimmed ();

      if (object->objectName () == QLatin1String ("iexapikeyEdit"))
        Application_Options->iexapikey = ((QLineEdit *) object)->text ().trimmed ();

      if (object->objectName () == QLatin1String ("splashCheckBox"))
      {
        if (((QCheckBox *) object)->checkState () == Qt::Checked)
          Application_Options->showsplashscreen = true;
        else
          Application_Options->showsplashscreen = false;
      }

      if (object->objectName () == QLatin1String ("newVersionCheckBox"))
      {
        if (((QCheckBox *) object)->checkState () == Qt::Checked)
          Application_Options->checknewversion = true;
        else
          Application_Options->checknewversion = false;
      }

      if (object->objectName () == QLatin1String ("londonPricesCheckBox"))
      {
        if (((QCheckBox *) object)->checkState () == Qt::Checked)
          Application_Options->longbp = true;
        else
          Application_Options->longbp = false;
      }

      if (object->objectName () == QLatin1String ("autoUpdateCheckBox"))
      {
        if (((QCheckBox *) object)->checkState () == Qt::Checked)
          Application_Options->autoupdate = true;
        else
          Application_Options->autoupdate = false;
      }

      if (object->objectName () == QLatin1String ("devModeCheckBox"))
      {
        if (((QCheckBox *) object)->checkState () == Qt::Checked)
          Application_Options->devmode = true;
        else
          Application_Options->devmode = false;

        (qobject_cast <MainWindow *> (parentWidget ()))->setDevMode (Application_Options->devmode);
      }

      if (object->objectName () == QLatin1String ("enableProxyBox"))
      {
        if (((QCheckBox *) object)->checkState () == Qt::Checked)
          Application_Options->enableproxy = true;
        else
          Application_Options->enableproxy = false;
      }

      if (object->objectName () == QLatin1String ("compilerEdit"))
        Application_Options->compiler = ((QLineEdit *) object)->text ().replace ("'", " ").trimmed ();

      if (object->objectName () == QLatin1String ("compilerDbgEdit"))
        Application_Options->compilerdbg = ((QLineEdit *) object)->text ().replace ("'", " ").trimmed ();

      if (object->objectName () == QLatin1String ("compilerRelEdit"))
        Application_Options->compilerrel = ((QLineEdit *) object)->text ().replace ("'", " ").trimmed ();

      if (object->objectName () == QLatin1String ("linkerEdit"))
        Application_Options->linker = ((QLineEdit *) object)->text ().replace ("'", " ").trimmed ();

      if (object->objectName () == QLatin1String ("linkerDbgEdit"))
        Application_Options->linkerdbg = ((QLineEdit *) object)->text ().replace ("'", " ").trimmed ();

      if (object->objectName () == QLatin1String ("linkerRelEdit"))
        Application_Options->linkerrel = ((QLineEdit *) object)->text ().replace ("'", " ").trimmed ();

      if (object->objectName () == QLatin1String ("proxyHostEdit"))
        Application_Options->proxyhost = ((QLineEdit *) object)->text ().replace ("'", " ").trimmed ();

      if (object->objectName () == QLatin1String ("proxyUserEdit"))
        Application_Options->proxyuser = ((QLineEdit *) object)->text ().replace ("'", " ").trimmed ();

      if (object->objectName () == QLatin1String ("proxyPasswordEdit"))
        Application_Options->proxypass = ((QLineEdit *) object)->text ().replace ("'", " ").trimmed ();

      if (object->objectName () == QLatin1String ("proxyPortSpinBox"))
        Application_Options->proxyport = (qint16) ((QSpinBox *) object)->value ();

      if (object->objectName () == QLatin1String ("netTimeOutSpinBox"))
        Application_Options->nettimeout = (qint16) ((QSpinBox *) object)->value ();

      if (object->objectName () == QLatin1String ("scrollSpeedSpinBox"))
        Application_Options->scrollspeed = (qint16) ((QSpinBox *) object)->value ();
    }
  }

  Application_Options->chart.lineColor = linecolor.rgb();
  Application_Options->chart.barColor = barcolor.rgb();
  Application_Options->chart.foreColor = forecolor.rgb();
  Application_Options->chart.backColor = backcolor.rgb();
  Application_Options->chart.style = ChartStyle ();
  Application_Options->chart.showGrid = Grid ();
  Application_Options->chart.showVolume = Volumes ();
  Application_Options->chart.linearScale = LinearScale ();
  Application_Options->chart.showOnlinePrice = OnlinePrice ();

  result = saveAppOptions (Application_Options);
  if (result != CG_ERR_OK)
  {
    showMessage (errorMessage (result), this);
    this->hide ();
    return;
  }

  maxrow = ui->tickerTableWidget->rowCount ();
  for (qint32 row = 0; row < maxrow; row ++)
  {
    symbol << ui->tickerTableWidget->item (row, 0)->text ();
    feed << ui->tickerTableWidget->item (row, 1)->text ();
  }

  result = gDatabase->saveTickerSymbols (symbol, feed);
  if (result != CG_ERR_OK)
  {
    showMessage (errorMessage (result), this);
    this->hide ();
    return;
  }
}

// signals
void
OptionsDialog::ok_clicked ()
{
  StockTicker *ticker;
  saveOptions ();
  loadOptions ();

  ticker = (qobject_cast <MainWindow *> (parentWidget ()))->getTicker ();
  if (ticker != NULL)
    ticker->setSpeed (ui->scrollSpeedSpinBox->value ());

  this->hide ();
}

void
OptionsDialog::cancel_clicked ()
{
  loadOptions ();
  this->hide ();
}

void
OptionsDialog::newVersionBtn_clicked ()
{
  MainWindow *mainwindow;

  mainwindow = qobject_cast <MainWindow *> (this->parentWidget ());
  mainwindow->checkNewVersion ();
  delay (2);
  if (mainwindow->newversion)
  {
    if (showDownloadMessage ())
      QDesktopServices::openUrl (QUrl (APPWEBPAGE));
  }
  else
    showMessage ("You run the current version for your platform.", this);
}

void
OptionsDialog::addBtn_clicked ()
{
  ui->addSymbolFrame->setEnabled (true);
  ui->symbolEdit->setFocus (Qt::OtherFocusReason);
}

void
OptionsDialog::symboladd_accepted ()
{
  YahooFeed yfeed;
  IEXFeed ifeed;
  QString smbl, name, market, feed, currency;
  int maxrow, row;
  bool duplicate = false, exists, valid, add = true;

  ui->addSymbolFrame->setEnabled (false);
  smbl = ui->symbolEdit->text ().trimmed ().toUpper ();
  if (smbl.size () == 0)
    return;

  this->setEnabled (false);

  // duplicate check
  maxrow = ui->tickerTableWidget->rowCount ();
  for (row = maxrow - 1; row >= 0; row --)
    if (ui->tickerTableWidget->item (row, 0)->text () == smbl)
      duplicate = true;

  if (duplicate == true)
  {
    showMessage ("Symbol already in ticker.", this);
    ui->symbolEdit->setText ("");
    this->setEnabled (true);
    return;
  }

  // check existence
  if (ui->googleCheckBox->checkState () == Qt::Checked)
  {
    feed = QStringLiteral ("IEX");
    valid = ifeed.validSymbol (smbl);
    if (valid == false)
    {
      showMessage ("Invalid symbol.", this);
      ui->symbolEdit->setText ("");
      this->setEnabled (true);
      return;
    }

    exists = ifeed.symbolExistence (smbl, name, market);
    if (exists == false)
      add = showOkCancel ("Symbol does not exist. Add anyway?", this);
  }
  else
  {
    feed = QStringLiteral ("YAHOO");
    valid = yfeed.validSymbol (smbl);
    if (valid == false)
    {
      showMessage ("Invalid symbol.", this);
      ui->addButton->setEnabled (true);
      return;
    }

    exists = yfeed.symbolExistence (smbl, name, market, currency);
    if (exists == false)
      add = showOkCancel ("Symbol does not exist. Add anyway?", this);
  }

  if (add)
  {
    QTableWidgetItem *item;

    ui->tickerTableWidget->insertRow (maxrow);
    item = new QTableWidgetItem (QTableWidgetItem::Type);
    ui->tickerTableWidget->setItem (maxrow, 0, item);
    item = new QTableWidgetItem (QTableWidgetItem::Type);
    ui->tickerTableWidget->setItem (maxrow, 1, item);
    ui->tickerTableWidget->item (maxrow, 0)->setText (smbl);
    ui->tickerTableWidget->item (maxrow, 1)->setText (feed);
    ui->tickerTableWidget->scrollToBottom ();
  }

  ui->symbolEdit->setText ("");
  this->setEnabled (true);
}

void
OptionsDialog::symboladd_rejected ()
{
  ui->addSymbolFrame->setEnabled (false);
  ui->symbolEdit->setText ("");

}

void
OptionsDialog::minusBtn_clicked ()
{
  QStringList selected_symbols;
  int row, maxrow;

  maxrow = ui->tickerTableWidget->rowCount ();
  for (row = 0; row < maxrow; row ++)
    if (ui->tickerTableWidget->item (row, 0)->isSelected ())
      selected_symbols << ui->tickerTableWidget->item (row, 0)->text ();

  ui->tickerTableWidget->clearSelection ();

  if (selected_symbols.size () == 0)
  {
    showMessage ("Select symbols first please.", this);
    return;
  }

  if (showOkCancel ("Delete selected symbols?", this) == false)
    return;

  foreach (const QString smbl, selected_symbols)
  {
    maxrow = ui->tickerTableWidget->rowCount ();
    for (row = maxrow - 1; row >= 0; row --)
      if (ui->tickerTableWidget->item (row, 0)->text () == smbl)
      {
        delete ui->tickerTableWidget->takeItem(row, 0);
        delete ui->tickerTableWidget->takeItem(row, 1);
        ui->tickerTableWidget->removeRow (row);
      }
  }
}

void
OptionsDialog::googlefeed_clicked (int state)
{
  if (state == Qt::Checked)
    ui->yahooCheckBox->setCheckState (Qt::Unchecked);
  else
    ui->yahooCheckBox->setCheckState (Qt::Checked);
}

void
OptionsDialog::yahoofeed_clicked (int state)
{
  if (state == Qt::Checked)
    ui->googleCheckBox->setCheckState (Qt::Unchecked);
  else
    ui->googleCheckBox->setCheckState (Qt::Checked);
}

// down
void
OptionsDialog::downButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tickerTableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
  ui->tickerTableWidget->setFocus (Qt::MouseFocusReason);
}

// up
void
OptionsDialog::upButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tickerTableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
  ui->tickerTableWidget->setFocus (Qt::MouseFocusReason);
}

// line color button clicked
void
OptionsDialog::lineColorButton_clicked (void)
{
  lineColorDialog->setCurrentColor (linecolor);
  lineColorDialog->show ();
  // lineColorDialog->open ();
  linecolor = lineColorDialog->selectedColor ();
  linepixmap->fill (linecolor);
}

// bar color button clicked
void
OptionsDialog::barColorButton_clicked (void)
{
  barColorDialog->setCurrentColor (barcolor);
  barColorDialog->move ((width () - barColorDialog->width ()) / 2,
                        (height () - barColorDialog->height ()) / 2);
  barColorDialog->show ();
  // barColorDialog->open ();
  barcolor = barColorDialog->selectedColor ();
  barpixmap->fill (barcolor);
}

// foreground color button clicked
void
OptionsDialog::foreColorButton_clicked (void)
{
  foreColorDialog->setCurrentColor (forecolor);
  foreColorDialog->move ((width () - foreColorDialog->width ()) / 2,
                         (height () - foreColorDialog->height ()) / 2);
  foreColorDialog->show ();
  // foreColorDialog->open ();
  forecolor = foreColorDialog->selectedColor ();
  forepixmap->fill (forecolor);
}

// background color button clicked
void
OptionsDialog::backColorButton_clicked (void)
{
  backColorDialog->setCurrentColor (backcolor);
  backColorDialog->move ((width () - backColorDialog->width ()) / 2,
                         (height () - backColorDialog->height ()) / 2);
  backColorDialog->show ();
  // backColorDialog->open ();
  backcolor = backColorDialog->selectedColor ();
  backpixmap->fill (backcolor);
}

// line color dialog done
void
OptionsDialog::lineColorDialog_finished ()
{
  linecolor = lineColorDialog->currentColor ();
  linepixmap->fill (linecolor);
  lineicon->addPixmap (*linepixmap, QIcon::Normal, QIcon::On);
  lineColorButton->setIcon (*lineicon);
}

// bar color dialog done
void
OptionsDialog::barColorDialog_finished ()
{
  barcolor = barColorDialog->currentColor ();
  barpixmap->fill (barcolor);
  baricon->addPixmap (*barpixmap, QIcon::Normal, QIcon::On);
  barColorButton->setIcon (*baricon);
}

// foreground color dialog done
void
OptionsDialog::foreColorDialog_finished ()
{
  forecolor = foreColorDialog->currentColor ();
  forepixmap->fill (forecolor);
  foreicon->addPixmap (*forepixmap, QIcon::Normal, QIcon::On);
  foreColorButton->setIcon (*foreicon);
}

// background color dialog done
void
OptionsDialog::backColorDialog_finished ()
{
  backcolor = backColorDialog->currentColor ();
  backpixmap->fill (backcolor);
  backicon->addPixmap (*backpixmap, QIcon::Normal, QIcon::On);
  backColorButton->setIcon (*backicon);
}

// apply platform
void
OptionsDialog::applyPlatformBtn_clicked (void)
{
   foreach (const ToolchainRec *rec, toolchains)
   {
     if (rec->platform == ui->platformComboBox->currentText ())
     {
       Application_Options->platform = rec->platform;
       ui->platformEdit->setText (Application_Options->platform);
       ui->platformEdit->setCursorPosition (0);

       Application_Options->compiler = rec->compiler;
       ui->compilerEdit->setText (Application_Options->compiler);
       ui->compilerEdit->setCursorPosition (0);

       Application_Options->compilerdbg = rec->compilerdbg;

       Application_Options->compilerrel = rec->compilerrel;

       Application_Options->linker = rec->linker;
       ui->linkerEdit->setText (Application_Options->linker);
       ui->linkerEdit->setCursorPosition (0);

       Application_Options->linkerdbg = rec->linkerdbg;

       Application_Options->linkerrel = rec->linkerrel;
     }
   }
}

/// events
void
OptionsDialog::closeEvent (QCloseEvent * event)
{
  Q_UNUSED (event);
  saveOptions ();
  loadOptions ();
}

void
OptionsDialog::showEvent (QShowEvent * event)
{
  Q_UNUSED (event);
}

// delete key
void
OptionsDialog::keyPressEvent (QKeyEvent * event)
{
  if (ui->tabWidget->tabText (ui->tabWidget->currentIndex ()) == QLatin1String ("Ticker"))
    if (event->key () == Qt::Key_Delete)
      minusBtn_clicked ();
}

