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

#ifndef QTACHART_OBJECT_H
#define QTACHART_OBJECT_H

#include <QLibrary>
#include <QObject>
#include <QPointer>
#include <QGraphicsLineItem>
#include <QIcon>
#include <QToolButton>
#include <QGraphicsProxyWidget>
#include "qtcgraphicsitem.h"
#include "common.h"
#include "qtachart_eventfilters.h"
#include "qtachart_object_sanitizer.h"
#include "function_dataset.h"
#include "dynparamsdialog.h"
#include "chartapp.h"


// object types
enum QTAChartObjectType
{
  QTACHART_OBJ_LABEL,               // label object
  QTACHART_OBJ_TEXT,                // text object
  QTACHART_OBJ_VLINE,               // verical line
  QTACHART_OBJ_HLINE,               // horizontial line
  QTACHART_OBJ_LINE,                // trend line
  QTACHART_OBJ_CHANNEL,             // channel (not used)
  QTACHART_OBJ_FIBO,                // fibonacci
  QTACHART_OBJ_SUBCHART,            // subchart object
  QTACHART_OBJ_VBARS,               // vertical bars
  QTACHART_OBJ_CURVE,               // curve
  QTACHART_OBJ_DOT,                 // dot
  QTACHART_OBJ_CONTAINER            // parent object for other objects
};

// data sets
enum QTAChartDataSet
{
  QTACHART_OPEN,                    // open
  QTACHART_CLOSE,                   // close
  QTACHART_HIGH,                    // high
  QTACHART_LOW,                     // low
  QTACHART_VOLUME,                  // volume
  QTACHART_NULL                     // no dataset needed
};

// text adjustment
enum QTAChartObjectTextAdjustment
{
  QTACHART_OBJ_VADJUST_NORMAL,
  QTACHART_OBJ_VADJUST_CENTER,
  QTACHART_OBJ_VADJUST_ABOVE,
  QTACHART_OBJ_VADJUST_BELOW,
  QTACHART_OBJ_HADJUST_NORMAL,
  QTACHART_OBJ_HADJUST_CENTER,
  QTACHART_OBJ_HADJUST_LEFT,
  QTACHART_OBJ_HADJUST_RIGHT
};

// symbols
#define QTACHART_OBJ_UPWARDS_ARROW                          "&#8593;"
#define QTACHART_OBJ_DOWNWARDS_ARROW                        "&#8595;"
#define QTACHART_OBJ_UPWARDS_WHITE_ARROW                    "&#8679;"
#define QTACHART_OBJ_DOWNWARDS_WHITE_ARROW                  "&#8681;"
#define QTACHART_OBJ_WHITE_UP_POINTING_INDEX                "&#9757;"
#define QTACHART_OBJ_WHITE_DOWN_POINTING_INDEX              "&#9759;"

typedef QVector < qreal >Coordinate;
typedef QVector < QGraphicsItem *>ItemVector;
typedef QList < QTCGraphicsItem *>GrItemVector;
typedef QVector < QGraphicsTextItem *>TextItemVector;
typedef PriceVector VolumeVector;

// forward declaration of QTACObject
class QTACObject;
typedef QVector < QTACObject *> ObjectVector;

// line edge
class LineEdge
{
public:
  LineEdge (void) NOEXCEPT;             // constructor
  ~LineEdge (void);                     // destructor

  QString trailerCandleText; // bottom text of candle a text object is attached on
  QGraphicsTextItem *pricetxt; // the QGraphicsTextItem of price
  qreal price;  // the price
  qreal pad;  // if there is no trailing candle, the number of frames from last bar;
  qint16 txtdirection; // 0 left of edge, 1 right of edge
  qint16 sequence;  // 1 for x1,y1, 2 for x2,y2
};

typedef QVector < LineEdge *>EdgeVector;

// subchart button class
class SubChartButton : public QToolButton
{
  Q_OBJECT
public:
  explicit SubChartButton (QTACObject *) NOEXCEPT; // constructor
  ~SubChartButton (void); // destructor

  QTACObject *getOwner (void) NOEXCEPT; // get owner object

private:
  // variables
  QTAChartCore *chartdata;
  QTACObject *owner;
};
Q_DECLARE_TYPEINFO (SubChartButton, Q_MOVABLE_TYPE);

// object class
class QTACObject : public QObject
{
  Q_OBJECT

public:
// functions
  QTACObject (QTAChartCore*, QTAChartObjectType);       // constructor
  QTACObject (QTACObject *, QTAChartObjectType);        // constructor
  QTACObject (void *, QString, QString);                // constructor
  ~QTACObject (void);                                   // destructor

  void changeForeColor (QColor color); // change the foreground color
  void clearITEMS (void) NOEXCEPT;  // clear all the objects on the chart
  void deleteITEMS (void);  // delete all the objects on the chart
  void drawVBars (void);    // draw vertical bars
  void drawLabel (void);    // draw a label
  void drawText (void);     // draw a label
  void drawHVLine (void);   // draw a horizontal or vertical line
  void drawTLine (void);    // draw a trend line
  void drawFibo (void);     // draw a fibo line
  void drawCurve (void);    // draw a curve
  void drawDot (void);      // draw a dot
  void emitUpdateOnlinePrice (RTPrice rtprice); // update online price signal emittion

  DataSet getModVSet (int *size); // get module's value set
  QTACObject *getParentObject (void) NOEXCEPT;  // return parent object
  QString getTitle (void) NOEXCEPT;         // get object's title
  QString getTrailerCandleText (void); // get text's trailer candle
  QString getTrailerCandleText2 (void); // get text's trailer candle
  QString getText (void) const
  {
    return text->toPlainText ();  // get object's text for label or text objects
  }
  QFont   getFont (void) const
  {
    return text->font ();  // get object's font for label or text objects
  }
  QColor  getColor (void) const; // get object's color for drawing objects
  void    getCoordinates (qreal *x1, qreal *y1,
                          qreal *x2, qreal *y2)
  {
    *x1 = X1;  // get object's received coordinates
    *x2 = X2;
    *y1 = Y1;
    *y2 = Y2;
  }
  qreal   getPrice (void) const; // return the price level
  qreal   getPrice2 (void) const; // return the price level
  qreal   getPad (void); // get the edge's pad
  qreal   getPad2 (void); // get the edge's pad
  qint16  getTxtDirection (void); // get text's direction of label
  qint16  getTxtDirection2 (void); // get text's direction of label
  inline int     getPeriod () const NOEXCEPT
  {
    return period;  // get the period
  }
  qreal paramValue (const QString& name) const;

  inline DynParamsDialog *getParamDialog () const NOEXCEPT
  {
    return paramDialog;  // indicators' parameters dialog
  }
  DataSet valueSet (void);          // get object's value set

  bool modifyIndicator (void); // modify technical indicator
  bool moduleInit (void *data, int *objtype);   // call module's Init() function
  bool moduleLoop (void);       // call module's Loop() function
  bool moduleEvent (int event);     // call module's Event() function
  void removeAllChildren (void); // remove all the children objects
  void removeChild (QTACObject *child); // remove a child object
  void setAttributes (QTAChartDataSet dstype,
                      int period,
                      QString periodParamName,
                      DataSet (*TAfunc) (const DataSet, int),
                      qreal rangemin,
                      qreal rangemax,
                      QColor color,
                      QString colorParamName);  // set object's attributes
  void setAttributes (QTAChartDataSet dstype,
                      int period,
                      QString periodParamName,
                      DataSet (*TAfunc2) (const FrameVector *, int),
                      qreal rangemin,
                      qreal rangemax,
                      QColor color,
                      QString colorParamName);  // set object's attributes
  void setAttributes_common (QTAChartDataSet dstype,
                             int period,
                             QString periodParamName,
                             qreal rangemin,
                             qreal rangemax,
                             QColor color,
                             QString colorParamName);  // set object's attributes
  void setCGScriptDebug (bool mode); // set's CGScript debug mode (true from debug)
  void setDataTitle (int x); // set subchart's title to display indicator's data
  void setForDelete (void);  // set object for delete
  inline void setHAdjustment (int adj) NOEXCEPT
  {
    hadjust = adj;
  }; // set text horizontal adjustment
  inline void setVAdjustment (int adj) NOEXCEPT
  {
    vadjust = adj;
  }; // set text vertical adjustment
  void setHLine (QGraphicsLineItem *hline, qreal value); // set the price level or value of a horizontal line
  void setVLine (QGraphicsLineItem *vline); // set the date/time stamp of a vertical line
  void setVLine (QGraphicsLineItem *vline, QString trailerCandleText); // set the date/time stamp of a vertical line
  void setTLine (QGraphicsLineItem *sline); // set the edges of a trend line
  void setTLine (QGraphicsLineItem *sline, LineEdge e1, LineEdge e2); // set the edges of a trend line
  void setFibo (QGraphicsLineItem *sline); // set the edges of a fibo line
  void setFibo (QGraphicsLineItem *sline, LineEdge e1, LineEdge e2); // set the edges of a fibo line
  void setParamDialog (ParamVector pvector, QString title, QObject *parent=0); // set module's parameter dialog
  void setTitle (QString);          // set object's title
  inline void setThickness (int t) NOEXCEPT
  {
    thickness = t < 0 ? 0: (t > 4 ? 4 : t);
  }; // set object's thickness
  void setText (QGraphicsTextItem *item, qreal x, qreal y); // set the text of a label or text object
  void setText (QGraphicsTextItem *item, QString candleText, qreal price); // set the text of a text object
  inline void setPrice (qreal p) NOEXCEPT
  {
    price = p;  // set price
  }
  inline void setPeriod (int p) NOEXCEPT
  {
    period = p;  // set period
  }
  void setSymbol (const char *symbol); // set symbol for text or label
  inline void setRange (qreal min, qreal max) NOEXCEPT
  {
    rmin = min;
    rmax = max;
  }; // set the range for subcharts
// variables
  QTAChartCore *chartdata;
  QTCGraphicsItem **ITEMS;   // graphics items on screen
  QGraphicsLineItem *hvline; // horizontal or verical line
  QGraphicsTextItem *text;   // for label or text objects
  QGraphicsTextItem *title;  // object's title
  QVector < QTACObject *> children;   //  vector of children
  EdgeVector Edge;           // edges of line
  ItemVector FiboLevel;      // graphics line items for fibo levels
  QGraphicsTextItem FiboLevelLbl[7]; // text labels for FiboLevel
  QGraphicsTextItem FiboLevelPrcLbl[7]; // text labels for FiboLevel prices
  PriceVector FiboLevelPrc; // percentage fibo levels 100, 76.4, 61.8, 50, 38.2, 23.6, 0
  qint32 subchart_dec;           // decrement of nsubcharts. initially 1, 0 if delete later
  qint32 type;               // object's type
  qint32 ITEMSsize;          // size of ITEMS;
  bool deleteit;             // true if object is going to be deleted
  bool onlineprice;          // true for online price object

// private:
// functions
  void QTACObject_constructor_common (void);    // constructors' body function
  void QTACObject_destructor_common (void); // destructor's body function
  void minmax (void) GNUHOT; // find dataset's min and max on chart
  inline qreal subYonPrice (qreal price) NOEXCEPT // returns the y of a subchart that corresponds to the price argument
  {
    return basey + ((height - 10) - ((price + qAbs (rangemin)) / quantum));
  }

// variables
  ObjectSanitizer *sanitizer; //  QTACObject sanitizer
  ObjectVector Object;  // QTACObject children
  QLibrary module; // module
  ModuleInit modinit; // module initializator
  ModuleLoop modloop; // module loop
  ModuleEvent modevent; // module event
  ModuleFinish modfinish; // module finish
  ModuleValueSet modvset; // module's value set
  ModuleCompiler modcompiler; // module's compiler report
  QIcon closeicon; // icon for close button of subcharts
  QTACObjectEventFilter *filter; // event filter
  SubChartButton *closeBtn; // subchart's close button
  QPointer <SubChartButton> editBtn;    // subchart's edit button
  QGraphicsProxyWidget *prxcloseBtn; // proxy to closeBtn
  QGraphicsProxyWidget *prxeditBtn; // proxy to editBtn
  QGraphicsLineItem *bottomline; // subchart's bottom line
  QTCGraphicsScene *scene;          // ptr on chartdata->Scene
  QPointer <DynParamsDialog> paramDialog; // indicators' parameters dialog
  QTACObject *parentObject;     // parent subchart
  QTACObject *parentModule;     // parentModule
  DataSet (*TAfunc) (const DataSet, int); // function to be used
  DataSet (*TAfunc2) (const FrameVector *, int); // function to be used
  DataSet dataset;   // object's dataset
  DataSet valueset;  // object's valueset generated by TAfunc (dataset)
  Coordinate X, Y;  // coordinates
  PriceVector FiboLevelPrice; // price the FiboLevel is attached
  QString titlestr;         // title string
  QString datastr;          // indicator's data string
  QString trailerCandleText;// bottom text of candle a text object is attached on
  QString periodParamName;      // name of the parameter of DynParamsDialog for period
  QString colorParamName;       // name of the parameter of DynParamsDialog for color
  QString moduleName;           // name of the module
  QString modulePath;           // full path of dll or so
  void *dynvset;            // value set for dynamic objects
  qreal width;              // width
  qreal height;             // height
  qreal basex;              // subchart's x coordinate
  qreal basey;              // subchart's y coordinate
  qreal relx;               // relative x coordinate
  qreal rely;               // relative y coordinate
  qreal objx;               // object's x coordinate
  qreal objy;               // object's y coordinate
  qreal X1, Y1, X2, Y2;     // drawing object's received coordinates
  qreal rangemin, rmin;     // range minimum. rmin QREAL_MIN means determine from dataset
  qreal rangemax, rmax;     // range maximum. rmax QREAL_MAX means determine from dataset
  qreal quantum;            // (rmax - rmin) / (height - 10)
  qreal price;              // price level for horizontal line object
  QColor forecolor;         // foreground color
  QColor backcolor;         // background color
  qint32 thickness;         // object's thickness in points
  qint32 visibleitems;      // number of visible items
  int period;               // TAfunc's period
  int lastperiod;           // period used for last calculation
  int valuesetsize;         // size of valueset
  int hadjust;              // horizontal adjustment
  int vadjust;              // verical adjustment
  QTAChartDataSet dataset_type;   // object's dataset type
  bool enabled;             // true if enabled, false othrwise (modules only)
  bool dynamic;             // true if created by a module
  bool needsupdate;         // true if the valueset needs update (modules only)
  bool cgscriptdebug;       // true if cgscript runs in debug mode

private:
  friend void drawObject (QTACObject *);    // draw the object
  inline void moduleLock ()                 // take mutex for module
  {
    (qobject_cast <ChartApp *> (qApp))->moduleLock (this);
  }
  inline void moduleUnlock ()               // release mutex for module
  {
    (qobject_cast <ChartApp *> (qApp))->moduleUnlock (this);
  }
  void modifyFromDialog( DynParamsDialog* );

private slots:
  void modification_accepted(void);
  void modification_rejected(void);
  void updateOnlinePriceSlot (RTPrice rtprice);

signals:
  void updateOnlinePrice (RTPrice rtprice); // update online price signal
};

extern void drawObject (QTACObject *);

#endif // QTACOBJECT_H
