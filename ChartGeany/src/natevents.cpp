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
 
#include <QtGlobal>
#include <QApplication>

#ifdef Q_OS_LINUX
#include <xcb/xcb.h>
#endif

#ifdef Q_OS_WIN32
#include <SDKDDKVer.h>
#include <Windows.h>
#endif

#include "natevents.h"
#include "common.h"

bool
NativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *l)
{
  Q_UNUSED (l);
  Q_UNUSED (eventType);
  Q_UNUSED (message);
  /*
  #ifdef Q_OS_LINUX
    if (eventType == "xcb_generic_event_t")
    {
      xcb_generic_event_t* ev = static_cast <xcb_generic_event_t *> (message);
    }
  #endif // Q_OS_LINUX
  */

#ifdef Q_OS_WIN32

  if (eventType == "windows_generic_MSG")
  {
    MSG *msg = static_cast <MSG*> (message);
    if (msg && msg->message == WM_QUERYENDSESSION)
    {
      qApp->exit ();
      return true;
    }
    if (msg && msg->message == WM_ENDSESSION)
    {
      qApp->exit ();
      return true;
    }
  }
#endif // Q_OS_WIN32  

  return false;
}
