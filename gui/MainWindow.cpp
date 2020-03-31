//============================================================================
//
// MainWindow
//
//============================================================================


#include <QApplication>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QSettings>
#include <QTabWidget>
#include "MainWindow.h"
#include "chartapp.h"
#include "common.h"
#include "progressdialog.h"
#include "DataManager.h"
#include "qtachart.h"
#include "qtachart_object.h"


#include "MainWindow_studies.cpp"


#define CREATE_DIALOG(ptr,T) \
  if(! ptr) { \
    ptr = new T(this); \
    if(! ptr) return; \
  }


MainWindow::MainWindow() :
    _dataManager(nullptr)
{
    createActions();
    createMenus();
    createTools();

    _tabWidget = new QTabWidget;
    _tabWidget->setTabsClosable(true);
    setCentralWidget( _tabWidget );
    connect( _tabWidget, SIGNAL(tabCloseRequested(int)),
             this, SLOT(closeTab(int)));

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


bool MainWindow::expandedChart() const
{
    return false; // TODO expandedChartFlag;
}


void MainWindow::setExpandChart(bool)
{
    // TODO
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
/*
        else if( wid->objectName() == type &&
                 type == QLatin1String("Portfolio") )
        {
            Portfolio* portfolio = qobject_cast <Portfolio *> (wid);
            keys += QString::number(portfolio->id());
        }
*/
        else
            keys += QStringLiteral("");
    }
    return keys;
}


void MainWindow::enableTickerButton() {}


void MainWindow::disableTickerButton() {}


void MainWindow::showAbout()
{
    QMessageBox::information( this, "About Scat",
        "Scat\n\nVersion 0.0.1" );
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


void MainWindow::keyPressEvent( QKeyEvent* e )
{
    switch( e->key() )
    {
        case Qt::Key_Q:
        case Qt::Key_Escape:
            close();
            break;

        default:
            e->ignore();
            break;
    }
}


void MainWindow::mousePressEvent( QMouseEvent* )
{
}


void MainWindow::createActions()
{
#define CONNECT(act,slot)   connect(act,SIGNAL(triggered()),this,slot)

/*
    _actOpen = new QAction( "&Open...", this );
    CONNECT( _actOpen, SLOT(open()) );

    _actSave = new QAction( "&Save", this );
    CONNECT( _actSave, SLOT(save()) );
*/
    _actQuit = new QAction( "&Quit", this );
    _actQuit->setShortcut( QKeySequence::Quit );
    CONNECT( _actQuit, SLOT(close()) );

    _actAbout = new QAction( "&About", this );
    CONNECT( _actAbout, SLOT(showAbout()) );

    _actManageData = new QAction( "&Manage Data...", this );
    _actManageData->setShortcut( QKeySequence("F1") );
    CONNECT( _actManageData, SLOT(showDataManager()) );
}


void MainWindow::createMenus()
{
    QMenuBar* bar = menuBar();

    QMenu* file = bar->addMenu( "&File" );
    //file->addAction( _actOpen );
    //file->addAction( _actSave );
    file->addAction( _actManageData );
    file->addSeparator();
    file->addAction( _actQuit );

    _studies = bar->addMenu( "&Studies" );
    _studies->setEnabled( false );
    addStudyItems();

    bar->addSeparator();

    QMenu* help = bar->addMenu( "&Help" );
    help->addAction( _actAbout );
}


void MainWindow::createTools()
{
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


#if 0
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
    CREATE_DIALOG( _dataManager, DataManager );
    _dataManager->show();
}


void MainWindow::closeTab(int index)
{
    if(_tabWidget->count() == 1 )
    {
        setExpandChart(false);
        _studies->setEnabled(false);
    }

    QWidget* wid = _tabWidget->widget(index);
    _tabWidget->removeTab(index);
    delete wid;
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

const char DEFAULT_FONT_FAMILY[] = "Tahoma";
#ifdef Q_OS_MAC
const int  FONT_POINTSIZE_PAD = 3;
const int  FONT_PIXELSIZE_PAD = 3;
#else
const int  FONT_POINTSIZE_PAD = 1;
const int  FONT_PIXELSIZE_PAD = 1;
#endif
const int  CHART_FONT_SIZE_PAD = 3;


int main( int argc, char **argv )
{
    ChartApp app( argc, argv );
    MainWindow w;

    CGScriptFunctionRegistrySize = cgscript_init();

    if( ! app.openDatabase() )
        return 1;
    loadAppOptions(Application_Options);

    progressdialog  = new ProgressDialog(&w);
    templatemanager = new TemplateManagerDialog(&w);
    debugdialog     = new DebugDialog(&w);

    w.show();

    //if( argc > 1 )
    //    w.open( argv[1] );

    return app.exec();
}


//EOF
