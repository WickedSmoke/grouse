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

#include <limits>
#include <QEvent>
#include <QKeyEvent>
#include <QShortcut>
#include "defs.h"
#include "ui_qtachart.h"
#include "qtachart_core.h"
#include "qtachart_properties.h"
#include "qtachart_help.h"
#include "qtachart_objects.h"
#include "qtachart_eventfilters.h"
#include "mainwindow.h"
#include "cgscript.h"

#ifdef DEBUG
#include <QElapsedTimer>
#endif

// constructor
QTAChart::QTAChart (QWidget * parent):
  QWidget (parent), ui (new Ui::QTAChart)
{
  QTAChartCore *core;
  QPalette palette;
  QFont font;

  classError = CG_ERR_OK;
  // dynamic allocation
  core = new (std::nothrow) QTAChartCore (this);
  if (core == NULL)
    return;

  if (core->classError != 0)
  {
    classError = core->classError;
    delete core;
    return;
  }

  ccore = core;
  tabText = QStringLiteral ("Default");

  ui->setupUi (this);

  // initialize
  core->firstshow = true;
  core->textitem = NULL;
  core->hvline = NULL;
  core->events_enabled = true;
  core->tfinit = false;
  core->forecolor = Application_Options->forecolor;
  core->framecolor = core->forecolor;
  core->textcolor = core->forecolor;
  core->backcolor = Application_Options->backcolor;
  core->gridcolor = core->forecolor;
  core->cursorcolor = Qt::yellow;
  core->linecolor = Application_Options->linecolor;
  core->barcolor = Application_Options->barcolor;

  core->titletext[0] = 0;
  core->subtitletext[0] = 0;
  core->framewidth = 10;
  core->linethickness = 3;
  core->barwidth = 1;
  core->points = 0.1;
  core->chartleftmost = 45;
  core->right_border_width = 70;
  core->title_height = 30;
  core->bottomline_height = 25;
  core->chartframe = 10;
  core->nsubcharts = 0;
  core->lineprice = &core->CLOSE;
  core->show_volumes = Application_Options->showvolume;
  core->volumes = NULL;
  core->show_grid = Application_Options->showgrid;
  core->show_onlineprice = Application_Options->showonlineprice;
  core->onlineprice = NULL;
  core->show_ruller = true;
  core->drag = false;
  core->object_drag = false;
  core->reloaded = false;
  core->redraw = true;
  core->recalc = true;
  core->linear = Application_Options->linear;
  core->currenttf = QStringLiteral ("");
  core->always_redraw = true;
  core->chart_style = Application_Options->chartstyle;
  core->height = height ();
  core->width = width ();
  core->chartrightmost = (qint64) (core->width - core->right_border_width);
  core->max_high = std::numeric_limits < qreal >::min ();
  core->min_low = std::numeric_limits < qreal >::max ();
  core->fracdig = 2;
  core->ruller_cursor_x = 200;
  core->ruller_cursor_y = 200;

  core->title->setPlainText ("");
  font = core->title->font ();
  font.setWeight (QFont::Bold);
  font.setFamily (DEFAULT_FONT_FAMILY);
  font.setStyle (QFont::StyleNormal);
  font.setPixelSize (10 + CHART_FONT_SIZE_PAD);
  core->title->setFont (font);
  core->title->setPos (core->chartleftmost, 1);

  core->gridFont = font;
  core->gridFont.setPixelSize (7 + CHART_FONT_SIZE_PAD);
  core->gridFont.setWeight (QFont::DemiBold);

  core->subtitle->setPlainText ("");
  font.setWeight (QFont::DemiBold);
  font.setFamily (DEFAULT_FONT_FAMILY);
  font.setStyle (QFont::StyleNormal);
  font.setPixelSize (8 + CHART_FONT_SIZE_PAD);
  core->subtitle->setFont (font);
  core->subtitle->setPos (core->chartleftmost, 18);

  core->scaletitle->setPlainText ("");
  font = core->scaletitle->font ();
  font.setWeight (QFont::Bold);
  font.setFamily (DEFAULT_FONT_FAMILY);
  font.setStyle (QFont::StyleNormal);
  font.setPixelSize (9 + CHART_FONT_SIZE_PAD);
  core->scaletitle->setFont (font);
  core->scaletitle->setDefaultTextColor (core->textcolor);
  core->scaletitle->setPos (core->chartrightmost - 100, 1);

  core->typetitle->setPlainText ("");
  font.setWeight (QFont::DemiBold);
  font.setStyle (QFont::StyleNormal);
  font.setPixelSize (9 + CHART_FONT_SIZE_PAD);
  font.setFamily (DEFAULT_FONT_FAMILY);
  core->typetitle->setFont (font);
  core->typetitle->setDefaultTextColor (core->textcolor);
  core->typetitle->setPos (core->chartrightmost - 100, 18);

  font = core->bottom_text->font ();
  font.setWeight (QFont::DemiBold);
  font.setFamily (DEFAULT_FONT_FAMILY);
  font.setPixelSize (8 + CHART_FONT_SIZE_PAD);
  core->bottom_text->setFont (font);
  core->bottom_text->setPlainText ("");
  core->bottom_text->setPos (core->chartleftmost, core->height - (core->bottomline_height + 5));
  core->bottom_text->setDefaultTextColor (core->textcolor);

  font = core->ruller_cursor->font ();
  font.setPixelSize (7 + CHART_FONT_SIZE_PAD);
  font.setFamily (DEFAULT_FONT_FAMILY);
  font.setWeight (QFont::DemiBold);
  core->ruller_cursor->setFont (font);
  core->ruller_cursor->setPos (core->ruller_cursor_x, core->ruller_cursor_y);
  core->ruller_cursor->setDefaultTextColor (core->backcolor);
  core->ruller_cursor->setDefaultBackgroundColor (core->forecolor);
  core->ruller_cursor->setZValue (1.0);

  core->topedge->setLine (0, 0, 0, 0);
  core->topedge->setPen (QPen (core->gridcolor));
  core->bottomedge->setLine (0, 0, 0, 0);
  core->bottomedge->setPen (QPen (Qt::darkYellow));
  core->rightedge->setLine (0, 0, 0, 0);
  core->rightedge->setPen (QPen (core->gridcolor));
  core->leftedge->setLine (0, 0, 0, 0);
  core->leftedge->setPen (QPen (core->gridcolor));

  core->chart = this->findChild < QGraphicsView * > ("graphicsView");
  core->chart->setViewportUpdateMode (QGraphicsView::FullViewportUpdate);
  core->chart->setCacheMode (QGraphicsView::CacheBackground);
  core->chart->setAlignment (Qt::AlignLeft | Qt::AlignTop);

  core->chart->setScene (core->scene);
  core->chart->installEventFilter (core->chartEventFilter);
  core->chart->setMouseTracking (true);
  core->chart->viewport()->setMouseTracking(true);

  core->scene->setItemIndexMethod (QTCGraphicsScene::NoIndex);
  core->scene->setBackgroundBrush (core->backcolor);
  core->scene->setBackgroundBrush (Qt::SolidPattern);
  core->scene->addItem (core->title);
  core->scene->addItem (core->subtitle);
  core->scene->addItem (core->scaletitle);
  core->scene->addItem (core->typetitle);
  core->scene->addItem (core->bottom_text);
  core->scene->addItem (core->ruller_cursor);
  core->scene->addItem (core->topedge);
  core->scene->addItem (core->bottomedge);
  core->scene->addItem (core->rightedge);
  core->scene->addItem (core->leftedge);
  core->scene->setObjectName ("graphicsScene");
  core->scene->installEventFilter (core->sceneEventFilter);

  // expand/shrink button
  core->expandicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/fullscreen.png"));
  core->expandBtn->setIcon (core->expandicon);
  core->expandBtn->setIconSize (QSize (32, 32));
  core->expandBtn->setFocusPolicy (Qt::NoFocus);
  core->expandBtn->setToolTip (TOOLTIP % QStringLiteral ("Expand/Restore (Alt+E)</span>"));
  core->prxexpandBtn = core->scene->addWidget (core->expandBtn, Qt::Widget);
  core->prxexpandBtn->setGeometry (QRectF (0, 0, 32, 32));

  // properies button
  core->propicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/Gear.png"));
  core->propertiesBtn->setIcon (core->propicon);
  core->propertiesBtn->setIconSize (QSize (32, 32));
  core->propertiesBtn->setFocusPolicy (Qt::NoFocus);
  core->propertiesBtn->setToolTip (TOOLTIP % QStringLiteral ("Preferences (Alt+S)</span>"));
  core->prxpropBtn = core->scene->addWidget (core->propertiesBtn, Qt::Widget);
  core->prxpropBtn->setGeometry (QRectF (0, 45, 32, 32));

  // help button
  core->helpBtn->setText (QStringLiteral ("HELP"));
  core->helpBtn->setStyleSheet
  (QStringLiteral ("background: transparent; color: %1;font: 11px;\
        font-weight: bold;border: 1px solid transparent;\
        border-color: %1;").arg (core->forecolor.name ()));
  core->helpBtn->setFocusPolicy (Qt::NoFocus);
  core->helpBtn->setToolTip (TOOLTIP % QStringLiteral ("Shortcuts (Alt+H)</span>"));
  core->prxhelpBtn = core->scene->addWidget (core->helpBtn, Qt::Widget);
  core->prxhelpBtn->setGeometry (QRectF(core->chartrightmost + 5,
                                        core->height - (core->bottomline_height + 2),
                                        core->right_border_width - 10,
                                        core->bottomline_height - 10));

  // data button
  core->dataicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/Blocknotes.png"));
  core->dataBtn->setIcon (core->dataicon);
  core->dataBtn->setIconSize (QSize (32, 32));
  core->dataBtn->setFocusPolicy (Qt::NoFocus);
  core->dataBtn->setToolTip (TOOLTIP % QStringLiteral ("Data (Alt+Y)</span>"));
  core->prxdataBtn = core->scene->addWidget (core->dataBtn, Qt::Widget);
  core->prxdataBtn->setGeometry (QRectF(0, 90, 32, 32));

  // zoom in button
  core->zoominicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/Zoom_In.png"));
  core->zoomInBtn->setIcon (core->zoominicon);
  core->zoomInBtn->setIconSize (QSize (32, 32));
  core->zoomInBtn->setFocusPolicy (Qt::NoFocus);
  core->zoomInBtn->setToolTip (TOOLTIP % QStringLiteral ("Zoom In (+)</span>"));
  core->prxzoominBtn = core->scene->addWidget (core->zoomInBtn, Qt::Widget);
  core->prxzoominBtn->setGeometry (QRectF(0, 135, 32, 32));

  // zoom out button
  core->zoomouticon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/Zoom_Out.png"));
  core->zoomOutBtn->setIcon (core->zoomouticon);
  core->zoomOutBtn->setIconSize (QSize (32, 32));
  core->zoomOutBtn->setFocusPolicy (Qt::NoFocus);
  core->zoomOutBtn->setToolTip (TOOLTIP % QStringLiteral ("Zoom Out (-)</span>"));
  core->prxzoomoutBtn = core->scene->addWidget (core->zoomOutBtn, Qt::Widget);
  core->prxzoomoutBtn->setGeometry (QRectF(0, 180, 32, 32));

  // function button
  core->functionicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/Chart_Graph_Descending.png"));
  core->functionBtn->setIcon (core->functionicon);
  core->functionBtn->setIconSize (QSize (32, 32));
  core->functionBtn->setFocusPolicy (Qt::NoFocus);
  core->functionBtn->setToolTip (TOOLTIP % QStringLiteral ("Indicators (Alt+F)</span>"));
  core->prxfunctionBtn = core->scene->addWidget (core->functionBtn, Qt::Widget);
  core->prxfunctionBtn->setGeometry (QRectF(0, 225, 32, 32));

  // draw button
  core->drawicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/Application_Blueprint.png"));
  core->drawBtn->setIcon (core->drawicon);
  core->drawBtn->setIconSize (QSize (32, 32));
  core->drawBtn->setFocusPolicy (Qt::NoFocus);
  core->drawBtn->setToolTip (TOOLTIP % QStringLiteral ("Draw (Alt+D)</span>"));
  core->prxdrawBtn = core->scene->addWidget (core->drawBtn, Qt::Widget);
  core->prxdrawBtn->setGeometry (QRectF(0, 270, 32, 32));

  // objects button
  core->objectsicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/File_List.png"));
  core->objectsBtn->setIcon (core->objectsicon);
  core->objectsBtn->setIconSize (QSize (32, 32));
  core->objectsBtn->setFocusPolicy (Qt::NoFocus);
  core->objectsBtn->setToolTip (TOOLTIP % QStringLiteral ("Manage Objects (Alt+O)</span>"));
  core->prxobjectsBtn = core->scene->addWidget (core->objectsBtn, Qt::Widget);
  core->prxobjectsBtn->setGeometry (QRectF(0, 315, 32, 32));

  // properties screen
  core->propScr->setStyleSheet (QStringLiteral ("background:transparent;color:white;"));
  core->propScr->setVisible (false);
  core->prxpropScr = core->scene->addWidget (core->propScr, Qt::Widget);
  core->prxpropScr->setPos (0, 40);
  core->prxpropScr->resize (core->width, core->height - 40);
  core->propScr->setVolumes (core->show_volumes);
  core->propScr->setGrid (core->show_grid);
  core->setChartStyle (core->chart_style);
  core->setLinearScale (core->linear);
  core->propScr->setOnlinePrice (core->show_onlineprice);

  // draw screen
  core->drawScr->setStyleSheet (QStringLiteral ("background:transparent;color:white;"));
  core->drawScr->setVisible (false);
  core->prxdrawScr = core->scene->addWidget (core->drawScr, Qt::Widget);
  core->prxdrawScr->setPos (0, 40);
  core->prxdrawScr->resize (core->width, core->height - 40);

  // function screen
  core->functionScr->setStyleSheet (QStringLiteral ("background:transparent;color:white;"));
  core->functionScr->setVisible (false);
  core->prxfunctionScr = core->scene->addWidget (core->functionScr, Qt::Widget);
  core->prxfunctionScr->setPos (0, 40);
  core->prxfunctionScr->resize (core->width, core->height - 40);

  // objects screen
  core->objectsScr->setStyleSheet (QStringLiteral ("background:transparent;color:white;"));
  core->objectsScr->setVisible (false);
  core->prxobjectsScr = core->scene->addWidget (core->objectsScr, Qt::Widget);
  core->prxobjectsScr->setPos (0, 40);
  core->prxobjectsScr->resize (core->width, core->height - 40);

  // help screen
  core->helpScr->setStyleSheet (QStringLiteral ("background:transparent;color:white;"));
  core->helpScr->setVisible (false);
  core->prxhelpScr = core->scene->addWidget (core->helpScr, Qt::Widget);
  core->prxhelpScr->setPos (0, 40);
  core->prxhelpScr->resize (core->width, core->height - 40);

  // data screen
  core->dataScr->setStyleSheet (QStringLiteral ("background:transparent;color:white;"));
  core->dataScr->setVisible (false);
  core->prxdataScr = core->scene->addWidget (core->dataScr, Qt::Widget);
  core->prxdataScr->setPos (0, 40);
  core->prxdataScr->resize (core->width, core->height - 40);

  setFocusProxy (&core->View);

  connect (core->propertiesBtn, SIGNAL (clicked ()), this,
           SLOT (propertiesBtn_clicked ()));

  connect (core->helpBtn, SIGNAL (clicked ()), this,
           SLOT (helpBtn_clicked ()));

  connect (core->dataBtn, SIGNAL (clicked ()), this,
           SLOT (dataBtn_clicked ()));

  connect (core->zoomInBtn, SIGNAL (clicked ()), this,
           SLOT (zoomInBtn_clicked ()));

  connect (core->zoomOutBtn, SIGNAL (clicked ()), this,
           SLOT (zoomOutBtn_clicked ()));

  connect (core->expandBtn, SIGNAL (clicked ()), this,
           SLOT (expandBtn_clicked ()));

  connect (core->drawBtn, SIGNAL (clicked ()), this,
           SLOT (drawBtn_clicked ()));

  connect (core->functionBtn, SIGNAL (clicked ()), this,
           SLOT (functionBtn_clicked ()));

  connect (core->objectsBtn, SIGNAL (clicked ()), this,
           SLOT (objectsBtn_clicked ()));
}

// destructor
QTAChart::~QTAChart ()
{
  if (classError == CG_ERR_NOMEM)
    return;

  const QTAChartCore *core = ccore;

  if (core->VOLUME.size () > 0)
    core->saveSettings ();

  ArrayDestroyAll_imp (this);
  StringDestroyAll_imp (this);

  delete ui;
}

// back button implementation
void
QTAChart::goBack (void)
{
  QTAChartCore *core = ccore;

  if (core->gridstep <= 1)
    return;

  core->expandicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/fullscreen.png"));
  core->expandBtn->setIcon (core->expandicon);
  core->expandBtn->setToolTip (TOOLTIP % QStringLiteral ("Expand/Restore (Alt+E)</span>"));
  core->events_enabled = true;
  core->showAllItems ();

  core->prxfunctionScr->setVisible (false);
  core->prxpropScr->setVisible (false);
  core->prxhelpScr->setVisible (false);
  core->prxdataScr->setVisible (false);
  core->prxdrawScr->setVisible (false);
  core->prxobjectsScr->setVisible (false);

  core->show_volumes = core->propScr->Volumes ();
  core->show_grid = core->propScr->Grid ();
  core->setChartStyle (core->propScr->ChartStyle ());
  core->linecolor = core->propScr->lineColor ();
  core->barcolor = core->propScr->barColor ();
  core->setLinearScale (core->propScr->LinearScale ());
  core->forecolor = core->propScr->foreColor ();
  core->backcolor = core->propScr->backColor ();
  core->show_onlineprice = core->propScr->OnlinePrice ();
  core->changeForeColor (core->forecolor);
  core->scene->setBackgroundBrush (core->backcolor);
  core->ruller_cursor->setDefaultTextColor (core->backcolor);
  core->ruller_cursor->setDefaultBackgroundColor (core->forecolor);

  QString btnStyleSheet =
    QString ("background: transparent; color: %1; \
            border: 1px solid transparent;  border-color: %1;").arg (core->backcolor.name ());

  core->expandBtn->setStyleSheet (btnStyleSheet);
  core->propertiesBtn->setStyleSheet (btnStyleSheet);
  core->dataBtn->setStyleSheet (btnStyleSheet);
  core->zoomInBtn->setStyleSheet (btnStyleSheet);
  core->zoomOutBtn->setStyleSheet (btnStyleSheet);
  core->functionBtn->setStyleSheet (btnStyleSheet);
  core->drawBtn->setStyleSheet (btnStyleSheet);
  core->objectsBtn->setStyleSheet (btnStyleSheet);
  core->helpBtn->setStyleSheet
  (QString ("background: transparent; color: %1;font: 11px;\
        font-weight: bold;border: none;").arg (core->forecolor.name ()));

  if (core->propScr->Volumes ())
    core->drawVolumes ();
  else
    core->deleteVolumes ();

  core->deleteITEMS ();
  core->draw ();
}

/// Events
///
// resize
void
QTAChart::resizeEvent (QResizeEvent * event)
{
  QTAChartCore *core = ccore;

  if (event->oldSize () == event->size ())
    return;

  core->setSizeChanged ();
  core->height = height ();
  core->width = width ();

  core->chartrightmost = (qint64) (core->width - core->right_border_width);
  core->title->setPos (core->chartleftmost, 1);
  core->subtitle->setPos (core->chartleftmost, 18);
  core->scaletitle->setPos (core->chartrightmost - 100, 1);
  core->typetitle->setPos (core->chartrightmost - 100, 18);
  core->prxhelpBtn->setPos (core->chartrightmost + 5,
                            core->height - (core->bottomline_height + 2));
  core->chart->resize (core->width, core->height);
  core->scene->setSceneRect (0, 0, core->width - 5, core->height - 5);
  if (core->events_enabled == true)
  {
    core->draw ();
    return;
  }

  core->prxpropScr->resize (core->width, core->height - 40);
  core->prxhelpScr->resize (core->width, core->height - 40);
  core->prxdataScr->resize (core->width, core->height - 40);
  core->prxdrawScr->resize (core->width, core->height - 40);
  core->prxobjectsScr->resize (core->width, core->height - 40);
  core->prxfunctionScr->resize (core->width, core->height - 40);
}

// show event
void
QTAChart::showEvent (QShowEvent * event)
{
  QTAChartCore *core = ccore;

  if (event->spontaneous ())
    return;

  if (core->firstshow == true)
  {
    TemplateManagerDialog *tmpldlg = new TemplateManagerDialog;
    tmpldlg->setReferenceChart (static_cast <void*> (this));
    tmpldlg->attachtemplate (QStringLiteral ("template_") % core->SymbolKey);
    delete tmpldlg;
    core->firstshow = false;
  }
}

// keypress (+,-, Alt + S)
void
QTAChart::keyPressEvent (QKeyEvent * event)
{
  QTAChartCore *core = ccore;

  if (core->object_drag)
    return;

  // Alt
  if (event->modifiers () & Qt::AltModifier)
  {
    // expand (Alt + E)
    if (event->key () == Qt::Key_E && core->events_enabled == true)
    {
      expandBtn_clicked ();
      return;
    }
    else
    // properties (Alt + S)
    if (event->key () == Qt::Key_S && core->events_enabled == true)
    {
      propertiesBtn_clicked ();
      return;
    }
    else
    // help (Alt + H)
    if (event->key () == Qt::Key_H && core->events_enabled == true)
    {
      helpBtn_clicked ();
      return;
    }
    else
    // data (Alt + Y)
    if (event->key () == Qt::Key_Y && core->events_enabled == true)
    {
      dataBtn_clicked ();
      return;
    }
    else
    // indicators (Alt + F)
    if (event->key () == Qt::Key_F && core->events_enabled == true)
    {
      functionBtn_clicked ();
      return;
    }
    else
    // draw (Alt + D)
    if (event->key () == Qt::Key_D && core->events_enabled == true)
    {
      drawBtn_clicked ();
      return;
    }
    else
    // objects (Alt + O)
    if (event->key () == Qt::Key_O && core->events_enabled == true)
    {
      objectsBtn_clicked ();
      return;
    }
    else
    // back (Alt + Z)
    if (event->key () == Qt::Key_Z)
    {
      backBtn_clicked ();
      return;
    }
    else if (!core->events_enabled)
    {
      return;
    }
    else
    // line chart (Alt + L)
    if (event->key () == Qt::Key_L)
    {
      core->deleteITEMS ();
      core->setChartStyle (QTACHART_LINE);
      goto EventEndLbl;
    }
    else
    // candle chart (Alt + C)
    if (event->key () == Qt::Key_C)
    {
      core->deleteITEMS ();
      core->setChartStyle (QTACHART_CANDLE);
      goto EventEndLbl;
    }
    else
    // heikin-ashi chart (Alt + A)
    if (event->key () == Qt::Key_A)
    {
      core->deleteITEMS ();
      core->setChartStyle (QTACHART_HEIKINASHI);
      goto EventEndLbl;
    }
    else
    // bar chart (Alt + B)
    if (event->key () == Qt::Key_B)
    {
      core->deleteITEMS ();
      core->setChartStyle (QTACHART_BAR);
      goto EventEndLbl;
    }
    else
    // volumes on/off (Alt + V)
    if (event->key () == Qt::Key_V)
    {
      if (core->propScr->Volumes ())
        core->deleteVolumes ();
      else
        core->drawVolumes ();
      goto EventEndLbl;
    }
    else
    // grid on/off (Alt + G)
    if (event->key () == Qt::Key_G)
    {
      if (core->propScr->Grid ())
        core->propScr->setGrid (false);
      else
        core->propScr->setGrid (true);
      core->show_grid = core->propScr->Grid ();
      goto EventEndLbl;
    }
    else
    // online price on/off (Alt + P)
    if (event->key () == Qt::Key_P)
    {
      if (core->propScr->OnlinePrice ())
        core->propScr->setOnlinePrice (false);
      else
        core->propScr->setOnlinePrice (true);

      core->show_onlineprice = core->propScr->OnlinePrice ();
      goto EventEndLbl;
    }
    else
    // linear price scale on/off (Alt + X)
    if (event->key () == Qt::Key_X)
    {
      if (core->propScr->LinearScale ())
        core->setLinearScale (false);
      else
        core->setLinearScale (true);

      goto EventEndLbl;
    }
    else
      return;
  }

  if (!core->events_enabled)
    return;

  // plus
  if (event->key () == Qt::Key_Plus || event->key () == Qt::Key_Equal)
  {
    if (core->framewidth < 25)
      core->framewidth++;
    else
      return;
  }
  else
  // minus
  if (event->key () == Qt::Key_Minus)
  {
    if (core->framewidth > 3)
      core->framewidth--;
    else
      return;
  }
  else
    return;

EventEndLbl:
  core->draw ();
}

void
QTAChart::backBtn_clicked (void)
{
  goBack ();
}

// properties button
void
QTAChart::propertiesBtn_clicked (void)
{
  QTAChartCore *core = ccore;

  if (core->object_drag)
    return;

  core->setChartProperties ();
}

// expand button
void
QTAChart::expandBtn_clicked (void)
{
  const QTAChartCore *core = ccore;
  MainWindow *mainwindow;

  if (core->object_drag)
    return;

  mainwindow = (qobject_cast <MainWindow *> (this->parentWidget ()->parentWidget ()->
                parentWidget ()->parentWidget ()));

  if (core->events_enabled == true)
  {
    if (mainwindow->expandedChart ())
      mainwindow->setExpandChart (false);
    else
      mainwindow->setExpandChart (true);
    return;
  }
  else
    backBtn_clicked ();

}

// help button
void
QTAChart::helpBtn_clicked (void)
{
  QTAChartCore *core = ccore;

  if (core->object_drag)
    return;

  if (core->events_enabled == true)
  {
    core->hideAllItems ();
    core->showHelp ();
    core->events_enabled = false;
    return;
  }
}

// data button
void
QTAChart::dataBtn_clicked (void)
{
  QTAChartCore *core = ccore;

  if (core->object_drag)
    return;

  if (core->events_enabled == true)
  {
    core->hideAllItems ();
    core->showData ();
    core->events_enabled = false;
    return;
  }
}

// zoom in button
void
QTAChart::zoomInBtn_clicked (void)
{
  const QTAChartCore *core = ccore;

  if (core->object_drag)
    return;

  QKeyEvent event = QKeyEvent(QEvent::KeyPress, Qt::Key_Plus, Qt::NoModifier,
                              QStringLiteral ("+"), false, 1);
  keyPressEvent (&event);
}

// zoom out button
void
QTAChart::zoomOutBtn_clicked (void)
{
  const QTAChartCore *core = ccore;

  if (core->object_drag)
    return;

  QKeyEvent event = QKeyEvent(QEvent::KeyPress, Qt::Key_Minus, Qt::NoModifier,
                              QStringLiteral ("-"), false, 1);
  keyPressEvent (&event);
}

void
QTAChart::drawBtn_clicked (void)
{
  QTAChartCore *core = ccore;

  if (core->object_drag)
    return;

  core->selectDrawObject ();
}

// function button
void
QTAChart::functionBtn_clicked (void)
{
  QTAChartCore *core = ccore;

  if (core->object_drag)
    return;

  core->selectFunction ();
}

// objects button
void
QTAChart::objectsBtn_clicked (void)
{
  QTAChartCore *core = ccore;

  if (core->object_drag)
    return;

  core->manageObjects ();
}

// get chart's symbol database key
QString
QTAChart::getSymbolKey ()
{
  return ccore->SymbolKey;
}

/*
 * unused. keep it here for possible future use
 *
// qSort callback for descending sort
static bool
descend (const QTAChartFrame & v1, const QTAChartFrame & v2) NOEXCEPT
{
  static QString ds1, ds2;
  ds1 = QString (v1.Date);
  ds2 = QString (v2.Date);

  if (ds1 != ds2)
    return ds1 > ds2;

  ds1 = QString (v1.Time);
  ds2 = QString (v2.Time);

  return ds1 > ds2;
}
*/

// load the frames
// callback for sqlite3_exec
static int
sqlcb_table_data (void *classptr, int argc, char **argv, char **column)
{
  TableDataClass *tdc = static_cast <TableDataClass *> (classptr);

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();
    // symbol,  timeframe, description, adjusted, base, market, source
    if (colname == QLatin1String ("SYMBOL"))
      tdc->symbol = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("TIMEFRAME"))
      tdc->timeframe = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("DESCRIPTION"))
      tdc->name = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("ADJUSTED"))
      tdc->adjusted = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("BASE"))
      tdc->base = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("MARKET"))
      tdc->market = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("SOURCE"))
      tdc->source = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("LASTUPDATE"))
      tdc->lastupdate = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("CURRENCY"))
      tdc->currency = QString (argv[counter]);
  }
  return 0;
}

/*
inline static qreal
MAX (qreal a, qreal b)
{
  if (a > b)
    return a;

  return b;
}

#define MAX(a,b)        (a>b?a:b)
*/

qreal
MAX (qreal a, qreal b, qreal c)
{
  int m = a;
  (void)((m < b) && (m = b)); //these are not conditional statements.
  (void)((m < c) && (m = c)); //these are just boolean expressions.
  return m;
}

void
QTAChart::loadFrames (QString tablename)
{
  const QString
  dash ("-"), D ("D: "), SP (" "), O (" O: "), C ("  C: "),
       H ("  H: "), L ("  L: ");
  TFClass TF;
  FrameVector frames;
  TableDataClass tdc;
  QString btext, SQLCommand;
  QStringList ymd;
  QTAChartFrame frame, haframe, prevframe;
  QTAChartCore *core = ccore;
  qreal excess_drag_width = 0;
  qint32 nframes, startbar = 0;
  int rc;
  bool ok;

#ifdef DEBUG
  QElapsedTimer timer;
  timer.start();
#endif

  ResourceMutex->lock ();
  classError = CG_ERR_OK;
  // sqlite3_release_memory(33554432);

// save the current excess_drag_width
  for (qint32 counter = 0; counter < core->TIMEFRAME.size (); counter ++)
  {
    if (core->TIMEFRAME[counter].TFTablename == tablename)
    {
      excess_drag_width = core->TIMEFRAME[counter].TFExcess_Drag_Width;
      if (core->tfinit)
        startbar = core->TIMEFRAME[counter].TFStartBar;
    }
  }

// read tablename row from symbols_l2
  SQLCommand = QStringLiteral ("SELECT * FROM symbols_l2 WHERE KEY = '") %
               tablename % QStringLiteral ("';");
  rc =  selectfromdb (SQLCommand.toUtf8(),
                      sqlcb_table_data, static_cast <void *> (&tdc));
  if (rc != SQLITE_OK)
  {
    classError = CG_ERR_ACCESS_DATA;
    setGlobalError(classError, __FILE__, __LINE__);
    goto loadFrames_end;
  }
  TF.TFTablename = tablename;

// read the frames from disc
  frames.reserve (32768);
  SQLCommand = QStringLiteral ("SELECT * FROM ") % tablename % QStringLiteral (" ORDER BY DATE DESC, TIME DESC;");
  rc =  selectfromdb (SQLCommand.toUtf8(),
                      sqlcb_dataframes, static_cast <void *> (&frames));
  if (rc != SQLITE_OK)
  {
    classError = CG_ERR_ACCESS_DATA;
    setGlobalError(classError, __FILE__, __LINE__);
    goto loadFrames_end;
  }
/*
#ifdef DEBUG
  qDebug () << "Frames loaded in:" << timer.elapsed();
  timer.start();
#endif
*/
  if (frames.count () == 0)
  {
    classError = CG_ERR_NO_QUOTES;
    setGlobalError(classError, __FILE__, __LINE__);
    goto loadFrames_end;
  }

  // if market is London and longbp = true;
  if ((tdc.currency.trimmed ().left (3) == QLatin1String ("GBp") ||
       tdc.market.trimmed ().left (3) == QLatin1String ("LON")) &&
      Application_Options->longbp)
  {
    int fc = frames.count ();
    for (qint32 counter = 0; counter < fc; counter ++)
    {
      frames[counter].High /= 100;
      frames[counter].Low /= 100;
      frames[counter].Open /= 100;
      frames[counter].Close /= 100;
      frames[counter].AdjClose /= 100;
    }
  }

// populate frame vector
  TF.TFStartBar = startbar;
  TF.TFExcess_Drag_Width = excess_drag_width;
  TF.HLOC.clear ();
  TF.HEIKINASHI.clear ();
  TF.TFName = tdc.timeframe;
  TF.TFSymbol = tdc.timeframe.left (1);
  TF.TFMarket = tdc.market;
  TF.TFCurrency = tdc.currency;
  nframes = frames.size ();

  for (qint32 counter = 0; counter < nframes; counter ++)
  {
    frame = frames.at (counter);

    ymd = QString (frame.Date).split(dash, QString::KeepEmptyParts);
    frame.year = frame.month = frame.day = 0;
    if (ymd.size () > 2)
    {
      frame.year = ymd.at (0).toUShort(&ok, 10);
      frame.month = ymd.at (1).toUShort(&ok, 10);
      frame.day = ymd.at (2).toUShort(&ok, 10);
    }

    btext =
      D %
      QString (frame.Date) %
      SP %
      QString (frame.Time) %
      O %
      QString::number (frame.Open, 'f', fracdig (frame.Open)) %
      C %
      QString::number (frame.Close, 'f', fracdig (frame.Close)) %
      H %
      QString::number (frame.High, 'f', fracdig (frame.High)) %
      L %
      QString::number (frame.Low, 'f', fracdig (frame.Low));

    frame.Text = btext;
    frames[counter] = frame;
  }

  core->reloaded = true;
  TF.HLOC = frames;
/*
#ifdef DEBUG
  qDebug () << "Candles completed in:" << timer.elapsed();
  timer.start();
#endif
*/
  if (nframes < 2)
  {
    core->TIMEFRAME += TF;
    goto loadFrames_end;
  }

// heikinashi
  /*
    TF.HEIKINASHI.prepend (TF.HLOC.at (nframes - 1)); // <-- ATTENTION: THIS IS VERY SLOW
  */
  TF.HEIKINASHI.reserve (nframes);
  prevframe = TF.HLOC.at (nframes - 1);
  for (qint32 counter = 0; counter < nframes; counter ++)
    TF.HEIKINASHI.append (prevframe);

  for (qint32 counter = nframes - 2; counter > -1; counter--)
  {
    frame = haframe = TF.HLOC.at (counter);

    haframe.Close = (frame.Open + frame.High + frame.Close + frame.Low) / 4;
    haframe.Open = (prevframe.Open + prevframe.Close) / 2;
    haframe.High = qMax (qMax(haframe.Close, haframe.Open), frame.High);
    haframe.Low =  qMin (qMin(haframe.Close, haframe.Open), frame.Low);

    btext =
      D %
      QString (haframe.Date) %
      SP %
      QString (haframe.Time) %
      O %
      QString::number (haframe.Open, 'f', fracdig (haframe.Open)) %
      C %
      QString::number (haframe.Close, 'f', fracdig (haframe.Close)) %
      H %
      QString::number (haframe.High, 'f', fracdig (haframe.High)) %
      L %
      QString::number (haframe.Low, 'f', fracdig (haframe.Low));

    haframe.Text = btext;
    /*
        TF.HEIKINASHI.prepend (haframe); // <-- ATTENTION: THIS IS VERY SLOW
    */
    TF.HEIKINASHI[counter] = haframe;
    prevframe  = haframe;
  }
/*
#ifdef DEBUG
  qDebug () << "HA completed in:" << timer.elapsed();
  timer.start();
#endif
*/
  for (qint32 counter = 0; counter < core->TIMEFRAME.size (); counter ++)
  {
    if (core->TIMEFRAME.at (counter).TFTablename == TF.TFTablename)
    {
      core->TIMEFRAME[counter] = TF;
      goto loadFrames_end;
    }
  }
  core->TIMEFRAME.append (TF);

loadFrames_end:

  ResourceMutex->unlock ();
/*
#ifdef DEBUG
  qDebug () << "Chart Creation completed in:" << timer.elapsed();
  qDebug () << "===========";
#endif
*/
  return;
}

// load data
void
QTAChart::loadData (QTAChartData data)
{
  QString textdata = QStringLiteral ("");

  textdata += QStringLiteral ("Book Value:  ") % data.bv % QStringLiteral ("\n\n") %
              QStringLiteral ("Market Cap:  ") % data.mc % QStringLiteral ("\n\n") %
              QStringLiteral ("EBITDA:  ") % data.ebitda % QStringLiteral ("\n\n") %
              QStringLiteral ("Price/Earnings:  ") % data.pe % QStringLiteral ("\n\n") %
              QStringLiteral ("PEG Ratio:  ") % data.peg % QStringLiteral ("\n\n") %
              QStringLiteral ("Dividend Yield:  ") % data.dy % QStringLiteral ("\n\n") %
              QStringLiteral ("EPS Current:  ") % data.epscurrent % QStringLiteral ("\n\n") %
              QStringLiteral ("EPS Next:  ") % data.epsnext % QStringLiteral ("\n\n") %
              QStringLiteral ("Earnings/Share:  ") % data.es % QStringLiteral ("\n\n") %
              QStringLiteral ("Price/Sales:  ") % data.ps % QStringLiteral ("\n\n") %
              QStringLiteral ("Price/Book:  ") % data.pbv % QStringLiteral ("\n\n");
  ccore->dataScr->setData (textdata);
}

// restore bottom text
void
QTAChart::restoreBottomText ()
{
  ccore->restoreBottomText ();
}

// always redraw chart on/off
void
QTAChart::setAlwaysRedraw (bool boolean)
{
  ccore->always_redraw = boolean;
}

// set chart's symbol
void
QTAChart::setSymbol (QString symbol)
{
  ccore->Symbol = symbol;
}

// set chart's symbol key
void
QTAChart::setSymbolKey (QString key)
{
  ccore->SymbolKey = key;
  ccore->loadSettings ();
}

// set symbol's feed
void
QTAChart::setFeed (QString feed)
{
  ccore->Feed = feed;
}

// set chart's title
void
QTAChart::setTitle (QString title, QString subtitle)
{
  ccore->setTitle (title, subtitle);
}

// set the bottom text to custom string
void
QTAChart::setCustomBottomText (QString string)
{
  ccore->setCustomBottomText (string);
}

// set linear chart on/off
void
QTAChart::setLinear (bool boolean)
{
  ccore->setLinearScale (boolean);
}

// set chart volumes on/off
void
QTAChart::showVolumes (bool boolean)
{
  ccore->show_volumes = boolean;
}

// set chart grid on/off
void
QTAChart::showGrid (bool boolean)
{
  ccore->show_grid = boolean;
}

// set online price on/off
void
QTAChart::showOnlinePrice (bool boolean)
{
  ccore->show_onlineprice = boolean;
}


ParamVector::~ParamVector()
{
    QVector<DynParam*>::iterator it;
    for( it = begin(); it != end(); ++it )
        delete *it;
}

DynParam* ParamVector::addParameter( const QString& name, qint32 type,
                                     qreal value )
{
    DynParam *param = new DynParam(name);
    param->type = type;
    param->defvalue = value;
    param->value = value;
    push_back( param );
    return param;
}


// Return pointer to named paramater or nullptr if the name doesn't exist.
const DynParam* ParamVector::constParameter( const QString& name ) const
{
    const_iterator it;
    for( it = begin(); it != end(); ++it )
    {
        if( (*it)->paramName == name )
            return *it;
    }
    return nullptr;
}


QTACObject* QTAChart::addStudyMACD( const QString& name, int period,
                                    QRgb colorMACD, QRgb colorSignal )
{
    if( period < 1 )
        return nullptr;

    QTACObject* obj = new QTACObject(ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes(QTACHART_CLOSE, period, QStringLiteral("Period"),
                       MACD, QREAL_MIN, QREAL_MAX,
                       Qt::white, QStringLiteral(""));
    obj->setTitle(name);

    QTACObject* childobj;
    childobj = new QTACObject(obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral("Period"),
                            MACD, QREAL_MIN, QREAL_MAX,
                            colorMACD, QStringLiteral("MACD color"));

    childobj = new QTACObject(obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral("Period"),
                            MACDSIGNAL, QREAL_MIN, QREAL_MAX,
                            colorSignal, QStringLiteral("Signal color"));

    childobj = new QTACObject(obj, QTACHART_OBJ_VBARS);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral("Period"),
                             MACDHIST, QREAL_MIN, QREAL_MAX,
                             Qt::white, QStringLiteral(""));

    childobj = new QTACObject(obj, QTACHART_OBJ_HLINE);
    childobj->setHLine(NULL, 0);
    childobj->setAttributes(QTACHART_CLOSE, 0, QStringLiteral(""),
                            DUMMY, 0, 0, Qt::black, QStringLiteral(""));

    return obj;
}

QTACObject* QTAChart::addStudySMA( const QString& name, int period, QRgb color )
{
    if( period < 1 )
        return nullptr;

    QTACObject* obj = new QTACObject(ccore, QTACHART_OBJ_CURVE);
    obj->setAttributes(QTACHART_CLOSE, period, QStringLiteral("Period"),
                       SMA, 0, 0, color, QStringLiteral("Color"));
    obj->setTitle(name);
    return obj;
}

QTACObject* QTAChart::addStudyEMA( const QString& name, int period, QRgb color )
{
    if( period < 1 )
        return nullptr;

    QTACObject* obj = new QTACObject(ccore, QTACHART_OBJ_CURVE);
    obj->setAttributes(QTACHART_CLOSE, period, QStringLiteral("Period"),
                       EMA, 0, 0, color, QStringLiteral("Color"));
    obj->setTitle(name);
    return obj;
}

QTACObject* QTAChart::addStudyParabolicSAR( const QString& name, QRgb color )
{
    QTACObject* obj = new QTACObject (ccore, QTACHART_OBJ_DOT);
    obj->setAttributes(QTACHART_CLOSE, 1, QStringLiteral(""),
                       PSAR, 0, 0, color, QStringLiteral("Color"));
    obj->setTitle(name);
    return obj;
}

QTACObject* QTAChart::addStudyRSI( const QString& name, int period,
                                   int highLevel, int lowLevel, QRgb color )
{
    QTACObject *obj, *childobj;

    if( period < 1 )
        return nullptr;

    obj = new QTACObject (ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes(QTACHART_CLOSE, period, QStringLiteral ("Period"),
                       DUMMY, 0, 100, color, QStringLiteral (""));
    obj->setTitle (name);

    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral("Period"),
                            RSI, 0, 100, color, QStringLiteral ("Color"));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, highLevel);
    childobj->setAttributes(QTACHART_CLOSE, period,
                            QStringLiteral("High level"),
                            DUMMY, 0, 100, color, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, lowLevel);
    childobj->setAttributes(QTACHART_CLOSE, period,
                            QStringLiteral ("Low level"),
                            DUMMY, 0, 100, color, QStringLiteral (""));
    return obj;
}

QTACObject* QTAChart::addStudyMFI( const QString& name, int period, QRgb color,
                                   int highLevel, int lowLevel,
                                   int mediumLevel )
{
    QTACObject *childobj;

    if( period < 1 )
        return nullptr;

    QTACObject* obj = new QTACObject (ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"),
                        DUMMY, 0, 100, color, QStringLiteral (""));
    obj->setTitle (name);
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"),
                             MFI, 0, 100, color, QStringLiteral ("Color"));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, highLevel);
    childobj->setAttributes (QTACHART_CLOSE, period,
                             QStringLiteral ("High level"),
                             DUMMY, 0, 100, color, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, lowLevel);
    childobj->setAttributes (QTACHART_CLOSE, period,
                             QStringLiteral ("Low level"),
                             DUMMY, 0, 100, color, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, mediumLevel);
    childobj->setAttributes (QTACHART_CLOSE, period,
                             QStringLiteral ("Medium level"),
                             DUMMY, 0, 100, color, QStringLiteral (""));

    return obj;
}

QTACObject* QTAChart::addStudyROC( const QString& name, int period, int Level,
                                   QRgb color )
{
    QTACObject *obj, *childobj;

    if (period < 1)
      return nullptr;

    obj = new QTACObject (ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), ROC, QREAL_MIN, QREAL_MAX, color, QStringLiteral (""));
    obj->setTitle (name);
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), ROC, QREAL_MIN, QREAL_MAX, color, QStringLiteral ("Color"));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Level);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Level"), DUMMY, 0, 0, color, QStringLiteral (""));

    return obj;
}

QTACObject* QTAChart::addStudySlowStoch( const QString& name, int period,
                               int Highlevel, int Mediumlevel, int Lowlevel,
                               QRgb Kcolor, QRgb Dcolor )
{
    QTACObject *obj, *childobj;

    if (period < 1)
      return nullptr;

    obj = new QTACObject (ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DUMMY, 0, 100, Dcolor, QStringLiteral (""));
    obj->setTitle (name);
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), STOCHSLOWD, 0, 100, Dcolor, "%D color");
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), STOCHSLOWK, 0, 100, Kcolor, "%K color");
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Highlevel);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("High level"), DUMMY, 0, 100, Kcolor, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Lowlevel);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Low level"), DUMMY, 0, 100, Kcolor, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Mediumlevel);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Medium level"), DUMMY, 0, 100, Kcolor, QStringLiteral (""));

    return obj;
}

QTACObject* QTAChart::addStudyFastStoch( const QString& name, int period,
                                 int Highlevel, int Mediumlevel, int Lowlevel,
                                 QRgb Kcolor, QRgb Dcolor )
{
    QTACObject *obj, *childobj;

    if (period < 1)
      return nullptr;

    obj = new QTACObject (ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DUMMY, 0, 100, Dcolor, QStringLiteral (""));
    obj->setTitle (name);
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), STOCHFASTD, 0, 100, Dcolor, "%D color");
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), STOCHFASTK, 0, 100, Kcolor, "%K color");
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Highlevel);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("High level"), DUMMY, 0, 100, Kcolor, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Lowlevel);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Low level"), DUMMY, 0, 100, Kcolor, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Mediumlevel);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Medium level"), DUMMY, 0, 100, Kcolor, QStringLiteral (""));

    return obj;
}

QTACObject* QTAChart::addStudyW_percent_R( const QString& name, int period,
                                           int Highlevel, int Lowlevel,
                                           QRgb color )
{
    QTACObject *obj, *childobj;

    if (period < 1)
      return nullptr;

    obj = new QTACObject (ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DUMMY, -100, 0, color, QStringLiteral (""));
    obj->setTitle (name);
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), WILLR, -100, 0, color, QStringLiteral ("Color"));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Highlevel);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("High level"), DUMMY, -100, 0, color, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Lowlevel);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Low level"), DUMMY, -100, 0, color, QStringLiteral (""));
    return obj;
}

QTACObject* QTAChart::addStudyBollingerBands( const QString& name, int period,
                                              QRgb color )
{
    QTACObject *obj, *childobj;

    if (period < 1)
      return nullptr;

    obj = new QTACObject (ccore, QTACHART_OBJ_CURVE);
    obj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), BBANDSMIDDLE, 0, 0, color, QStringLiteral ("Color"));
    obj->setTitle (name);
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), BBANDSUPPER, 0, 0, color, QStringLiteral ("Color"));
    // childobj->setParamDialog (paramDialog->getPVector (), name);
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), BBANDSLOWER, 0, 0, color, QStringLiteral ("Color"));
    // childobj->setParamDialog (paramDialog->getPVector (), name);

    return obj;
}

QTACObject* QTAChart::addStudyADX( const QString& name, int period, int Weak,
                                   int Strong, int Verystrong, QRgb color )
{
    QTACObject *obj, *childobj;

    if (period < 1)
      return nullptr;

    obj = new QTACObject (ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"),
                        ADX, 0, 100, color, QStringLiteral (""));
    obj->setTitle (name);

    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"),
                             ADX, 0, 100, color, QStringLiteral ("Color"));

    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Weak);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Weak"),
                             DUMMY, 0, 100, color, QStringLiteral (""));

    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Strong);
    childobj->foreIntensity = 300;
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Strong"),
                             DUMMY, 0, 100, color, QStringLiteral (""));

    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Verystrong);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Very strong"),
                             DUMMY, 0, 100, color, QStringLiteral (""));
    childobj->foreIntensity = 300;

    return obj;
}

QTACObject* QTAChart::addStudyAroon( const QString& name, int period,
                                     int Highlevel, int Mediumlevel,
                                     int Lowlevel,
                                     QRgb UpColor, QRgb DownColor )
{
    QTACObject *obj, *childobj;

    if (period < 1)
      return nullptr;

    obj = new QTACObject (ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DUMMY, 0, 100, DownColor, QStringLiteral (""));
    obj->setTitle (name);
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), AROONDOWN, 0, 100, DownColor, "Down color");
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), AROONUP, 0, 100, UpColor, "Up color");
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Highlevel);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("High level"), DUMMY, 0, 100, UpColor, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Lowlevel);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Low level"), DUMMY, 0, 100, UpColor, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Mediumlevel);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Medium level"), DUMMY, 0, 100, UpColor, QStringLiteral (""));

    return obj;
}

QTACObject* QTAChart::addStudyCCI( const QString& name, int period,
                                   int Highlevel, int Lowlevel, QRgb color )
{
    QTACObject *obj, *childobj;

    if (period < 1)
      return nullptr;

    obj = new QTACObject (ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes(QTACHART_CLOSE, period, QStringLiteral ("Period"),
                       DUMMY, QREAL_MIN, QREAL_MAX, color, QStringLiteral (""));
    obj->setTitle (name);
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral ("Period"),
                            CCI, QREAL_MIN, QREAL_MAX,
                            color, QStringLiteral ("Color"));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Highlevel);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral("High level"),
                            DUMMY, QREAL_MIN, QREAL_MAX,
                            color, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Lowlevel);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral("Low level"),
                            DUMMY, QREAL_MIN, QREAL_MAX,
                            color, QStringLiteral (""));

    return obj;
}

QTACObject* QTAChart::addStudySTDDEV( const QString& name, int period,
                                      QRgb color )
{
    QTACObject *obj, *childobj;

    if (period < 1)
      return nullptr;

    obj = new QTACObject (ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes(QTACHART_CLOSE, period, QStringLiteral ("Period"),
                       DUMMY, 0, QREAL_MAX, color, QStringLiteral (""));
    obj->setTitle (name);
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral ("Period"),
                            STDDEV, 0, QREAL_MAX,
                            color, QStringLiteral ("Color"));

    return obj;
}

QTACObject* QTAChart::addStudyMomentum( const QString& name, int period,
                                        int Level, QRgb color )
{
    QTACObject *obj, *childobj;

    if (period < 1)
      return nullptr;

    obj = new QTACObject (ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes(QTACHART_CLOSE, period, QStringLiteral ("Period"),
                       MOMENTUM, QREAL_MIN, QREAL_MAX,
                       color, QStringLiteral(""));
    obj->setTitle (name);
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral ("Period"),
                            MOMENTUM, QREAL_MIN, QREAL_MAX,
                            color, QStringLiteral ("Color"));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Level);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral ("Level"),
                            DUMMY, 0, 0, color, QStringLiteral (""));

    return obj;
}

QTACObject* QTAChart::addStudyDMI( const QString& name, int period, int Weak,
                                   int Strong, int veryStrong, QRgb color )
{
    QTACObject *obj, *childobj;

    if (period < 1)
      return nullptr;

    obj = new QTACObject (ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes(QTACHART_CLOSE, period, QStringLiteral ("Period"),
                       DMX, 0, 100, color, QStringLiteral (""));
    obj->setTitle (name);
    childobj = new QTACObject (obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral ("Period"),
                            DMX, 0, 100, color, QStringLiteral ("Color"));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Weak);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral ("Weak"),
                            DUMMY, 0, 100, color, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, Strong);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral ("Strong"),
                            DUMMY, 0, 100, color, QStringLiteral (""));
    childobj = new QTACObject (obj, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, veryStrong);
    childobj->setAttributes(QTACHART_CLOSE, period,
                            QStringLiteral ("Very strong"),
                            DUMMY, 0, 100, color, QStringLiteral (""));

    return obj;
}

QTACObject* QTAChart::addStudyATR( const QString& name, int period, QRgb color )
{
    QTACObject *obj, *childobj;

    if (period < 1)
      return nullptr;

    obj = new QTACObject(ccore, QTACHART_OBJ_SUBCHART);
    obj->setAttributes(QTACHART_CLOSE, period, QStringLiteral("Period"),
                       ATR, 0, QREAL_MAX, color, QStringLiteral(""));
    obj->setTitle(name);
    childobj = new QTACObject(obj, QTACHART_OBJ_CURVE);
    childobj->setAttributes(QTACHART_CLOSE, period, QStringLiteral("Period"),
                            ATR, 0, QREAL_MAX, color, QStringLiteral("Color"));

    return obj;
}

void QTAChart::addMarkerLabel()
{
    ccore->drawScr->createTextObject(QTACHART_OBJ_LABEL);
}

void QTAChart::addMarkerTrailingText()
{
    ccore->drawScr->createTextObject(QTACHART_OBJ_TEXT);
}

void QTAChart::addMarkerHLine()
{
    ccore->drawScr->createTHVLineObject(QTACHART_OBJ_HLINE);
}

void QTAChart::addMarkerVLine()
{
    ccore->drawScr->createTHVLineObject(QTACHART_OBJ_VLINE);
}

void QTAChart::addMarkerTrendLine()
{
    ccore->drawScr->createTHVLineObject(QTACHART_OBJ_LINE);
}

void QTAChart::addMarkerFibonacci()
{
    ccore->drawScr->createTHVLineObject(QTACHART_OBJ_FIBO);
}
