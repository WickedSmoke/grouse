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

    CG_ERR_RESULT addChart( const TableDataVector& );
    bool expandedChart() const;
    void setExpandChart( bool );

    QStringList getTabKeys( const QString& type );

public slots:

    void showAbout();

protected:

    virtual void closeEvent( QCloseEvent* );

private slots:

    void addStudy();
    void addMarker( QAction* );
    void showDataManager();
    void showOptions();
    void toggleTicker(bool);
    void closeTab(int);
    void tickerSpeed(int);

private:

    void createActions();
    void createMenus();
    void createTools();
    void addStudyItems();

    QAction* _actOpen;
    QAction* _actSave;
    QAction* _actQuit;
    QAction* _actAbout;
    QAction* _actManageData;

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
