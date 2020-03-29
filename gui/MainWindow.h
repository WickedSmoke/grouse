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
class QTabWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow();

    CG_ERR_RESULT addChart( const TableDataVector& );
    bool expandedChart() const;
    void setExpandChart( bool );

    QStringList getTabKeys( const QString& type );

    void enableTickerButton();
    void disableTickerButton();

public slots:

    void open( const QString& file );
    void showAbout();

protected:

    virtual void closeEvent( QCloseEvent* );
    virtual void keyPressEvent( QKeyEvent* );
    virtual void mousePressEvent( QMouseEvent* );

    /*
    virtual void mouseReleaseEvent( QMouseEvent* );
    virtual void mouseMoveEvent( QMouseEvent* );
    virtual void focusInEvent( QFocusEvent * );
    virtual void focusOutEvent( QFocusEvent * );
    */

private slots:

    void addStudy();
    void showDataManager();
    void closeTab(int);

private:

    void createActions();
    void createMenus();
    void createTools();

    QAction* _actOpen;
    QAction* _actSave;
    QAction* _actQuit;
    QAction* _actAbout;
    QAction* _actManageData;

    QMenu* _studies;

    DataManager* _dataManager;
    QTabWidget*  _tabWidget;

    // Disabled copy constructor and operator=
    MainWindow( const MainWindow & ) : QMainWindow( 0 ) {}
    MainWindow &operator=( const MainWindow & ) { return *this; }

    friend class DataManager;   // To access _tabWidget.
};


#endif  //MAINWINDOW_H
