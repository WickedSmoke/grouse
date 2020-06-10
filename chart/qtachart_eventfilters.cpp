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

#include <QApplication>
#include <QTimer>
#include <QTabWidget>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include "qtachart_core.h"


//----------------------------------------------------------------------------
// QTCGraphicsScene

QTCGraphicsScene::QTCGraphicsScene(QObject * parent) : QGraphicsScene(parent)
{
  // NOTE: core must be set manually by scene owner.

  dragOffsetX = dragOffsetY = 0.0;
  padx = -1;
  pady = -1;
  phase = 0;
}

// control drag and add an object on the chart
void QTCGraphicsScene::dragObjectCtrl(QGraphicsSceneMouseEvent *event)
{
  if (core->dragged_obj_type == QTACHART_OBJ_LABEL ||
      core->dragged_obj_type == QTACHART_OBJ_TEXT)
  {
    dragText(event);
    return;
  }

  if (core->dragged_obj_type == QTACHART_OBJ_HLINE ||
      core->dragged_obj_type == QTACHART_OBJ_VLINE ||
      core->dragged_obj_type == QTACHART_OBJ_LINE ||
      core->dragged_obj_type == QTACHART_OBJ_FIBO)
  {
    dragHVLine(event);
    return;
  }
}

// drag and add a line
void QTCGraphicsScene::dragHVLine(QGraphicsSceneMouseEvent *event)
{
    QPointF point;
    QRectF rect;
    qreal x, y;
    int evtype = event->type ();

    point = event->scenePos ();
    x = point.x ();
    y = point.y ();

    if (core->dragged_obj_type == QTACHART_OBJ_HLINE)
    {
      if (y < core->charttopmost || y > core->chartbottomost)
        return;
      core->hvline->setLine(core->chartleftmost, y + dragOffsetY,
                            core->chartrightmost, y + dragOffsetY);
      core->setBottomText (x);
      if (evtype == QEvent::GraphicsSceneMouseRelease)
      {
        core->addHLine(core->hvline, core->priceOnY(y + dragOffsetY));
        core->object_drag = false;
        appRestoreOverrideCursor (core->chart);
      }
      else
      {
        y += dragOffsetY;   // For Press & Move snap cursor to line.
      }
      core->setRullerCursor (y);
    }
    else if (core->dragged_obj_type == QTACHART_OBJ_VLINE)
    {
      if (x < core->chartleftmost || x > core->chartrightmost)
        return;
      core->hvline->setLine(x + dragOffsetX, core->charttopmost,
                            x + dragOffsetX, core->chartbottomost);
      core->setRullerCursor (y);
      core->setBottomText (x);
      if (event->type () == QEvent::GraphicsSceneMouseRelease)
      {
        core->addVLine (core->hvline);
        core->object_drag = false;
        appRestoreOverrideCursor (core->chart);
      }
    }
    else if (core->dragged_obj_type == QTACHART_OBJ_LINE)
    {
      if (x < core->chartleftmost || x > core->chartrightmost)
        return;

      if (y < core->charttopmost || y > core->chartbottomost)
        return;

      if (core->hvline->line ().x1 () == core->hvline->line ().x2 () &&
          core->hvline->line ().y1 () == core->hvline->line ().y2 () &&
          core->hvline->line ().y2 () == 0)
        phase = 0;
      else
        phase = 1;

      core->setRullerCursor (y);
      core->setBottomText (x);

      if (phase == 1)
      {
        qreal x1, y1;

        x1 = core->hvline->line().x1 ();
        y1 = core->hvline->line().y1 ();

        core->hvline->setLine(x1, y1, x + dragOffsetX, y + dragOffsetY);
      }

      if (evtype == QEvent::GraphicsSceneMousePress)
      {
        if (phase == 0)
        {
          core->hvline->setLine (x, y, x, y);
          phase = 1;
        }
      }

      if (evtype == QEvent::GraphicsSceneMouseRelease)
      {
        if (phase == 1)
        {
          core->addTLine (core->hvline);
          core->object_drag = false;
          appRestoreOverrideCursor (core->chart);
          phase = 0;
        }
      }
    }
    else if (core->dragged_obj_type == QTACHART_OBJ_FIBO)
    {
      if (x < core->chartleftmost || x > core->chartrightmost)
        return;

      if (y < core->charttopmost || y > core->chartbottomost)
        return;

      if (core->hvline->line ().x1 () == core->hvline->line ().x2 () &&
          core->hvline->line ().y1 () == core->hvline->line ().y2 () &&
          core->hvline->line ().y2 () == 0)
        phase = 0;
      else
        phase = 1;

      core->setRullerCursor (y);
      core->setBottomText (x);

      if (phase == 1)
      {
        qreal y1;

        y1 = core->hvline->line().y1 ();
        core->hvline->setLine(x + dragOffsetX, y1,
                              x + dragOffsetX, y + dragOffsetY);
      }

      if (evtype == QEvent::GraphicsSceneMousePress)
      {
        if (phase == 0)
        {
          core->hvline->setLine (x, y, x, y);
          phase = 1;
        }
      }

      if (evtype == QEvent::GraphicsSceneMouseRelease)
      {
        if (phase == 1)
        {
          core->addFibo (core->hvline);
          core->object_drag = false;
          appRestoreOverrideCursor (core->chart);
          phase = 0;
        }
      }
    }
}


// drag and add a Label/Text object
void QTCGraphicsScene::dragText(QGraphicsSceneMouseEvent *event)
{
    QPointF point;
    QRectF rect;
    qreal x, y;

    point = event->scenePos ();
    x = point.x ();
    y = point.y ();
    rect = core->textitem->boundingRect ();

    if (core->textitem->x() <= 0)
      padx = rect.width () / 2;

    if (padx == -1)
      padx = x - core->textitem->x();

    x -= padx;

    if (x < core->chartleftmost)
    {
      x = core->chartleftmost;
    }
    else
    {
      if (x + rect.width () > core->chartrightmost)
        x = core->chartrightmost - rect.width ();
    }

    if (core->textitem->y() <= 0)
      pady = rect.height () / 2;

    if (pady == -1)
      pady = y - core->textitem->y();

    y -= pady;

    if (y < core->charttopmost)
      y = core->charttopmost;
    else if (y + rect.height () > core->chartbottomost)
      y = core->chartbottomost - rect.height ();

    core->textitem->setPos (x, y);
    core->setBottomText (x);
    if (event->type () == QEvent::GraphicsSceneMouseRelease)
    {
      if (core->dragged_obj_type == QTACHART_OBJ_LABEL)
        core->addLabel (core->textitem, x, y);
      else if (core->dragged_obj_type == QTACHART_OBJ_TEXT)
        core->addText (core->textitem, x, y);

      core->object_drag = false;
      appRestoreOverrideCursor (core->chart);
      padx = pady = -1;
    }
}

void QTCGraphicsScene::updatePos(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = event->scenePos();
    core->setBottomText(pos.x());
    core->setRullerCursor(pos.y());
    core->last_x = pos.x();
}

void QTCGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    if( event->isAccepted() )
        return;
    if( core->object_drag )
    {
        dragObjectCtrl(event);
        return;
    }
    if (! core->events_enabled)
        return;

    if (core->drag == true)
    {
      const int sense = 4;
      QPointF pos = event->scenePos();
      qreal diff = core->initial_mouse_x - pos.x();
      qreal adiff = qAbs(diff);

      // pointer moved right
      if (adiff >= sense)
      {
        int bars = (int) (adiff / core->framewidth);
        if (bars > 0)
        {
          if (diff < 0)
            core->chartBackward(bars);
          else      // pointer moved left
            core->chartForward(bars);
          core->initial_mouse_x = pos.x();
        }
      }
    }
    updatePos(event);
}

void QTCGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if( event->isAccepted() )
        return;
    if( core->object_drag )
    {
        dragObjectCtrl(event);
        return;
    }
    if (! core->events_enabled)
        return;

    if (event->button() == Qt::LeftButton && core->drag == false )
    {
      QPointF pos = event->scenePos();
      if (pos.y() > core->title_height && pos.x() < core->width)
      {
        core->drag = true;
        core->initial_mouse_x = pos.x();
        appSetOverrideCursor(core->chart, QCursor(Qt::ClosedHandCursor));
      }
    }
    updatePos(event);
}

void QTCGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if( event->isAccepted() )
        return;
    if( core->object_drag )
    {
        dragObjectCtrl(event);
        return;
    }
    if (! core->events_enabled)
        return;

    if (core->drag == true)
    {
      core->drag = false;
      appRestoreOverrideCursor(core->chart);
    }
    updatePos(event);
}

void QTCGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
  if (! core->events_enabled)
    return;

#ifdef GUI_DESKTOP
  // Zoom
  if (event->delta() > 0)
  {
    if (core->framewidth < 25)
    {
      core->framewidth++;
      core->draw();
    }
  }
  else
  {
    if (core->framewidth > 3)
    {
      core->framewidth--;
      core->draw();
    }
  }
#else
  if (event->delta() > 0)
    core->chartForward(1);      // right
  else
    core->chartBackward(1);     // left

  core->setRullerCursor(core->ruller_cursor_y);
  core->setBottomText(core->last_x);
#endif
}

//----------------------------------------------------------------------------
// QTAChartEventFilter

QTAChartEventFilter::QTAChartEventFilter (QObject * parent)
{
  if (parent != NULL)
    setParent (parent);
}

static void _chartKeyFocus( QTAChartCore* core )
{
  appRestoreOverrideCursor (core->chart);
  core->setRullerCursor (core->ruller_cursor_y);
  core->setBottomText (core->last_x);
}

bool
QTAChartEventFilter::eventFilter (QObject * watched, QEvent * event)
{
  if (watched->inherits ("QGraphicsView"))
  {
    QGraphicsView *View = qobject_cast <QGraphicsView *> (watched);
    QTAChart *chart = qobject_cast <QTAChart *> (View->parentWidget ());
    QTAChartCore *core = static_cast <QTAChartCore *> (getData (chart));
    int evtype;

    if (core == NULL)
      return false;

    if (!core->events_enabled)
      return false;

    evtype = event->type ();
    if (evtype == QEvent::Show)
    {
      core->Scene.setFocus (Qt::OtherFocusReason);
    }
    else if(evtype == QEvent::FocusIn)
    {
      _chartKeyFocus(core);
    }
    else if (evtype == QEvent::KeyPress)
    {
      QKeyEvent *keyEvent = static_cast < QKeyEvent * > (event);
      Qt::Key keyPressed = static_cast <Qt::Key> (keyEvent->key ());

      _chartKeyFocus(core);

      // tab
      if (keyPressed == Qt::Key_Tab)
      {
        QTabWidget *tabWidget;
        int idx;

        tabWidget = qobject_cast <QTabWidget *> (chart->parentWidget ()->parentWidget ());
        idx = tabWidget->currentIndex ();
        idx ++;

        if (idx == tabWidget->count ())
          return true;

        tabWidget->setCurrentIndex (idx);
        tabWidget->update ();
        return true;
      }
      // backtab
      else if (keyPressed == Qt::Key_Backtab)
      {
        QTabWidget *tabWidget;
        int idx;

        tabWidget = qobject_cast <QTabWidget *> (chart->parentWidget ()->parentWidget ());
        idx = tabWidget->currentIndex ();
        if (idx == 0)
          return true;

        idx --;
        tabWidget->setCurrentIndex (idx);
        tabWidget->update ();
        return true;
      }
      // left
      else if (keyPressed == Qt::Key_Left)
      {
        core->chartBackward (1);
        return true;
      }
      // right
      else if (keyPressed == Qt::Key_Right)
      {
        core->chartForward (1);
        return true;
      }
      // home
      else if (keyPressed == Qt::Key_Home)
      {
        core->chartBegin ();
        return true;
      }
      // end
      else if (keyPressed == Qt::Key_End)
      {
        core->chartEnd ();
        return true;
      }
      // pgup
      else if (keyPressed == Qt::Key_PageUp)
      {
        core->chartPagePrevious ();
        return true;
      }
      // pgdn
      else if (keyPressed == Qt::Key_PageDown)
      {
        core->chartPageNext ();
        return true;
      }
      // esc
      else if (keyPressed == Qt::Key_Escape)
      {
        if (core->object_drag == true)
        {
          core->object_drag = false;
          if (core->textitem != NULL)
          {
            delete core->textitem;
            core->textitem = NULL;
          }

          if (core->hvline != NULL)
          {
            delete core->hvline;
            core->hvline = NULL;
          }

          appRestoreOverrideCursor (core->chart);
        }
        return true;
      }
    }
  }

  return QObject::eventFilter (watched, event);
}

//----------------------------------------------------------------------------
// QTACObjectEventFilter

QTACObjectEventFilter::QTACObjectEventFilter (QObject * parent)
{
  if (parent == NULL)
    return;

  setParent (parent);
  core = qobject_cast <QTACObject *> (parent->parent())->chartdata;
}

bool
QTACObjectEventFilter::eventFilter (QObject * watched, QEvent * event)
{
  QTACObject *object = qobject_cast<QTACObject *>(watched->parent());
  int evtype;

  if (object->deleteit || core->object_drag)
    return QObject::eventFilter (object, event);

  evtype = event->type ();

  if (evtype == QEvent::GraphicsSceneHoverEnter)
  {
    if (object->type == QTACHART_OBJ_HLINE ||
        object->type == QTACHART_OBJ_VLINE)
    {
      object->title->setOpacity (0.7);
      object->hvline->setOpacity (0.7);
      core->dragged_obj_type = object->type;
      core->events_enabled = false;
    }
    else if (object->type == QTACHART_OBJ_LABEL ||
             object->type == QTACHART_OBJ_TEXT)
    {
      object->text->setOpacity (0.7);
      core->textitem = object->text;
      core->dragged_obj_type = object->type;
      core->events_enabled = false;
    }
    else if (object->type == QTACHART_OBJ_LINE)
    {
      if (watched == object->Edge[0]->pricetxt)
      {
        LineEdge edge;
        qreal x1, x2, y1, y2;
        edge = *object->Edge[1];
        *object->Edge[1]= *object->Edge[0];
        *object->Edge[0] = edge;
        object->Edge[0]->sequence = 1;
        object->Edge[1]->sequence = 2;
        x1 = object->hvline->line ().x2 ();
        x2 = object->hvline->line ().x1 ();
        y1 = object->hvline->line ().y2 ();
        y2 = object->hvline->line ().y1 ();
        object->hvline->setLine (x1, y1, x2, y2);
        core->hvline = object->hvline;
      }

      object->title->setOpacity (0.7);
      object->hvline->setOpacity (0.7);
      object->Edge[0]->pricetxt->setOpacity (0.7);
      object->Edge[1]->pricetxt->setOpacity (0.7);
      core->dragged_obj_type = object->type;
      core->events_enabled = false;
    }
    else if (object->type == QTACHART_OBJ_FIBO)
    {
      if (watched == object->Edge[0]->pricetxt)
      {
        LineEdge edge;
        qreal x1, x2, y1, y2;
        edge = *object->Edge[1];
        *object->Edge[1]= *object->Edge[0];
        *object->Edge[0] = edge;
        object->Edge[0]->sequence = 1;
        object->Edge[1]->sequence = 2;
        x1 = object->hvline->line ().x2 ();
        x2 = object->hvline->line ().x1 ();
        y1 = object->hvline->line ().y2 ();
        y2 = object->hvline->line ().y1 ();
        object->hvline->setLine (x1, y1, x2, y2);
        core->hvline = object->hvline;
      }

      object->title->setOpacity (0.7);
      object->hvline->setOpacity (0.7);
      object->Edge[0]->pricetxt->setOpacity (0.7);
      object->Edge[1]->pricetxt->setOpacity (0.7);
      core->dragged_obj_type = object->type;
      core->events_enabled = false;
      for (qint32 counter = 0; counter < object->FiboLevelPrc.size (); counter ++)
      {
        object->FiboLevelLbl[counter].setOpacity (0.7);
        object->FiboLevelPrcLbl[counter].setOpacity (0.7);
        object->FiboLevel[counter]->setOpacity (0.7);
      }
    }
    else if (object->type == QTACHART_OBJ_CURVE || object->type == QTACHART_OBJ_DOT)
    {
      QTACObject *child;
      object->title->setOpacity (0.7);
      core->object_drag = false;
      core->events_enabled = true;

      for (qint32 counter = 0; counter < object->ITEMSsize; counter ++)
        object->ITEMS[counter]->setOpacity (0.7);

      foreach (child, object->children)
      {
        for (qint32 counter = 0; counter < child->ITEMSsize; counter ++)
          child->ITEMS[counter]->setOpacity (0.7);
      }
    }
  }
  else if (evtype == QEvent::GraphicsSceneHoverLeave)
  {
    if (object->type == QTACHART_OBJ_HLINE ||
        object->type == QTACHART_OBJ_VLINE)
    {
      object->title->setOpacity (1.0);
      object->hvline->setOpacity (1.0);
      core->object_drag = false;
      core->events_enabled = true;
    }
    else if (object->type == QTACHART_OBJ_LABEL ||
             object->type == QTACHART_OBJ_TEXT)
    {
      object->text->setOpacity (1.0);
      core->object_drag = false;
      core->events_enabled = true;
    }
    else if (object->type == QTACHART_OBJ_LINE)
    {
      object->title->setOpacity (1.0);
      object->hvline->setOpacity (1.0);
      object->Edge[0]->pricetxt->setOpacity (1.0);
      object->Edge[1]->pricetxt->setOpacity (1.0);
      core->object_drag = false;
      core->events_enabled = true;
    }
    else if (object->type == QTACHART_OBJ_FIBO)
    {
      object->title->setOpacity (1.0);
      object->hvline->setOpacity (1.0);
      object->Edge[0]->pricetxt->setOpacity (1.0);
      object->Edge[1]->pricetxt->setOpacity (1.0);
      core->object_drag = false;
      core->events_enabled = true;

      for (qint32 counter = 0; counter < object->FiboLevelPrc.size (); counter ++)
      {
        object->FiboLevelLbl[counter].setOpacity (1.0);
        object->FiboLevelPrcLbl[counter].setOpacity (1.0);
        object->FiboLevel[counter]->setOpacity (1.0);
      }
    }

    if (object->type == QTACHART_OBJ_CURVE || object->type == QTACHART_OBJ_DOT)
    {
      QTACObject *child;
      object->title->setOpacity (1.0);
      core->object_drag = false;
      core->events_enabled = true;

      for (qint32 counter = 0; counter < object->ITEMSsize; counter ++)
        object->ITEMS[counter]->setOpacity (1.0);

      foreach (child, object->children)
      {
        for (qint32 counter = 0; counter < child->ITEMSsize; counter ++)
          child->ITEMS[counter]->setOpacity (1.0);
      }
    }
    appRestoreOverrideCursor (core->chart);
  }
  else if (evtype == QEvent::GraphicsSceneMouseDoubleClick ||
           evtype == QEvent::GraphicsSceneContextMenu)
  {
    appRestoreOverrideCursor (core->chart);
    if (object->type == QTACHART_OBJ_CURVE ||
        object->type == QTACHART_OBJ_DOT)
    {
      bool modrslt;
      core->events_enabled = false;
      modrslt = object->modifyIndicator ();
      if (modrslt == false)
        object->setForDelete ();
      core->events_enabled = true;
    }
    else if (object->type == QTACHART_OBJ_HLINE ||
             object->type == QTACHART_OBJ_VLINE ||
             object->type == QTACHART_OBJ_LINE ||
             object->type == QTACHART_OBJ_FIBO)
    {
      if (object->modifyLine() == false)
      {
        object->setForDelete ();
        core->events_enabled = true;
      }
    }
    else if (object->type == QTACHART_OBJ_LABEL ||
             object->type == QTACHART_OBJ_TEXT)
    {
      if (object->modifyText() == false)
      {
        object->setForDelete ();
        core->events_enabled = true;
      }
    }

    if (object->deleteit == false)
      core->draw ();

    core->object_drag = false;
    core->drag = false;
    return true;
  }
  else if (evtype == QEvent::GraphicsSceneMousePress)
  {
    QGraphicsSceneMouseEvent *qMouse;
    QPointF point;

    qMouse = static_cast <QGraphicsSceneMouseEvent *> (event);
    point = qMouse->scenePos ();

    if (qMouse->button () == Qt::LeftButton)
    {
      if (object->type == QTACHART_OBJ_HLINE ||
          object->type == QTACHART_OBJ_VLINE ||
          object->type == QTACHART_OBJ_LINE ||
          object->type == QTACHART_OBJ_FIBO)
      {
        if (object->title->opacity () < 1)
        {
          core->object_drag = true;
          core->drag = false;
          core->hvline = object->hvline;

          QLineF line( core->hvline->line() );
          core->scene->setDragOffset(line.x2() - point.x(),
                                     line.y2() - point.y());

          appSetOverrideCursor (core->chart, QCursor (Qt::PointingHandCursor));
        }
      }
      else if (object->type == QTACHART_OBJ_LABEL ||
               object->type == QTACHART_OBJ_TEXT)
      {
        if (object->text->opacity () < 1)
        {
          core->object_drag = true;
          core->drag = false;
          appSetOverrideCursor (core->chart, QCursor (Qt::PointingHandCursor));
        }
      }
    }
  }
  else if (evtype == QEvent::KeyPress)
  {
    if (object->type == QTACHART_OBJ_HLINE)
    {
      QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);
      qreal dy = 0.0;
      if( keyEvent->key() == Qt::Key_Up )
        dy = 0.1;
      else if( keyEvent->key() == Qt::Key_Down )
        dy = -0.1;
      if( dy != 0.0 )
      {
        object->setHLine(object->hvline, object->price + dy);
        return true;
      }
    }
  }

  return QObject::eventFilter(object, event);
}
