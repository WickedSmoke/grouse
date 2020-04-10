#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//============================================================================
//
// MainWindow
//
//============================================================================


#include <QMainWindow>
#include "defs.h"


class QTreeWidget;
class QCheckBox;

class SymbolBrowser : public QWidget
{
    Q_OBJECT

public:
    enum Columns
    {
        COL_Symbol,
        COL_Base
    };

    SymbolBrowser(QWidget* parent = nullptr);
    QString adjustedText() const;

public slots:
    void reload();

public:
    QTreeWidget* tree;
    QCheckBox* adjusted;
};


class DataManager;
class PortfolioManager;
class OptionsDialog;
class ChartPropertiesDialog;
class StockTicker;
class QTabWidget;
class QDockWidget;
class QTreeWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow();

    CG_ERR_RESULT addPortfolio(int pf_id, QString title,
                               QString currency, QString feed);

public slots:

    void showAbout();

protected:

    virtual void closeEvent( QCloseEvent* );

private slots:

    void browserSelect( QTreeWidgetItem* , int );
    void addStudy();
    void addMarker( QAction* );
    void showDataManager();
    void showPortfolioManager();
    void showOptions();
    void editChartProp();
    void showChart( const TableDataVector& );
    void toggleTicker(bool);
    void closeTab(int);
    void tickerSpeed(int);

private:
    void syncPref( bool load ) const;
    CG_ERR_RESULT addChart( const TableDataVector& );
    QStringList getTabKeys( const QString& type );
    void createMenus();
    void addStudyItems();

    QMenu* _editMenu;
    QMenu* _studies;
    QMenu* _markers;

    SymbolBrowser* _symBrowser;
    DataManager* _dataManager;
    PortfolioManager* _portfolioManager;
    OptionsDialog* _optionsDialog;
    ChartPropertiesDialog* _chartProp;
    QTabWidget*  _tabWidget;
    QDockWidget* _bdock;
    QDockWidget* _tdock;
    StockTicker* _ticker;

    // Disabled copy constructor and operator=
    MainWindow( const MainWindow & ) : QMainWindow( 0 ) {}
    MainWindow &operator=( const MainWindow & ) { return *this; }

    friend class DataManager;   // To access _tabWidget.
};


#endif  //MAINWINDOW_H
