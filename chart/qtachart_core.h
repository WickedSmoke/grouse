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

#ifndef QTACHART_CORE_H
#define QTACHART_CORE_H

#include <QMutex>
#include <QFrame>
#include <QColor>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QGraphicsView>
#include "qtcgraphicsscene.h"
#include "qtachart.h"
#include "qtachart_properties.h"
#include "qtachart_eventfilters.h"
#include "qtachart_help.h"
#include "qtachart_data.h"
#include "qtachart_draw.h"
#include "qtachart_functions.h"
#include "qtachart_objects.h"
#include "qtachart_object.h"
#include "textobjectdialog.h"
#include "lineobjectdialog.h"
#include "priceupdater.h"

#define STRMAXLEN      256
#define SHADOW_WIDTH     1
#define MAXITEMS      8192
#define MAXGRIDLINES   512

// time frame structure
typedef struct
{
  QString TFSymbol;     // eg D, W, M, Y
  QString TFName;       // eg DAY, WEEK, MONTH, YEAR
  QString TFCurrency;   // currency
  QString TFMarket;     // market
  QString TFTablename;  // tablename
  FrameVector HLOC; // framevector of time frame
  FrameVector HEIKINASHI; // heikin-ashi framevector of time frame
  qreal TFExcess_Drag_Width; // excess drag width
  int TFStartBar;   // start bar
} TFClass;
Q_DECLARE_TYPEINFO (TFClass, Q_MOVABLE_TYPE);

typedef QVector < QGraphicsTextItem * >TextVector;
typedef QVector < QGraphicsLineItem * >LineVector;
typedef QVector < SubChartButton *> CloseSCBVector;
typedef QVector < QWidget *> gcQWidgetVector;  //  garbage collector
typedef QVector < TFClass > TimeFrameVector;

enum _QTAChartCandleTrend
{
  QTACHART_CANDLE_UP,
  QTACHART_CANDLE_DOWN
};
typedef enum _QTAChartCandleTrend QTAChartCandleTrend;

typedef struct
{
  // high, low, open, close, date and time
  const QTAChartFrame *HLOC;
  // coordinates of the candle/bar body
  qreal x;
  qreal y;
  // in dots
  qreal width;
  qreal height;
  qreal uporopen; // up shadow for candle or open for bar
  qreal downorclose; // down shadow for candle or close for bar
  // candle trend (unused for bars)
  QTAChartCandleTrend trend;
  // candle's frame color (unused for bars)
  QColor fcolor;
} QTAChartBarData;      // Bar's/Candle's geometry data;
Q_DECLARE_TYPEINFO (QTAChartBarData, Q_MOVABLE_TYPE);

// QTACGraphicsTextItem
class QTACGraphicsTextItem : public QGraphicsTextItem
{
  Q_OBJECT

public:
  QTACGraphicsTextItem () : QGraphicsTextItem()
  {
    background = Qt::black;
  }

  QColor background;        // background color

  inline void setDefaultBackgroundColor (const QColor & color) NOEXCEPT
  {
    background = color;
  }

  void paint( QPainter *painter, const QStyleOptionGraphicsItem *o, QWidget *w)
  {
    painter->setBrush(background);
    painter->drawRect(boundingRect());
    QGraphicsTextItem::paint(painter, o, w);
  }
};


class QTAChartCore : public QObject
{
  Q_OBJECT
public:
  // functions
  explicit QTAChartCore (QWidget * parent = 0);     // constructor
  ~QTAChartCore (void); // destructor
  friend class QTACObject;

  void addHLine (QGraphicsLineItem *item, qreal price); // add horizontal line
  void addVLine (QGraphicsLineItem *item); // add vertical line
  void addVLine (QGraphicsLineItem *item, QString trailerCandleText); // add vertical line
  void addTLine (QGraphicsLineItem *item); // add trend line
  void addTLine (QGraphicsLineItem *item, LineEdge e1, LineEdge e2); // add trend line
  QTACObject *addLabel (QGraphicsTextItem *item, qreal x, qreal y); // add label
  void addFibo (QGraphicsLineItem *item); // add fibo
  void addFibo (QGraphicsLineItem *item, LineEdge e1, LineEdge e2);  // add fibo
  SubChartButton *addSCB (QTACObject *obj, QString name); // add subchart button
  void addText(QGraphicsTextItem *item, qreal x, qreal y); // add trailing text
  void addText(QGraphicsTextItem *item, QString candleText, qreal price);
  void addOnlinePrice (void); // add online price

  int barOnX (int x) const; // return the bar number that corresponds to x or -1 if not applicable

  void changeForeColor (QColor color); // change the foreground color
  void chartBackward (int nbars);   // move the chart backward
  void chartBegin (void);   // move to chart's begin
  void chartEnd (void);     // move to chart's end
  void chartForward (int nbars);    // move the chart forward by nbars
  void chartPageNext (void);    // move to previous page
  void chartPagePrevious (void);    // move to previous page
  void clearGRID (void) NOEXCEPT; // clear all the grid lines on the chart
  void clearITEMS (void) NOEXCEPT;  // clear all the rect objects on the chart
  void createTFButtons (void); // create timeframe buttons

  void deleteGRID (void);   // delete the chart's grid
  void deleteVolumes (void);    // delete volumes
  void deleteObject (QTACObject *); // delete an object
  void deleteOnlinePrice (void); // delete online price
  void draw (void);     // draws the chart
  void drawBar (QTAChartBarData * bardata, qint32 &counter); // draw a bar on chart
  void drawBarChart (void); // draw a bar chart
  void drawPriceLine (QColor forecolor, qint32 thickness); //draw price line
  void drawCandle (QTAChartBarData * data, qint32 &counter);    // draw a candle on the chart
  void drawCandleChart (void);  // draw a chart of candles
  void drawGRID (void);     // draw the grid
  void drawVolumes (void);  // draw volumes
  void deleteObject_clicked (QTACObject *); // delete an object
  void deleteITEMS (void);  // delete graphics items on screen

  inline qint32  getCurrentBar (void) const NOEXCEPT
  {
    return currentbar;  // get the bar under mouse pointer
  }
  QString getBottomText (int x); // form the bottom text
  QString getBottomTextHA (int x); // form the bottom text for HA
  QString getTabText (void) const; // get the parent's tab label

  void hideAllItems (void); // hide all the objects on a chart

  void loadSettings (void); // load chart settings

  void manageObjects (void); // manage objects

  GNUFASTCALL qreal MSVCFASTCALL priceOnY (int y) GNUHOT; // returns the price that corresponds to the y argument

  void restoreBottomText (void);    // restore the bottom text

  void saveSettings (void) const; // save chart settings
  void setBottomText (int x);   // set the bottom text
  void setCustomBottomText (QString string); // set the bottom text to custom string
  void setChartStyle (int chartstyle);  // set the chart's style
#ifdef CHART_SCREENS
  void setChartProperties (void);   // set the chart's properties
  void selectDrawObject (void); // select object to draw
  void selectFunction (void); // select function to apply
  void showHelp (void); // show help
  void showData (void); // show statistics
#endif
  void setLinearScale (bool scale); // set the chart's price scale
  bool setPdlgOpened (); // sets param dialog to open, returns true on success
  void setPdlgClosed (); //sets param dialog to closed
  void setRullerCursor (int y); // set the price cursor
  void setTitle (QString title, QString subtitle); // set chart's title
  inline void setSizeChanged () NOEXCEPT
  {
    size_changed = true;
  }; // set size_changed
  void showAllItems (void); // show all the objects on the chart
  inline bool visibleLastBar () const NOEXCEPT // report true if last bar is visible
  {
    return (bool) ((int) *excess_drag_width);
  }

  GNUFASTCALL qreal MSVCFASTCALL yOnPrice (qreal price) const GNUHOT; // returns the y that corresponds to the price argument

  // variables & classes
  QMutex *DrawMutex; // mutex to protect chart from concurent updates
  QTCGraphicsScene Scene; // static scene
  QGraphicsView View; // static view
  QTAChartEventFilter *chartEventFilter;    // event filter for chart
  QTAChartSceneEventFilter *sceneEventFilter;   // event filter for chart
  QColor forecolor;     // foreground color
  QColor backcolor;     // background color
  QColor framecolor;        // bar's frame color
  QColor textcolor;     // text color
  QColor cursorcolor;       // price cursor color
  QColor gridcolor;     // grid line color
  QColor linecolor;     // line chart color
  QColor barcolor;      // bar chart color
  QPixmap proppixmap;   // pixmap for properties button
  QPixmap helppixmap;   // pixmap for help button
  QPixmap zoominpixmap; // pixmap for zoom in button
  QPixmap zoomoutpixmap;    // pixmap for zoom out button
  QIcon propicon; // icon for properties button
  QIcon expandicon; // icon for properties button
  QIcon functionicon; // icon for function button
  QIcon drawicon; // icon for draw button
  QIcon objectsicon; // icon for objects button
  QIcon helpicon; // icon for help button
  QIcon dataicon; // icon for data button
  QIcon zoominicon; // icon for zoom in button
  QIcon zoomouticon; // icon for zoom out button
  QFont gridFont;   // chart's grid font
  QString Symbol;  // chart's symbol
  QString Feed;  // symbol's feed
  QString SymbolKey; // symbol's key on chart settings
  QString currenttf; // current time frame eg DAY, WEEK etc
  QStringList monthlit; // literals for month (JAN, FEB etc)
  QGraphicsView *chart;     // chart
  QTCGraphicsScene *scene;  // ptr on Scene
  QTCGraphicsItem **GRID;       // grid lines
  QTCGraphicsItem **ITEMS;      // graphics items on screen
  QTACGraphicsTextItem *ruller_cursor;  // chart's price cursor
  QGraphicsTextItem *textitem;  // item to be used in label or text object (defined by dragged_obj_type)
  QGraphicsTextItem *title; // chart's title
  QGraphicsTextItem *subtitle;  // chart's subtitle
  QGraphicsTextItem *scaletitle;    // chart's price scale title (linear or logarithmic)
  QGraphicsTextItem *typetitle; // chart's type title (Bar, Line, Candle, Heikin-Ashi)
  QGraphicsTextItem *bottom_text;   // chart's bottom text
  QGraphicsLineItem *topedge;   // line indicating chart's top edge
  QGraphicsLineItem *bottomedge;    // line indicating chart's bottom edge
  QGraphicsLineItem *leftedge;  // line indicating chart's left edge
  QGraphicsLineItem *rightedge; // line indicating chart's right edge
  QGraphicsLineItem *hvline;    // line item to be used in hline or vline
  QToolButton **tfBtn;      // chart's time frame buttons
  QGraphicsProxyWidget **prxtfBtn; // time frame button proxy

#ifdef CHART_SCREENS
  QToolButton *propertiesBtn;   // chart's properties button
  QToolButton *helpBtn; // chart's help button
  QToolButton *dataBtn; // chart's help button
  QToolButton *zoomInBtn;   // chart's propetries' button
  QToolButton *zoomOutBtn;  // chart's propetries' button
  QToolButton *expandBtn;   // chart's expansion/shrink (and back) button
  QToolButton *functionBtn; // chart's function button
  QToolButton *drawBtn;     // chart's draw button
  QToolButton *objectsBtn;      // chart's objects button
  QTACProperties *propScr;  // chart's properties screen
  QTACDraw *drawScr;    // chart's draw screen
  QTACFunctions *functionScr;   // chart's function screen
  QTACObjects *objectsScr;  // chart's objects screen
  QTACHelp *helpScr;    // help screen
  QTACData *dataScr;    // fundamental data screen
  QGraphicsProxyWidget *prxpropBtn; // properties' button proxy
  QGraphicsProxyWidget *prxhelpBtn; // help button proxy
  QGraphicsProxyWidget *prxdataBtn; // help button proxy
  QGraphicsProxyWidget *prxzoominBtn;   // zoom in button proxy
  QGraphicsProxyWidget *prxzoomoutBtn;  // zoom out button proxy
  QGraphicsProxyWidget *prxexpandBtn;   // chart's expansion/shrink button proxy
  QGraphicsProxyWidget *prxfunctionBtn; // chart's function button proxy
  QGraphicsProxyWidget *prxobjectsBtn;  // chart's objects button proxy
  QGraphicsProxyWidget *prxdrawBtn; // chart's draw button proxy
  QGraphicsProxyWidget *prxpropScr; // properties' screen proxy
  QGraphicsProxyWidget *prxdrawScr; // draw screen proxy
  QGraphicsProxyWidget *prxfunctionScr; // draw screen proxy
  QGraphicsProxyWidget *prxobjectsScr;  // objects screen proxy
  QGraphicsProxyWidget *prxhelpScr; // help screen proxy
  QGraphicsProxyWidget *prxdataScr; // data screen proxy
#endif

  QTACObject *volumes;  // volumes' subchart object
  QTACObject *onlineprice; // online price
  TextObjectDialog *textobjectdialog;       // label/text object's dialog
  LineObjectDialog *lineobjectdialog;       // hline, vline etc line based object's dialog
  gcQWidgetVector garbageQWidget; // garbage collector for QWidgets
  CloseSCBVector CloseSCB;  // subchart buttons vector
  PriceVector *lineprice; // price applied to line
  FrameVector *HLOC;        // chart's frame vector
  FrameVector *HEIKINASHI;  // chart's frame vector (heikinashi values)
  PriceUpdater *priceUpdater; // priceUpdater object;
  qint32 *startbar;         // start bar
  qreal *excess_drag_width; // excessive drag of the chart to the left. default is 0
  TimeFrameVector TIMEFRAME; // chart's vector of time frames;
  PriceVector OPEN;     // open values
  PriceVector CLOSE;    // close values
  PriceVector HIGH;     // high values
  PriceVector LOW;      // low values
  VolumeVector VOLUME;  // volumes
  ObjectVector Object;  // the objects
  QAtomicInt paramdlgopened;        // param dialog opened flag
  qreal xexcess_drag_width; // excess_drag_width setting
  qreal points;         // fractional resolution (eg two digits = 0.01)
  qreal dots_per_point;     // number of dots a point counts on the linear vertical axis
  qreal chartheight;        // height of the chart
  qreal chartwidth;     // width of the chart, minus the right border
  qreal chartframe;     // frame of the chart in dots
  qreal title_height;       // height in dots of the title
  qreal bottomline_height;  // height in dots of the bottom line
  qreal right_border_width; // the width of the vertical text area (price ruler). default is 50
  qreal max_high;       // the maximum high of all candles on screen
  qreal min_low;        // the minimum low of all candles on screen
  qreal initial_mouse_x;    // initial x position of mouse drag
  qreal ruller_cursor_x;        // price cursor's x
  qreal ruller_cursor_y;        // price cursor's y
  qreal last_x;         // mouse pointer's last reported x
  quint64 eventcounter; // counter of events
  qint64 charttop;      // (price / points) the y=0 coordinate refers (in cents or pips)
  qint64 chartbottom;       // (price / points) the y=chartheight coordinate refers (in cents or pips)
  qint64 chartleftmost;         // the leftmost point of the chart
  qint64 chartrightmost;        // the rightmost point of the chart
  qint64 charttopmost;          // the topmost point of the chart
  qint64 chartbottomost;        // the bottommost point of the chart;
  qint32 xstartbar;         // start bar from settings
  qint32 width;         // chart's width
  qint32 height;            // chart's height
  qint32 nbars_on_chart;        // bars on chart
  qint32 framewidth;            // frame's width
  qint32 barwidth;              // bar's width
  qint32 gridstep;          // step for grid lines
  qint32 decimals;          // number of digits after the decimal point
  qint32 chart_style;       // QTACHART_CANDLE, QTACHART_HEIKINASHI, QTACHART_LINE. default candle
  qint32 dragged_obj_type; // QTACHART_OBJ_LABEL, QTACHART_OBJ_TEXT, QTACHART_OBJ_VLINE etc;
  qint32 linethickness; // width of line in line chart
  qint32 nsubcharts;        // number of subcharts
  qint32 visibleitems;      // number of visible items
  qint32 visiblegriditems;  // number of visible grid items
  qint32 ITEMSsize;         // size of ITEMS;
  qint32 GRIDsize;          // size of GRID;
  qint32 fracdig;           // fractional digits on grid
  qint32 classError;        // error code for this class

private:
  qint32 currentbar;        // bar under mouse pointer

public:
  bool show_volumes;    // default true
  bool show_onlineprice; // default false
  bool show_grid;       // default true
  bool show_ruller;     // default true
  bool drag;            // mouse in drag mode. default false
  bool reloaded;        // data reloaded. default false
  bool redraw;          // redraw the chart. default true
  bool recalc;          // recalculate geometry no matter what. default true
  bool always_redraw;       // always redraw. default true
  bool geom_changed;        // geometry changes
  bool events_enabled;      // enables or disables events
  bool object_drag;         // true when mouse pointer drags an object
  bool tfinit;              // true if time frames initialized, false otherwise
  bool linear;              // true if the chart is linear, false if logarithmic
  bool firstshow;           // true until first seen
  bool size_changed;        // true if chart's size/geometry changed
  char titletext[STRMAXLEN];        // title's text
  char subtitletext[STRMAXLEN];     // subtitle's text

private slots:
  void SCBtn_clicked (void); // subchart button slot
  void tfBtn_clicked (void); // time frame button slot
};

// returns the maximum number of decimals (eg 2 decimals = 0.01)
extern qreal maxfractionals (FrameVector *HLOC) NOEXCEPT GNUHOT;

// returns of core data of QTAChart
extern QTAChartCore *getData (QTAChart *) NOEXCEPT;

#endif // QTACHART_CORE_H
