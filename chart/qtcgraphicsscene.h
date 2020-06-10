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

#ifndef QTCGRAPHICSSCENE_H
#define QTCGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QMutex>

class QTAChartCore;

// QTCGraphicsScene
class QTCGraphicsScene : public QGraphicsScene
{
  Q_OBJECT

public:
  explicit QTCGraphicsScene (QObject *parent = nullptr);

  void setDragOffset(qreal dx, qreal dy)
  {
      dragOffsetX = dx;
      dragOffsetY = dy;
  }

  void
  qtcAddItem (QGraphicsItem *item)
  {
    protectScene.lock ();
    addItem (item);
    protectScene.unlock ();
  }

  QGraphicsProxyWidget *
  qtcAddWidget (QWidget * widget, Qt::WindowFlags wFlags = 0)
  {
    QGraphicsProxyWidget *wid;
    protectScene.lock ();
    wid = addWidget (widget, wFlags);
    protectScene.unlock ();
    return wid;
  }

  QGraphicsLineItem *
  qtcAddLine (qreal x1, qreal y1, qreal x2, qreal y2, const QPen & pen = QPen())
  {
    QGraphicsLineItem *line;
    protectScene.lock ();
    line = addLine (x1, y1, x2, y2, pen);
    protectScene.unlock ();
    return line;
  }

  QTAChartCore* core;

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
  void wheelEvent(QGraphicsSceneWheelEvent *);

private:
  void dragObjectCtrl(QGraphicsSceneMouseEvent *);
  void dragText(QGraphicsSceneMouseEvent *);
  void dragHVLine(QGraphicsSceneMouseEvent *);
  void updatePos(QGraphicsSceneMouseEvent *);

  QMutex protectScene;
  qreal dragOffsetX;
  qreal dragOffsetY;
  qreal padx;       // pad over x
  qreal pady;       // pad over y
  qint32 phase;     // 0, 1, 2, 3....
};

#endif // QTCGRAPHICSSCENE_H
