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

#ifndef QTACHART_EF_H
#define QTACHART_EF_H

#include <QObject>

class QTAChartCore;

// custom event filter for scene
class QTAChartSceneEventFilter:public QObject
{
Q_OBJECT public:
  explicit QTAChartSceneEventFilter (QObject * parent); // constructor
  ~QTAChartSceneEventFilter (void);  // destructor

private:
  QTAChartCore *core;
  qreal padx;		// pad over x
  qreal pady;		// pad over y
  qint32 phase;     // 0, 1, 2, 3....

  // control drag and add an object on the chart
  void dragObjectCtrl (QObject *core, QEvent *event);

  // drag and add a Label/Text object
  void dragText (QObject *core, QEvent *event);

  // drag and add a horizontal line
  void dragHVLine (QObject *coreptr, QEvent *event);

protected:
  bool eventFilter (QObject * object, QEvent * event);

};

// custom event filter for chart
class QTAChartEventFilter:public QObject
{
Q_OBJECT public:
  explicit QTAChartEventFilter (QObject * parent); // constructor
  ~QTAChartEventFilter (void); // destructor

protected:
  bool eventFilter (QObject * watched, QEvent * event);
};

// custom event filter for object
class QTACObjectEventFilter:public QObject
{
Q_OBJECT public:
  explicit QTACObjectEventFilter (QObject * parent); // constructor
  ~QTACObjectEventFilter (void); // destructor

private:
  QTAChartCore *core;

protected:
  bool eventFilter (QObject * watched, QEvent * event);
};


#endif // QTACHART_EF_H
