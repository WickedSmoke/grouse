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

#ifndef UNIX_SIGNALS_H
#define UNIX_SIGNALS_H

#include <QtGlobal>

#ifndef Q_OS_WIN

#include <QSocketNotifier>

class UnixSignals : public QObject
{
  Q_OBJECT

public:
  UnixSignals (QObject *parent = 0);
  ~UnixSignals ();

public slots:
// Qt signal handlers.
  void handleSigHup();
  void handleSigTerm();
  void handleSigAbrt();

private:
  QSocketNotifier *snHup;
  QSocketNotifier *snTerm;
  QSocketNotifier *snAbrt;

signals:
  void sigTerm (); 		// SIGTERM signal
  void sigHup ();		// SIGHUP signal
  void sigAbrt ();		// SIGABRT signal
};


#endif // Q_OS_WIN

#endif // UNIX_SIGNALS_H
