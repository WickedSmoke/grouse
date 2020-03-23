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
#include "MainWindow.h"


MainWindow::MainWindow()
{
    createActions();
    createMenus();
    createTools();
}


void MainWindow::showAbout()
{
    QMessageBox::information( this, "About MainWindow",
        "Product\n\nVersion 0.0.1" );
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
    _actOpen = new QAction( "&Open...", this );
    connect( _actOpen, SIGNAL(triggered()), this, SLOT(open()));

    _actSave = new QAction( "&Save", this );
    connect( _actSave, SIGNAL(triggered()), this, SLOT(save()));

    _actQuit = new QAction( "&Quit", this );
    connect( _actQuit, SIGNAL(triggered()), this, SLOT(close()));

    _actAbout = new QAction( "&About", this );
    connect( _actAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
}


void MainWindow::createMenus()
{
    QMenuBar* bar = menuBar();

    QMenu* file = bar->addMenu( "&File" );
    file->addAction( _actOpen );
    file->addAction( _actSave );
    file->addSeparator();
    file->addAction( _actQuit );

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


//----------------------------------------------------------------------------


int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    MainWindow w;
    w.show();

    if( argc > 1 )
        w.open( argv[1] );

    return app.exec();
}


//EOF
