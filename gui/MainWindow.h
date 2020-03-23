#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//============================================================================
//
// MainWindow
//
//============================================================================


#include <QMainWindow>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow();

public slots:

    void open( const QString& file );
    void showAbout();

protected:

    virtual void keyPressEvent( QKeyEvent* );
    virtual void mousePressEvent( QMouseEvent* );

    /*
    virtual void mouseReleaseEvent( QMouseEvent* );
    virtual void mouseMoveEvent( QMouseEvent* );
    virtual void focusInEvent( QFocusEvent * );
    virtual void focusOutEvent( QFocusEvent * );
    */

private slots:

    void open();
    void save();
    void saveAs();

private:

    void createActions();
    void createMenus();
    void createTools();

    QAction* _actOpen;
    QAction* _actSave;
    QAction* _actQuit;
    QAction* _actAbout;

    // Disabled copy constructor and operator=
    MainWindow( const MainWindow & ) : QMainWindow( 0 ) {}
    MainWindow &operator=( const MainWindow & ) { return *this; }
};


#endif  //MAINWINDOW_H
