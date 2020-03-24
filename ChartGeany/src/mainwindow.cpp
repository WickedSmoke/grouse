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
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QTime>
#include <QDir>
#include <QFontDatabase>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextStream>
#include <QTemporaryFile>
#include <QProgressBar>
#include <QDateTime>
#include <QColor>
#include <QPixmap>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QLayout>
#include <QMainWindow>
#include <QSettings>

#include "netservice.h"
#include "sqlite3.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "splashdialog.h"
#include "qtachart.h"
#include "portfolio.h"
#include "editorwidget.h"
#include "debugdialog.h"
#include "unix_signals.h"
#include "cgscript.h"

const char DEFAULT_FONT_FAMILY[] = "Tahoma";
#ifdef Q_OS_MAC
const int  FONT_POINTSIZE_PAD = 3;
const int  FONT_PIXELSIZE_PAD = 3;
#else
const int  FONT_POINTSIZE_PAD = 1;
const int  FONT_PIXELSIZE_PAD = 1;
#endif
const int  CHART_FONT_SIZE_PAD = 3;

AppSettings *Application_Settings;
SQLists *ComboItems;
DownloadDataDialog *downloaddatadialog;
TemplateManagerDialog *templatemanager;
ProgressDialog *progressdialog;
DebugDialog *debugdialog;
QProgressBar *GlobalProgressBar;
static SplashDialog *splash = nullptr;
QString Year, Month, Day, UID, RunCounter;
QMutex *ResourceMutex = nullptr;
size_t CGScriptFunctionRegistrySize;

int NCORES;
bool FULL = false;


#define CREATE_DIALOG(ptr,T) \
  if(! ptr) { \
    ptr = new T(this); \
    if(! ptr) return; \
  }


// load application fonts
void
MainWindow::loadFonts ()
{
  QFile file;
  QByteArray barray;

  file.setFileName (QStringLiteral (":/fonts/fonts/tahoma.ttf"));
  if (file.open(QIODevice::ReadOnly))
  {
    barray.clear ();
    barray = file.readAll ();
    QFontDatabase::addApplicationFontFromData (barray);
    file.close ();
  }

  barray.clear ();
}

// check if there is new version available
void
MainWindow::checkNewVersion ()
{
  QTemporaryFile tempFile;      // temporary file
  QTextStream in;
  QString line, urlstr;
  NetService *netservice = nullptr;
  CG_ERR_RESULT ioresult = CG_ERR_OK;

  newversion = false;

  urlstr =
#ifdef Q_OS_LINUX
    QStringLiteral ("https://chart-geany.sourceforge.io/version/linux/current.txt");
#else
#ifdef Q_OS_WIN
    QStringLiteral ("https://chart-geany.sourceforge.io/version/windows/current.txt");
#else
#ifdef Q_OS_MAC
    QStringLiteral ("https://chart-geany.sourceforge.io/version/mac/current.txt");
#endif
#endif
#endif

  // open temporary file
  if (!tempFile.open ())
    return;

  tempFile.resize (0);

  netservice = new NetService (Application_Settings->options.nettimeout,
                               nativeHttpHeader ().toLatin1 (), this);
  ioresult = netservice->httpGET (urlstr, tempFile, nullptr);
  if (ioresult != CG_ERR_OK)
    return;

  in.setDevice (&tempFile);
  in.seek (0);
  line = in.readAll ();

  if (line.size () < 1)
    return;

  QStringList versiondigits = line.split (QStringLiteral ("."), QString::SkipEmptyParts);
  if (versiondigits.length () == 3)
  {
    if (versiondigits[0].toInt () > VERSION_MAJOR)
    {
      newversion = true;
      return;
    }

    if (versiondigits[0].toInt () < VERSION_MAJOR)
      return;

    if (versiondigits[1].toInt () > VERSION_MINOR)
    {
      newversion = true;
      return;
    }

    if (versiondigits[1].toInt () < VERSION_MINOR)
      return;

    if (versiondigits[2].toInt () > VERSION_PATCH)
    {
      newversion = true;
      return;
    }
  }
}

// constructor
MainWindow::MainWindow (QWidget * parent):
  QMainWindow (parent), ui (new Ui::MainWindow),
  loadcsvdlg(nullptr), portfoliomanagerdlg(nullptr),
  optionsdlg(nullptr), infodlg(nullptr)
{
  Q_UNUSED (QTACastFromConstVoid)

  const QString stylesheet =
    QStringLiteral ("background: transparent; background-color: white; color:black");
  QDateTime datetime;
  QFile initcopy;
  const char* openError;

  GlobalError = CG_ERR_OK;
  ResourceMutex = new QMutex (QMutex::NonRecursive);

  ui->setupUi (this);
  setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );


  // set the db path
  QString dbfile = QDir::homePath() % QDir::separator() % QStringLiteral(".config") % QDir::separator() % APPDIR % QDir::separator() % DBNAME;
  if( ! idb.openFile( dbfile, &openError ) )
  {
    showMessage ( QString(openError).append(" Application quits.") );
    qApp->exit (1);

#if defined (Q_OS_WIN) || defined (Q_OS_MAC)
    exit (1);
#else
    quick_exit (1);
#endif
  }

  ticker = nullptr;
  expandedChartFlag = false;
  tickerVisible = false;
  newversion = false;
  setWindowTitle (QApplication::applicationName ());
  this->setStatusBar ( nullptr );
  this->setStyleSheet ("background-color:white;");
  tabWidget = ui->tabWidget;
  ui->tabWidget->clear ();
  ui->tabWidget->setUsesScrollButtons (true);
  ui->tabWidget->setStyleSheet ("QTabBar::tab { height: 25px;}");
  ui->tabWidget->setDocumentMode (false);

  ui->developButton->setVisible (false);
  ui->debugButton->setVisible (false);
  // ui->modulesButton->setVisible (false);

  ui->managerButton->setStyleSheet (stylesheet);
  ui->screenshotButton->setStyleSheet (stylesheet);
  ui->infoButton->setStyleSheet (stylesheet);
  ui->homeButton->setStyleSheet (stylesheet);
  ui->optionsButton->setStyleSheet (stylesheet);
  ui->tickerButton->setStyleSheet (stylesheet);
  ui->portfolioButton->setStyleSheet (stylesheet);
  ui->exitButton->setStyleSheet (stylesheet);
  ui->developButton->setStyleSheet (stylesheet);
  ui->debugButton->setStyleSheet (stylesheet);
  ui->modulesButton->setStyleSheet (stylesheet);

  // current year, month - 1, day
  datetime = QDateTime::currentDateTimeUtc ();
  Year = QString::number(datetime.date ().year ());
  Month = QString::number(datetime.date ().month () - 1);
  Day = QString::number(datetime.date ().day ());

  // connect to signals
  connect (ui->tabWidget, SIGNAL(tabCloseRequested(int)), this,
           SLOT(closeTab_clicked (int)));
  connect (ui->tabWidget, SIGNAL(currentChanged(int)), this,
           SLOT(currentTab_changed (int)));
  connect (ui->managerButton, SIGNAL (clicked ()), this,
           SLOT (managerButton_clicked ()));
  connect (ui->portfolioButton, SIGNAL (clicked ()), this,
           SLOT (portfolioButton_clicked ()));
  connect (ui->modulesButton, SIGNAL (clicked ()), this,
           SLOT (modulesButton_clicked ()));
  connect (ui->developButton, SIGNAL (clicked ()), this,
           SLOT (developButton_clicked ()));
  connect (ui->screenshotButton, SIGNAL (clicked ()), this,
           SLOT (screenshotButton_clicked ()));
  connect (ui->optionsButton, SIGNAL (clicked ()), this,
           SLOT (optionsButton_clicked ()));
  connect (ui->tickerButton, SIGNAL (clicked ()), this,
           SLOT (tickerButton_clicked ()));
  connect (ui->debugButton, SIGNAL (clicked ()), this,
           SLOT (debugButton_clicked ()));
  connect (ui->homeButton, SIGNAL (clicked ()), this,
           SLOT (homeButton_clicked ()));
  connect (ui->infoButton, SIGNAL (clicked ()), this,
           SLOT (infoButton_clicked ()));
  connect (ui->exitButton, SIGNAL (clicked ()), this,
           SLOT (exitButton_clicked ()));
  /*
  // catch session manager's signal
  QGuiApplication::setFallbackSessionManagementEnabled(false);
  connect(qApp, SIGNAL(commitDataRequest(QSessionManager)),
          SLOT(commitData(QSessionManager)));
  */

  // export application settings
  Application_Settings = &appsettings;

  // export classes and variables
  ComboItems = &comboitems;

  // load application's options
  loadAppOptions (&Application_Settings->options);

  // show developer mode buttons
  if (Application_Settings->options.devmode == true)
  {
    ui->developButton->setVisible (true);
    ui->debugButton->setVisible (true);
    // ui->modulesButton->setVisible (true);
  }

  // show splash
  if (Application_Settings->options.showsplashscreen == true)
  {
    splash = new SplashDialog (this);
    correctWidgetFonts (splash);
    splash->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, splash->size(), qApp->desktop()->availableGeometry()));
    splash->show ();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    delay (3);
  }

  // initialize SQL statements
  strcpy (comboitems.formats_query, "select FORMAT from FORMATS");
  strcpy (comboitems.timeframes_query, "select TIMEFRAME from TIMEFRAMES_ORDERED");
  strcpy (comboitems.currencies_query, "select SYMBOL from CURRENCIES");
  strcpy (comboitems.markets_query, "select MARKET from MARKETS");
  strcpy (comboitems.datafeeds_query, "select * from DATAFEEDS order by FEEDNAME");
  strcpy (comboitems.transactiontypes_query, "select * from TRANSACTIONTYPES");
  strcpy (comboitems.commissiontypes_query, "select * from COMMISSIONTYPES");

  // create widgets
  downloaddatadlg = new DownloadDataDialog (this);
  datamanagerdlg = new DataManagerDialog (this);
  debugdlg = new DebugDialog (this);
  modulemanagerdlg = new ModuleManagerDialog (this);
  progressdlg = new ProgressDialog (this);
  waitdlg = new WaitDialog;
  templatemanagerdlg = new TemplateManagerDialog (this);

  // load application fonts
  loadFonts ();

  // export classes and variables
  downloaddatadialog = downloaddatadlg;
  progressdialog = progressdlg;
  templatemanager = templatemanagerdlg;
  debugdialog = debugdlg;

  // check if some initialization failed
  if (GlobalError.fetchAndAddAcquire (0)!= CG_ERR_OK)
  {
    ui->managerButton->setEnabled (false);
    ui->screenshotButton->setEnabled (false);
    ui->infoButton->setEnabled (false);
    ui->optionsButton->setEnabled (false);
    ui->tabWidget->setEnabled (false);
  }

  // initially remove all tabs
  while (ui->tabWidget->count () > 1)
    ui->tabWidget->removeTab (0);

  ui->tabWidget->resize (width () - 2, height () - 60);

  newversion = false;
  if (Application_Settings->options.checknewversion == true)
  {
    // check for new vesrion
    checkNewVersion ();
    if (newversion && showDownloadMessage ())
      QDesktopServices::openUrl (QUrl (APPWEBPAGE));
  }

  waitdlg->setMessage (QString::fromUtf8 ("Exiting. Please wait..."));

#ifdef Q_OS_MAC
  NCORES = 1;
#else
  NCORES = QThread::idealThreadCount ();
  if (NCORES == -1)
    NCORES = 1;
#endif // Q_OS_MAC

  correctWidgetFonts (this);
  if (Application_Settings->options.showsplashscreen == true)
    splash->hide ();

  idb.disableModules();

  // initialize cgscript
  CGScriptFunctionRegistrySize = cgscript_init ();

  // connect to unix signals
#ifndef Q_OS_WIN
  UnixSignals *usignals = new UnixSignals (this);
  connect(usignals, SIGNAL(sigTerm()), this, SLOT(close()));
  connect(usignals, SIGNAL(sigHup()),  this, SLOT(close()));
  connect(usignals, SIGNAL(sigAbrt()), this, SLOT(close()));
#endif

  QSettings pref(APPDIR, APPNAME);
  QSize size = pref.value("main-window-size", QSize()).toSize();
  if (size.isValid())
  {
    resize(size);
    show();
  }
  else
  {
    showMaximized();
  }
}

// destructor
MainWindow::~MainWindow ()
{
  if (ticker != nullptr)
  {
    delete ticker;
    ticker = nullptr;
  }

  if (ui->tabWidget != nullptr)
  {
    delete ui->tabWidget;
    ui->tabWidget = nullptr;
  }

  QList<QWidget *> allWidgets = this->findChildren<QWidget *> ();

  foreach (QWidget *wid, allWidgets)
  {
    if (wid->objectName () == QLatin1String ("Chart") ||
        wid->objectName () == QLatin1String ("Editor"))
      delete wid;
  }

  if (waitdlg != nullptr)
  {
    delete waitdlg;
    waitdlg = nullptr;
  }

  delete ui;
}

/*
void
MainWindow::commitData(QSessionManager &manager)
{
  Q_UNUSED (manager);
}
*/

// enable ticker button
void
MainWindow::enableTickerButton ()
{
  ui->tickerButton->setEnabled (true);
};

// disable ticker button
void
MainWindow::disableTickerButton ()
{
  ui->tickerButton->setEnabled (false);
};

// add a new chart
CG_ERR_RESULT
MainWindow::addChart (TableDataVector & datavector)
{
  QList<QAbstractButton*> allPButtons;
  QTAChart *tachart;
  QTAChartData Data;
  QString SQLCmd, title, subtitle;
  CG_ERR_RESULT result = CG_ERR_OK;

  tachart = new (std::nothrow) QTAChart (ui->tabWidget);
  if (!tachart)
  {
    result = CG_ERR_NOMEM;
    setGlobalError(result, __FILE__, __LINE__);
    showMessage (errorMessage (result), this);
    return result;
  }

  if (tachart->getClassError () != CG_ERR_OK)
  {
    result = tachart->getClassError ();
    setGlobalError(result, __FILE__, __LINE__);
    showMessage (errorMessage (result), this);
    if (tachart != nullptr)
      delete tachart;
    return result;
  }

  foreach (const TableDataClass tdc, datavector)
  {
    tachart->loadFrames (tdc.tablename);
    if (tachart->getClassError () != CG_ERR_OK)
    {
      result = tachart->getClassError ();
      setGlobalError(result, __FILE__, __LINE__);
      showMessage (errorMessage (result), this);
      delete tachart;
      return result;
    }
  }

  tachart->setSymbolKey (datavector[0].tablename);
  tachart->setObjectName ("Chart");

  // load data
  result = idb.loadChartData(datavector[0].base, &Data);
  if( result != CG_ERR_OK )
  {
    delete tachart;
    showMessage (QStringLiteral ("Symbol ") % datavector[0].symbol % ": " % errorMessage (result), this);
    return result;
  }

  tachart->loadData (Data);
  tachart->setSymbol (datavector[0].symbol);
  tachart->setFeed (datavector[0].source);
  title = datavector[0].symbol;
  subtitle = datavector[0].name;

  tachart->setAlwaysRedraw (true);
  tachart->setTitle (title, subtitle);

  ui->tabWidget->addTab (tachart, datavector[0].symbol % QStringLiteral (" ") % (datavector[0].adjusted == QStringLiteral ("NO")?QStringLiteral ("RAW"):QStringLiteral ("ADJ")));
  ui->tabWidget->setCurrentIndex (ui->tabWidget->count () - 1);
  tachart->setTabText (datavector[0].symbol % QStringLiteral (" ") % (datavector[0].adjusted == QStringLiteral ("NO")?QStringLiteral ("RAW"):QStringLiteral ("ADJ")));

  // remove tooltip from close tab buttons
  allPButtons = ui->tabWidget->findChildren<QAbstractButton*> ();
  for (int ind = 0; ind < allPButtons.size(); ind++)
    if (allPButtons.at(ind)->inherits("CloseButton"))
      allPButtons.at(ind)->setToolTip(QStringLiteral (""));

  return result;
}

// add a new portfolio
CG_ERR_RESULT
MainWindow::addPortfolio (int pf_id, QString title, QString currency, QString feed)
{
  CG_ERR_RESULT result = CG_ERR_OK;
  Portfolio *portfolio;
  QStringList tabkeys;

  tabkeys = getTabKeys ("Portfolio");
  if (tabkeys.size () > 0)
  {
    for (qint32 counter = 0; counter < tabkeys.size (); counter ++)
    {
      if (tabkeys[counter].toInt () == pf_id)
      {
        ui->tabWidget->setCurrentIndex (counter);
        return result;
      }
    }
  }

  portfolio = new (std::nothrow) Portfolio (pf_id, ui->tabWidget);
  if (!portfolio)
  {
    result = CG_ERR_NOMEM;
    setGlobalError(result, __FILE__, __LINE__);
    showMessage (errorMessage (result), this);
    return result;
  }

  portfolio->setObjectName ("Portfolio");
  portfolio->setTitle (title);
  portfolio->setFeed (feed);
  portfolio->setCurrency (currency);
  ui->tabWidget->addTab (portfolio, title);
  ui->tabWidget->setCurrentIndex (ui->tabWidget->count () - 1);

  return result;
}

// expanded chart
bool
MainWindow::expandedChart () const NOEXCEPT
{
  return expandedChartFlag;
}

// expand/shrink chart
void
MainWindow::setExpandChart (bool expandflag)
{
  int pad;

  if (expandflag == expandedChartFlag)
    return;

  expandedChartFlag = expandflag;

  if (!expandedChartFlag)
    pad = 60;
  else
    pad = 0;

  if (tickerVisible)
    pad += TICKER_HEIGHT;

  int max = ui->tabWidget->count ();
  if (!expandflag)
  {
    if (max == 0)
      return;
    ui->managerButton->show ();
    ui->screenshotButton->show ();
    ui->optionsButton->show ();
    ui->tickerButton->show ();
    ui->portfolioButton->show ();
    ui->homeButton->show ();
    ui->infoButton->show ();
    ui->exitButton->show ();
    ui->modulesButton->show ();
    if (Application_Settings->options.devmode == true)
    {
      ui->debugButton->show ();
      ui->developButton->show ();
    }
    ui->tabWidget->move (0, 55);
    ui->tabWidget->resize (width () - 2, height () - pad);
  }
  else
  {
    ui->managerButton->hide ();
    ui->screenshotButton->hide ();
    ui->optionsButton->hide ();
    ui->tickerButton->hide ();
    ui->portfolioButton->hide ();
    ui->developButton->hide ();
    ui->homeButton->hide ();
    ui->infoButton->hide ();
    ui->exitButton->hide ();
    ui->debugButton->hide ();
    ui->modulesButton->hide ();
    ui->tabWidget->move (0, 0);
    ui->tabWidget->resize (width () - 2, height () - (pad + 5));

    if (max == 0)
      return;
  }

  for (qint32 counter = 0; counter < max; counter ++)
  {
    if (ui->tabWidget->widget(counter)->objectName () == QLatin1String ("Chart"))
      ui->tabWidget->widget(counter)->resize (ui->tabWidget->width () - 2,
                                              ui->tabWidget->height () - 20);
  }
}

void MainWindow::showCsvDialog()
{
  CREATE_DIALOG( loadcsvdlg, LoadCSVDialog )
  loadcsvdlg->show();
}

// set developer's mode
void
MainWindow::setDevMode (bool devmodeflag)
{
  if (devmodeflag)
  {
    ui->developButton->setVisible (true);
    ui->debugButton->setVisible (true);
    // ui->modulesButton->setVisible (true);
    return;
  }

  ui->developButton->setVisible (false);
  ui->debugButton->setVisible (false);
  // ui->modulesButton->setVisible (false);

  return;

}

// get the database keys of all open charts
QStringList
MainWindow::getTabKeys (QString type)
{
  static QStringList keys;
  int max = ui->tabWidget->count ();

  keys.clear ();

  if (max == 0)
    return keys;

  for (qint32 counter = 0; counter < max; counter ++)
  {
    QWidget *wid;

    wid = ui->tabWidget->widget(counter);
    if (wid->objectName () == type && type == QLatin1String ("Chart"))
    {
      QTAChart *chart;
      chart = qobject_cast <QTAChart *> (wid);
      keys += chart->getSymbolKey ();
    }
    else if (wid->objectName () == type && type == QLatin1String ("Portfolio"))
    {
      Portfolio *portfolio;
      portfolio = qobject_cast <Portfolio *> (wid);
      keys += QString::number (portfolio->id ());
    }
    else
      keys += QStringLiteral ("");
  }

  return keys;
}

/// Signals
///

// managerButton_clicked ()
void
MainWindow::managerButton_clicked ()
{
  datamanagerdlg->show ();
}

// portfolioButton_clicked ()
void
MainWindow::portfolioButton_clicked ()
{
  CREATE_DIALOG( portfoliomanagerdlg, PortfolioManagerDialog )
  portfoliomanagerdlg->show ();
}

// modulesButton_clicked ()
void
MainWindow::modulesButton_clicked ()
{
  modulemanagerdlg->show ();
}

// developButton_clicked ()
void
MainWindow::developButton_clicked ()
{
  EditorWidget *editor;
  editor = new (std::nothrow) EditorWidget (ui->tabWidget);
  editor->setObjectName ("Editor");
  ui->tabWidget->addTab (editor, "New Module.cgs");
  ui->tabWidget->setCurrentIndex (ui->tabWidget->count () - 1);
}

// debugButton_clicked ()
void
MainWindow::debugButton_clicked ()
{
  if (debugdlg == nullptr)
    return;
  debugdlg->show ();
  debugdlg->activateWindow ();
  debugdlg->raise ();
}

// managerButton_clicked ()
void
MainWindow::screenshotButton_clicked ()
{
  QFileDialog *fileDialog;
  QTAChart *chart;
  QString fileName = "";
  QPixmap screenshot;

  if (ui->tabWidget->count () == 0)
  {
    showMessage ("Open a chart first please.", this);
    return;
  }

  if (ui->tabWidget->widget(ui->tabWidget->currentIndex ())->objectName () != QLatin1String ("Chart"))
  {
    showMessage ("Screenshots available only for charts.", this);
    return;
  }

  fileDialog = new QFileDialog;
  correctTitleBar (fileDialog);
  correctWidgetFonts (fileDialog);
  chart = qobject_cast <QTAChart *> (ui->tabWidget->widget(ui->tabWidget->currentIndex ()));
  chart->setCustomBottomText (APPWEBPAGE);
  screenshot = QPixmap::grabWidget (chart);
  fileName = fileDialog->getSaveFileName(this, "Save chart", "", "Image (*.png)");

  if (fileName == "")
    goto screenshotButton_clicked_end;

  if (fileName.mid (fileName.size () - 4).toLower () != ".png")
    fileName += ".png";

  screenshot.save(fileName, "PNG");
  showMessage ("Screenshot saved.", this);

screenshotButton_clicked_end:
  chart->restoreBottomText ();
  delete fileDialog;

}

// exit_clicked ()
void
MainWindow::exitButton_clicked ()
{
  if( showOkCancel(QString("Quit %1?").arg(QApplication::applicationName()),
                   this) )
      close();
}

// options_clicked ()
void
MainWindow::optionsButton_clicked ()
{
#if 0   // #ifdef Q_OS_MAC
  if( optionsdlg )
  {
    /* this is a weird trick in order to keep os x running */
    optionsdlg->exec ();
    delete optionsdlg;
    optionsdlg = new (std::nothrow) OptionsDialog (this);
    if (optionsdlg == nullptr)
      return;
  }
#endif

  CREATE_DIALOG( optionsdlg, OptionsDialog )
  correctWidgetFonts (optionsdlg);
  optionsdlg->show();
}

// home_clicked ()
void
MainWindow::homeButton_clicked ()
{
  QDesktopServices::openUrl(QUrl(APPWEBPAGE));
}

// info_clicked ()
void
MainWindow::infoButton_clicked ()
{
  CREATE_DIALOG( infodlg, InfoDialog )
  infodlg->show ();
  infodlg->activateWindow ();
  infodlg->raise ();
}

// ticker_clicked ()
void
MainWindow::tickerButton_clicked ()
{
  QStringList lsymbol, lfeed;
  int pad = 0, max;

  if (tickerVisible == false)
  {
    CG_ERR_RESULT result;

    result = loadTickerSymbols (lsymbol, lfeed);
    if (result == CG_ERR_OK)
    {
      if (lsymbol.size () == 0)
      {
        showMessage ("No symbols found in ticker", this);
        return;
      }
    }
    else
    {
      showMessage (errorMessage (result), this);
      return;
    }
  }

  ui->tickerButton->setEnabled (false);

  if (tickerVisible)
  {
    tickerVisible = false;
    ticker->hide ();
    delete ticker;
    ticker = nullptr;
    ui->tickerButton->setEnabled (true);
  }
  else
  {
    tickerVisible = true;
    ticker = new StockTicker (this);

    if (GlobalError.fetchAndAddAcquire (0)== CG_ERR_OK)
    {
      ticker->setVisible (false);
      ticker->update ();
      this->layout ()->addWidget (ticker);
    }
    else
    {
      tickerVisible = false;
      ticker->setVisible (false);
      showMessage (errorMessage (GlobalError.fetchAndAddAcquire (0)), this);
      delete ticker;
      ticker = nullptr;
      ui->tickerButton->setEnabled (true);
    }

  }

  if (!expandedChartFlag)
    pad = 60;
  else
    pad = 0;

  if (tickerVisible)
    pad += TICKER_HEIGHT;

  if (!expandedChartFlag)
  {
    ui->managerButton->show ();
    ui->screenshotButton->show ();
    ui->optionsButton->show ();
    ui->homeButton->show ();
    ui->infoButton->show ();
    ui->exitButton->show ();
    ui->tabWidget->move (0, 55);
    ui->tabWidget->resize (width () - 2, height () - pad);
  }
  else
  {
    ui->managerButton->hide ();
    ui->screenshotButton->hide ();
    ui->optionsButton->hide ();
    ui->portfolioButton->hide ();
    ui->developButton->hide ();
    ui->homeButton->hide ();
    ui->infoButton->hide ();
    ui->exitButton->hide ();
    ui->tabWidget->move (0, 0);
    ui->tabWidget->resize (width () - 2, height () - (pad + 5));
  }

  if (tickerVisible)
  {
    ticker->move (0, height () - TICKER_HEIGHT);
    ticker->resize (width (), TICKER_HEIGHT);
    ticker->show ();
  }

  max = ui->tabWidget->count ();
  if (max > 0)
    for (qint32 counter = 0; counter < max; counter ++)
      ui->tabWidget->widget(counter)->resize (ui->tabWidget->width () - 2,
                                              ui->tabWidget->height () - 20);

  if (tickerVisible)
  {
    ticker->setGeometry (0, height () - (TICKER_HEIGHT + 2),
                         width (), TICKER_HEIGHT);
    ticker->show ();
  }
}

// closeTab_clicked ()
void
MainWindow::closeTab_clicked (int index)
{
  QWidget *wid;

  if (ui->tabWidget->count () == 1)
    setExpandChart (false);

  ui->tabWidget->widget (index)->hide ();
  wid = ui->tabWidget->widget (index);
  ui->tabWidget->removeTab (index);
  delete wid;
}

// currentTab_changed ()
void
MainWindow::currentTab_changed (int index)
{
  int max;

  max = ui->tabWidget->count ();
  if (max < 1)
    return;

  for (qint32 counter = 0; counter < max; counter ++)
    ui->tabWidget->findChild<QTabBar *> (QLatin1String("qt_tabwidget_tabbar"))->
    setTabTextColor (counter, QColor(Qt::black));

  ui->tabWidget->findChild<QTabBar *> (QLatin1String("qt_tabwidget_tabbar"))->
  setTabTextColor (index, QColor(Qt::blue));

  ui->tabWidget->widget (index)->setFocus (Qt::OtherFocusReason);
}

/// Events
///
void MainWindow::closeEvent(QCloseEvent *event)
{
  QSettings pref(APPDIR, APPNAME);
  QSize saveSize;

  if (!isMaximized())
    saveSize = size();
  pref.setValue("main-window-size", saveSize);
  event->accept();
}

// show
void
MainWindow::showEvent (QShowEvent * event)
{
  Q_UNUSED (event)
}

// resize
void
MainWindow::resizeEvent (QResizeEvent * event)
{
  QSize newsize;
  int max, pad;

  if (event->oldSize () == event->size ())
    return;

  newsize = event->size ();
  ui->homeButton->move (newsize.width () - 140, ui->infoButton->y ());
  ui->infoButton->move (newsize.width () - 95, ui->infoButton->y ());
  ui->exitButton->move (newsize.width () - 50, ui->exitButton->y ());

  if (!expandedChartFlag)
    pad = 60;
  else
    pad = 0;

  if (tickerVisible)
    pad += TICKER_HEIGHT;

  ui->tabWidget->resize (newsize.width () - 2, newsize.height () - pad);

  max = ui->tabWidget->count ();
  if (max > 0)
    for (qint32 counter = 0; counter < max; counter ++)
      ui->tabWidget->widget(counter)->resize (ui->tabWidget->width () - 2,
                                              ui->tabWidget->height () - 20);

  if (tickerVisible)
    ticker->setGeometry (0, height () - (TICKER_HEIGHT + 2),
                         width (), TICKER_HEIGHT);
}

// focus
void
MainWindow::focusInEvent (QFocusEvent * event)
{
  ui->tabWidget->setFocus (event->reason ());
}
