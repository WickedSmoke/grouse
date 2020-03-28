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
#include "progressdialog.h"
#include "DataManager.h"


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
    setCentralWidget( _tabWidget );

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


CG_ERR_RESULT MainWindow::addChart(TableDataVector& /*datavector*/)
{
    return CG_ERR_OK;   // TODO
}


bool MainWindow::expandedChart() const
{
    return false; // TODO expandedChartFlag;
}


void MainWindow::setExpandChart(bool)
{
    // TODO
}


QStringList MainWindow::getTabKeys(QString /*type*/)
{
    return QStringList();   // TODO
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

    _actOpen = new QAction( "&Open...", this );
    CONNECT( _actOpen, SLOT(open()) );

    _actSave = new QAction( "&Save", this );
    CONNECT( _actSave, SLOT(save()) );

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
    file->addAction( _actOpen );
    file->addAction( _actSave );
    file->addSeparator();
    file->addAction( _actQuit );

    QMenu* tools = bar->addMenu( "&Tool" );
    tools->addAction( _actManageData );

    bar->addSeparator();

    QMenu* help = bar->addMenu( "&Help" );
    help->addAction( _actAbout );
}


void MainWindow::createTools()
{
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


void MainWindow::save()
{
}


void MainWindow::saveAs()
{
}


void MainWindow::showDataManager()
{
    CREATE_DIALOG( _dataManager, DataManager );
    _dataManager->show();
}


//----------------------------------------------------------------------------


#include "cgscript.h"
#include "downloaddatadialog.h"
#include "progressdialog.h"
#include "templatemanagerdialog.h"
#include "debugdialog.h"


AppOptions _options;
AppOptions *Application_Options = &_options;

SQLists _comboitems;
SQLists *ComboItems = &_comboitems;

QProgressBar *GlobalProgressBar = nullptr;
QMutex *ResourceMutex = nullptr;
QString Year, Month, Day;

DownloadDataDialog *downloaddatadialog;
TemplateManagerDialog *templatemanager;
ProgressDialog *progressdialog;
DebugDialog *debugdialog;

size_t CGScriptFunctionRegistrySize;
int NCORES;

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
    InstrumentDatabase idb;
    MainWindow w;

    CGScriptFunctionRegistrySize = cgscript_init();

    {
    const char* openError;
    QString fn = QDir::homePath() % QDir::separator() %
        QStringLiteral(".config") % QDir::separator() % APPDIR %
        QDir::separator() % DBNAME;
    if( ! idb.openFile( fn, &openError ) )
    {
        showMessage( openError );
        return 1;
    }
    }

    downloaddatadialog = new DownloadDataDialog(&w);
    progressdialog  = new ProgressDialog(&w);
    templatemanager = new TemplateManagerDialog(&w);
    debugdialog     = new DebugDialog(&w);

    w.show();

    if( argc > 1 )
        w.open( argv[1] );

    return app.exec();
}


//EOF
