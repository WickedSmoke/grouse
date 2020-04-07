#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//============================================================================
//
// MainWindow
//
//============================================================================


#include <QMainWindow>
#include "defs.h"


class DataManager;
class OptionsDialog;
class StockTicker;
class QTabWidget;
class QDockWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow();

public slots:

    void showAbout();

protected:

    virtual void closeEvent( QCloseEvent* );

private slots:

    void addStudy();
    void addMarker( QAction* );
    void showDataManager();
    void showOptions();
    void showChart( const TableDataVector& );
    void toggleTicker(bool);
    void closeTab(int);
    void tickerSpeed(int);

private:
    CG_ERR_RESULT addChart( const TableDataVector& );
    QStringList getTabKeys( const QString& type );
    void createMenus();
    void addStudyItems();

    QMenu* _studies;
    QMenu* _markers;

    DataManager* _dataManager;
    OptionsDialog* _optionsDialog;
    QTabWidget*  _tabWidget;
    QDockWidget* _tdock;
    StockTicker* _ticker;

    // Disabled copy constructor and operator=
    MainWindow( const MainWindow & ) : QMainWindow( 0 ) {}
    MainWindow &operator=( const MainWindow & ) { return *this; }

    friend class DataManager;   // To access _tabWidget.
};


#endif  //MAINWINDOW_H
