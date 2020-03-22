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
 
#include <QPen>
#include "qtcgraphicsscene.h"
#include "qtcgraphicsitem.h"

// constructor
QTCGraphicsItem::QTCGraphicsItem ( int reqtype, QGraphicsItem * parent)
{
  if (parent != NULL)
    setParentItem (parent);
  QTCGraphicsItem_constructor_common (reqtype);

}

QTCGraphicsItem::QTCGraphicsItem ( int reqtype )
{
  QTCGraphicsItem_constructor_common (reqtype);
}

void
QTCGraphicsItem::QTCGraphicsItem_constructor_common (int reqtype)
{
  type = reqtype;
  Scene = NULL;
  initialized = false;
  switch (type)
  {
  case LineItem:
    lineItem = new QGraphicsLineItem ();
    break;
  case RectItem:
    rectItem = new QGraphicsRectItem ();
    break;
  case EllipseItem:
    ellipseItem = new QTCGraphicsEllipseItem ();
    break;
  }
}

void
QTCGraphicsItem::init (QTCGraphicsScene *scene)
{
  QGraphicsItem *item;
  Scene = scene;

  if (initialized)
    return;

  switch (type)
  {
  case LineItem:
    scene->qtcAddItem (lineItem);
    break;
  case RectItem:
    scene->qtcAddItem (rectItem);
    break;
  case EllipseItem:
    scene->qtcAddItem (ellipseItem);
    break;
  }

  foreach (item, childItems ())
  {
    switch (type)
    {
    case LineItem:
      item->setParentItem (lineItem);
      break;
    case RectItem:
      item->setParentItem (rectItem);
      break;
    case EllipseItem:
      item->setParentItem (ellipseItem);
      break;
    }
  }

  initialized = true;
}

// destructor
QTCGraphicsItem::~QTCGraphicsItem ()
{
  if (Scene != NULL)
  {
    switch (type)
    {
    case LineItem:
      Scene->removeItem (lineItem);
      break;
    case RectItem:
      Scene->removeItem (rectItem);
      break;
    case EllipseItem:
      Scene->removeItem (ellipseItem);
      break;
    }
  }

  switch (type)
  {
  case LineItem:
    delete lineItem;
    break;
  case RectItem:
    delete rectItem;
    break;
  case EllipseItem:
    delete ellipseItem;
    break;
  }
}

/// rect
// rect
QRectF
QTCGraphicsItem::rect () const
{
  if (type == RectItem)
    return rectItem->rect () ;
  else if (type == EllipseItem)
    return ellipseItem->rect () ;

  return QRectF (0,0,0,0);
}

// setRect
void
QTCGraphicsItem::setRect ( const QRectF & rectangle )
{
  if (type == RectItem)
    rectItem->setRect (rectangle);
  else if (type == EllipseItem)
    ellipseItem->setRect (rectangle);
}

void
QTCGraphicsItem::setRect ( qreal x, qreal y, qreal width, qreal height )
{
  if (type == RectItem)
    rectItem->setRect (x, y, width, height);
  else if (type == EllipseItem)
    ellipseItem->setRect (x, y, width, height);
}

// brush
QBrush
QTCGraphicsItem::brush () const
{
  if (type == RectItem)
    rectItem->brush ();
  else if (type == EllipseItem)
    ellipseItem->brush ();

  return QBrush (Qt::black);
}

// setBrush
void
QTCGraphicsItem::setBrush ( const QBrush & brush )
{
  if (type == RectItem)
    rectItem->setBrush (brush);
  else if (type == EllipseItem)
    ellipseItem->setBrush (brush);
}

/// general
// boundingRect
QRectF
QTCGraphicsItem::boundingRect () const
{
  switch (type)
  {
  case LineItem:
    return lineItem->boundingRect () ;
  case RectItem:
    return rectItem->boundingRect () ;
  case EllipseItem:
    return ellipseItem->boundingRect () ;
  default:
    return QRectF (0,0,0,0);
  }
}

// paint
void
QTCGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget)
{
  Q_UNUSED (painter)
  Q_UNUSED (style)
  Q_UNUSED (widget)
  return;
}

// pen
QPen
QTCGraphicsItem::pen () const
{
  switch (type)
  {
  case LineItem:
    return lineItem->pen () ;
  case RectItem:
    return rectItem->pen () ;
  case EllipseItem:
    return ellipseItem->pen () ;
  default:
    return QPen (Qt::black);
  }
}

// setPen
void
QTCGraphicsItem::setPen ( const QPen & pen )
{
  switch (type)
  {
  case LineItem:
    lineItem->setPen (pen) ;
    break;
  case RectItem:
    rectItem->setPen (pen) ;
    break;
  case EllipseItem:
    ellipseItem->setPen (pen) ;
    break;
  }
}

// zValue
qreal
QTCGraphicsItem::zValue () const
{
  switch (type)
  {
  case LineItem:
    return lineItem->zValue ();
  case RectItem:
    return rectItem->zValue ();
  case EllipseItem:
    return ellipseItem->zValue ();
  default:
    return 0;
  }
}

// setZValue
void
QTCGraphicsItem::setZValue ( qreal z )
{
  switch (type)
  {
  case LineItem:
    lineItem->setZValue (z);
    break;
  case RectItem:
    rectItem->setZValue (z);
    break;
  case EllipseItem:
    ellipseItem->setZValue (z);
    break;
  }
}

// opacity
qreal
QTCGraphicsItem::opacity () const
{
  switch (type)
  {
  case LineItem:
    return lineItem->opacity ();
  case RectItem:
    return rectItem->opacity ();
  case EllipseItem:
    return ellipseItem->opacity ();
  default:
    return 0;
  }
}

// setOpacity
void
QTCGraphicsItem::setOpacity ( qreal z )
{
  switch (type)
  {
  case LineItem:
    lineItem->setOpacity (z);
    break;
  case RectItem:
    rectItem->setOpacity (z);
    break;
  case EllipseItem:
    ellipseItem->setOpacity (z);
    break;
  }
}

// pos
QPointF
QTCGraphicsItem::pos () const
{
  switch (type)
  {
  case LineItem:
    return lineItem->pos ();
  case RectItem:
    return rectItem->pos ();
  case EllipseItem:
    return ellipseItem->pos ();
  default:
    return QPointF (0,0);
  }
}

// setPos
void
QTCGraphicsItem::setPos (const QPointF & pos)
{
  switch (type)
  {
  case LineItem:
    lineItem->setPos (pos);
    break;
  case RectItem:
    rectItem->setPos (pos);
    break;
  case EllipseItem:
    ellipseItem->setPos (pos);
    break;
  }
}

void
QTCGraphicsItem::setPos (qreal x, qreal y)
{
  switch (type)
  {
  case LineItem:
    lineItem->setPos (x, y);
    break;
  case RectItem:
    rectItem->setPos (x, y);
    break;
  case EllipseItem:
    ellipseItem->setPos (x, y);
    break;
  }
}

// isVisible
bool
QTCGraphicsItem::isVisible () const
{
  switch (type)
  {
  case LineItem:
    return lineItem->isVisible ();
  case RectItem:
    return rectItem->isVisible ();
  case EllipseItem:
    return ellipseItem->isVisible ();
  default:
    return true;
  }
}

// setVisible
void
QTCGraphicsItem::setVisible (bool visible)
{
  switch (type)
  {
  case LineItem:
    lineItem->setVisible (visible);
    break;
  case RectItem:
    rectItem->setVisible (visible);
    break;
  case EllipseItem:
    ellipseItem->setVisible (visible);
    break;
  }
}

// setSize
void
QTCGraphicsItem::setSize (qreal x, qreal y, qreal width, qreal height)
{
  switch (type)
  {
  case LineItem:
    lineItem->setLine (x, y, width, height);
    break;
  case RectItem:
    rectItem->setRect (x, y, width, height);
    break;
  case EllipseItem:
    ellipseItem->setRect (x, y, width, height);
    break;
  }
}

// children
QList<QGraphicsItem *>
QTCGraphicsItem::children () const
{
  if (type == LineItem)
    return lineItem->childItems ();

  if (type == RectItem)
    return rectItem->childItems ();

  return ellipseItem->childItems ();
}
