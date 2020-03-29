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

#include <QDir>
#include <QMutex>
#include <QMessageBox>
#include <QThread>
#include "chartapp.h"
#include "defs.h"
#include "idb.h"


static SQLists _sqLists;
SQLists *ComboItems = &_sqLists;

QMutex *ResourceMutex = nullptr;

int NCORES;


ChartApp::ChartApp (int & argc, char **argv): QApplication(argc, argv)
{
  modmutex = new QMutex;
  ResourceMutex = new QMutex(QMutex::NonRecursive);

#ifdef Q_OS_MAC
  NCORES = 1;
#else
  NCORES = QThread::idealThreadCount();
  if (NCORES == -1)
    NCORES = 1;
#endif
}

ChartApp::~ChartApp ()
{
  delete modmutex;
  delete ResourceMutex;
}

bool ChartApp::openDatabase ()
{
  const char* openError;
  QString fn = QDir::homePath() % QDir::separator() %
      QStringLiteral(".config") % QDir::separator() % APPDIR %
      QDir::separator() % DBNAME;

  if( ! idb.openFile( fn, &openError ) )
  {
    showMessage( QString(openError).append(" Application quits.") );
    return false;
  }

  idb.initializeListQueries( _sqLists );
  return true;
}

void ChartApp::moduleLock (QObject *obj)
{
  modmutex->lock ();
  lockholder = obj;
}

void ChartApp::moduleUnlock (QObject *obj)
{
  Q_UNUSED (obj)

  modmutex->unlock ();
  lockholder = nullptr;
}


// show message box
void showMessage (const QString& message, QWidget* parent)
{
  QMessageBox::question(parent, QStringLiteral("Message"),
                        message % QStringLiteral("           "),
                        QMessageBox::Ok);
}

// show Ok/Cancel message box
bool showOkCancel (const QString& message, QWidget* parent)
{
  QMessageBox::StandardButton btn;
  btn = QMessageBox::question(parent, QStringLiteral("Question"),
                            message,
                            QMessageBox::Ok | QMessageBox::Cancel,
                            QMessageBox::Cancel);
  return( btn == QMessageBox::Ok );
}
