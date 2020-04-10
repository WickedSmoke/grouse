//============================================================================
//
// MainWindow
//
//============================================================================


#include <QApplication>
#include <QDockWidget>
#include <QFileDialog>
#include <QKeyEvent>
#include <QLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QSettings>
#include <QTabWidget>
#include <QTreeWidget>
#include "MainWindow.h"
#include "chartapp.h"
#include "common.h"
#include "DataManager.h"
#include "OptionsDialog.h"
#include "portfolio.h"
#include "PortfolioManager.h"
#include "qtachart.h"
#include "qtachart_object.h"
#include "StockTicker.h"


#include "MainWindow_studies.cpp"


#define CREATE_DIALOG(ptr,T) \
  if(! ptr) { \
    ptr = new T(this); \
    if(! ptr) return; \
  }


class SymbolBrowser : public QTreeWidget
{
public:
    SymbolBrowser(QWidget* parent = 0) : QTreeWidget(parent)
    {
        setColumnCount( 1 );
        setMaximumWidth( 100 );
        setHeaderHidden(true);
        setRootIsDecorated(false);
    }

    void reload( /*bool adjusted*/ )
    {
        QString filter;
        SymbolSummary summary;
        int i, n;

        if( gDatabase->loadSymbolSummary( &summary, filter ) == CG_ERR_OK )
        {
            clear();

            n = summary.symbolList.size();
            for( i = 0; i < n; ++i )
            {
                if( summary.adjustedList[i] == QStringLiteral("YES") )
                {
                    QTreeWidgetItem* item = new QTreeWidgetItem(this);
                    item->setText(0, summary.symbolList[i] );
                    item->setText(1, summary.baseList[i] );
                    addTopLevelItem( item );
                }
            }

            setColumnWidth( 0, 90 );
        }
    }
};


MainWindow::MainWindow() :
    _dataManager(nullptr), _portfolioManager(nullptr), _optionsDialog(nullptr),
    _tdock(nullptr), _ticker(nullptr)
{
    setWindowTitle("Chart Grouse");

    createMenus();


    _tabWidget = new QTabWidget;
    _tabWidget->setTabsClosable(true);
    setCentralWidget( _tabWidget );
    connect( _tabWidget, SIGNAL(tabCloseRequested(int)),
             this, SLOT(closeTab(int)));


    _symBrowser = new SymbolBrowser(this);
    connect( _symBrowser, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
             SLOT(browserSelect(QTreeWidgetItem*, int)) );

    _bdock = new QDockWidget("Browser", this);
    _bdock->setFeatures( QDockWidget::NoDockWidgetFeatures );
    _bdock->setAllowedAreas( Qt::LeftDockWidgetArea );
    // Using an empty widget to hide the titlebar.
    _bdock->setTitleBarWidget( new QWidget(_bdock) );
    _bdock->setWidget( _symBrowser );

    addDockWidget( Qt::LeftDockWidgetArea, _bdock );


    setMinimumSize(700, 440);

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

    _symBrowser->reload();
}


void MainWindow::browserSelect( QTreeWidgetItem* item, int )
{
    TableDataVector td;
    int rc = gDatabase->loadTableData( item->text( 1 ), QStringLiteral("YES"),
                                       &td );
    if (rc != CG_ERR_OK)
    {
        showMessage(errorMessage(rc), this);
        return;
    }
    showChart( td );
}


CG_ERR_RESULT MainWindow::addChart( const TableDataVector& datavector )
{
    QTAChart *tachart;
    QTAChartData Data;
    QString SQLCmd, title, subtitle;
    CG_ERR_RESULT result = CG_ERR_OK;

    tachart = new (std::nothrow) QTAChart(_tabWidget);
    if (!tachart)
    {
        result = CG_ERR_NOMEM;
        goto fail;
    }

    if (tachart->getClassError() != CG_ERR_OK)
    {
        result = tachart->getClassError ();
        goto fail_chart;
    }

    foreach(const TableDataClass tdc, datavector)
    {
        tachart->loadFrames(tdc.tablename);
        if (tachart->getClassError() != CG_ERR_OK)
        {
            result = tachart->getClassError ();
            goto fail_chart;
        }
    }

    tachart->setSymbolKey(datavector[0].tablename);
    tachart->setObjectName("Chart");

    // load data
    result = gDatabase->loadChartData(datavector[0].base, &Data);
    if( result != CG_ERR_OK )
    {
        delete tachart;
        showMessage( QStringLiteral("Symbol ") % datavector[0].symbol %
                     ": " % errorMessage(result), this );
        return result;
    }

    tachart->loadData(Data);
    tachart->setSymbol(datavector[0].symbol);
    tachart->setFeed(datavector[0].source);
    title = datavector[0].symbol;
    subtitle = datavector[0].name;

    tachart->setAlwaysRedraw (true);
    tachart->setTitle (title, subtitle);

    {
    QString text = datavector[0].symbol % QStringLiteral(" ") %
                    (datavector[0].adjusted == QStringLiteral("NO") ?
                        QStringLiteral("RAW") : QStringLiteral("ADJ"));

    _tabWidget->addTab( tachart, text );
    _tabWidget->setCurrentIndex( _tabWidget->count() - 1 );
    _studies->setEnabled(true);
    _markers->setEnabled(true);
    tachart->setTabText( text );
    }

#if 0
    // remove tooltip from close tab buttons
    QList<QAbstractButton*> allPButtons;
    allPButtons = _tabWidget->findChildren<QAbstractButton*> ();
    for (int ind = 0; ind < allPButtons.size(); ind++)
        if (allPButtons.at(ind)->inherits("CloseButton"))
            allPButtons.at(ind)->setToolTip(QStringLiteral (""));
#endif

    return result;

fail_chart:
    delete tachart;
fail:
    setGlobalError(result, __FILE__, __LINE__);
    showMessage(errorMessage(result), this);
    return result;
}


QStringList MainWindow::getTabKeys( const QString& type )
{
    static QStringList keys;
    int max = _tabWidget->count();

    keys.clear ();
    if (max == 0)
        return keys;

    for (qint32 counter = 0; counter < max; counter ++)
    {
        QWidget *wid = _tabWidget->widget(counter);
        if( wid->objectName() == type && type == QLatin1String("Chart") )
        {
            QTAChart* chart = qobject_cast <QTAChart *> (wid);
            keys += chart->getSymbolKey();
        }
        else if( wid->objectName() == type &&
                 type == QLatin1String("Portfolio") )
        {
            Portfolio* portfolio = qobject_cast <Portfolio *> (wid);
            keys += QString::number(portfolio->id());
        }
        else
            keys += QStringLiteral("");
    }
    return keys;
}


void MainWindow::showAbout()
{
    QMessageBox* about = new QMessageBox(this);
    about->setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );
    about->setWindowTitle( "About Chart Grouse" );
    about->setIconPixmap( QPixmap(QStringLiteral(":/images/cg-logo.png")) );
    about->setTextFormat( Qt::RichText );
    about->setText( "<h3>Version 0.0.1</h3>\n"
                    "<p>&copy; 2020 Lucas Tsatiris, Karl Robillard</p>" );
    about->exec();
}


void MainWindow::closeEvent( QCloseEvent* event )
{
    QSettings pref(APPDIR, APPNAME);
    QSize saveSize;

    if( ! isMaximized() )
        saveSize = size();
    pref.setValue("main-window-size", saveSize);
    event->accept();
}


static const char* _markerName[6] =
{
    "Label",
    "Trailing Text",
    "Horizontal Line",
    "Vertical Line",
    "Trend Line",
    "Fibonacci",
};


void MainWindow::createMenus()
{
    QAction* act;

    QMenuBar* bar = menuBar();

    QMenu* file = bar->addMenu( "&File" );
    file->addAction( "Manage &Data...", this, SLOT(showDataManager()),
                     QKeySequence("F1") );
    file->addAction( "Manage &Portfolios...", this,SLOT(showPortfolioManager()),
                     QKeySequence("F2") );
    file->addAction( "&Edit Options...", this, SLOT(showOptions()),
                     QKeySequence("CTRL+E") );
    act = file->addAction( "Show &Ticker", this, SLOT(toggleTicker(bool)),
                     QKeySequence("CTRL+T") );
    act->setCheckable(true);
    file->addSeparator();
    file->addAction( "&Quit", this, SLOT(close()), QKeySequence::Quit );

    _studies = bar->addMenu( "&Studies" );
    _studies->setEnabled( false );
    addStudyItems();

    _markers = bar->addMenu( "&Markers" );
    _markers->setEnabled( false );
    connect( _markers, SIGNAL(triggered(QAction*)),
             this, SLOT(addMarker(QAction*)) );
    for( int i = 0; i < 6; ++i )
    {
        act = _markers->addAction( _markerName[i] );
        act->setData( i );
    }

    bar->addSeparator();

    QMenu* help = bar->addMenu( "&Help" );
    help->addAction( "&About", this, SLOT(showAbout()) );
}


CG_ERR_RESULT MainWindow::addPortfolio(int pf_id, QString title,
                                       QString currency, QString feed)
{
    CG_ERR_RESULT result = CG_ERR_OK;
    Portfolio *portfolio;
    QStringList tabkeys;

    tabkeys = getTabKeys("Portfolio");
    if (tabkeys.size() > 0)
    {
        for (qint32 counter = 0; counter < tabkeys.size(); counter ++)
        {
            if (tabkeys[counter].toInt() == pf_id)
            {
                _tabWidget->setCurrentIndex(counter);
                return result;
            }
        }
    }

    portfolio = new (std::nothrow) Portfolio(pf_id, _tabWidget);
    if (!portfolio)
    {
        result = CG_ERR_NOMEM;
        setGlobalError(result, __FILE__, __LINE__);
        showMessage(errorMessage(result), this);
        return result;
    }

    portfolio->setObjectName("Portfolio");
    portfolio->setTitle(title);
    portfolio->setFeed(feed);
    portfolio->setCurrency(currency);
    connect(portfolio, SIGNAL(showChart(const TableDataVector&)),
    SLOT(showChart(const TableDataVector&)) );

    _tabWidget->addTab(portfolio, title);
    _tabWidget->setCurrentIndex(_tabWidget->count() - 1);

    return result;
}


void MainWindow::addStudy()
{
    QAction* act = qobject_cast<QAction*>( sender() );
    if( act )
    {
        QTAChart* chart = qobject_cast<QTAChart*>(_tabWidget->currentWidget());
        if( ! chart )
            return;

        switch( act->data().toInt() )
        {
#include "MainWindow_addStudy.cpp"
        }
    }
}


void MainWindow::addMarker( QAction* act )
{
    QTAChart* chart = qobject_cast<QTAChart*>(_tabWidget->currentWidget());
    if( ! chart )
        return;

    switch( act->data().toInt() )
    {
        case 0:
            chart->addMarkerLabel();
            break;
        case 1:
            chart->addMarkerTrailingText();
            break;
        case 2:
            chart->addMarkerHLine();
            break;
        case 3:
            chart->addMarkerVLine();
            break;
        case 4:
            chart->addMarkerTrendLine();
            break;
        case 5:
            chart->addMarkerFibonacci();
            break;
    }
}


#if 0
void MainWindow::open( const QString& file )
{
    if( 1 )
    {
        setWindowTitle( file );
    }
    else
    {
        QString error( "Error opening file " );
        QMessageBox::warning( this, "Load", error + file );
    }
}


void MainWindow::open()
{
    QString fn;
    QString path( "" /*lastSampleFileName*/ );

    //if( ! path.isNull() )
    //    pathTruncate( path );

    fn = QFileDialog::getOpenFileName( this, "Open File", path );
    if( ! fn.isEmpty() )
        open( fn );
}
#endif


void MainWindow::showDataManager()
{
    if(! _dataManager)
    {
        _dataManager = new DataManager(this);
        if(! _dataManager) return;
        connect( _dataManager, SIGNAL(showChart(const TableDataVector&)),
                 SLOT(showChart(const TableDataVector&)) );
    }
    _dataManager->show();
}


void MainWindow::showPortfolioManager()
{
    CREATE_DIALOG( _portfolioManager, PortfolioManager )
    _portfolioManager->show();
}


void MainWindow::showOptions()
{
    if(! _optionsDialog)
    {
        _optionsDialog = new OptionsDialog(this);
        if(! _optionsDialog) return;
        connect( _optionsDialog, SIGNAL(tickerSpeedChanged(int)),
                 SLOT(tickerSpeed(int)) );
    }
    _optionsDialog->show();
}


void MainWindow::showChart( const TableDataVector& tv )
{
    QStringList symkeys = getTabKeys("Chart");
    if (symkeys.size() != 0)
    {
      for (int i = 0; i < symkeys.size(); ++i)
      {
        if (tv[0].tablename == symkeys[i])
        {
          _tabWidget->setCurrentIndex(i);
          return;
        }
      }
    }

    addChart(tv);
}


void MainWindow::toggleTicker( bool on )
{
    if( on )
    {
        if(! _ticker)
        {
            _ticker = new StockTicker(this);
            if(! _ticker) return;

            // TODO: Set height based upon font used.
            // Does not appear in QDockWidget without some minimum height.
            _ticker->setFixedHeight( 33 );

            if(_tdock)
            {
                _tdock->setWidget( _ticker );
                _tdock->show();
            }
            else
            {
                _tdock = new QDockWidget("Ticker", this);
                _tdock->setFeatures( QDockWidget::NoDockWidgetFeatures );
                _tdock->setAllowedAreas( Qt::BottomDockWidgetArea );
                // Using an empty widget to hide the titlebar.
                _tdock->setTitleBarWidget( new QWidget(_tdock) );
                _tdock->setWidget( _ticker );

                addDockWidget( Qt::BottomDockWidgetArea, _tdock );
            }
        }

        QStringList lsymbol, lfeed;
        int rc = gDatabase->loadTickerSymbols(lsymbol, lfeed);
        if(rc != CG_ERR_OK)
        {
            showMessage(errorMessage(rc), this);
            return;
        }
        if(lsymbol.isEmpty())
        {
            showMessage("No symbols found in ticker", this);
            return;
        }
        _ticker->show();
    }
    else if( _ticker )
    {
#if 0
        _ticker->hide();
#else
        _tdock->hide();
        delete _ticker;
        _ticker = nullptr;
#endif
    }
}


void MainWindow::closeTab(int index)
{
    if(_tabWidget->count() == 1 )
    {
        _studies->setEnabled(false);
        _markers->setEnabled(false);
    }

    QWidget* wid = _tabWidget->widget(index);
    _tabWidget->removeTab(index);
    delete wid;
}


void MainWindow::tickerSpeed(int speed)
{
    if( _ticker )
        _ticker->setSpeed(speed);
}


//----------------------------------------------------------------------------


#include "cgscript.h"
#include "progressdialog.h"
#include "templatemanagerdialog.h"
#include "debugdialog.h"


AppOptions _options;
AppOptions *Application_Options = &_options;

QProgressBar *GlobalProgressBar = nullptr;
QString Year, Month, Day;

TemplateManagerDialog *templatemanager;
ProgressDialog *progressdialog;
DebugDialog *debugdialog;

size_t CGScriptFunctionRegistrySize;


int main( int argc, char **argv )
{
    ChartApp app( argc, argv );
    int rc = 1;

    CGScriptFunctionRegistrySize = cgscript_init();

    if( ! app.openDatabase() )
        return 1;
    loadAppOptions(Application_Options);

    MainWindow* win = new MainWindow;
    if( win )
    {
        progressdialog  = new ProgressDialog(win);
        templatemanager = new TemplateManagerDialog(win);
        debugdialog     = new DebugDialog(win);

        //win->show();
        //if( argc > 1 )
        //    win->open( argv[1] );

        rc = app.exec();
        delete win;
    }
    return rc;
}


//EOF
