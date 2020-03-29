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

#ifndef CHARTAPP_H
#define CHARTAPP_H

#include <QApplication>
#include "idb.h"

class QMutex;

class ChartApp : public QApplication
{
  Q_OBJECT

public:
  ChartApp(int& argc, char ** argv) ;
  ~ChartApp();

  bool openDatabase ();
  void moduleLock (QObject *obj);
  void moduleUnlock (QObject *obj);

private:
  InstrumentDatabase idb;
  QObject *lockholder;  // the object that holds the lock
  QMutex *modmutex;     // mutex for modules
};

extern SQLists *ComboItems;     // QStringLists used as combo box items and more
extern QMutex *ResourceMutex;   // mutex to protect shared resources
extern int NCORES;              // number of active cores

// show a message box
extern void showMessage (const QString& message, QWidget* parent = nullptr);

// show an Ok/Cancel question box
extern bool showOkCancel (const QString& message, QWidget* parent = nullptr);


#endif // CHARTAPP_H
