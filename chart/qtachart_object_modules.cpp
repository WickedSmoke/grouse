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

#include <csignal>

#include "chartapp.h"
#include "qtachart_core.h"
#include "cgscript.h"

#if defined (Q_OS_LINUX) || defined (Q_OS_MAC)
#define CHECK_SIGNAL \
int sig = get_last_signal (); \
switch (sig) \
{ \
  case 0: \
    break; \
  case SIGSEGV: \
    throw std::runtime_error("Segmentation fault"); \
    break; \
  case SIGFPE: \
    throw std::runtime_error("Floating-point exception"); \
    break; \
  case SIGILL: \
    throw std::runtime_error("Illegal instruction"); \
    break; \
  default: \
   throw std::runtime_error("Unhandled signal"); \
}
#else
#define CHECK_SIGNAL
#endif

// call module's Init() function
bool
QTACObject::moduleInit (void *data, int *objtype)
{
  int rslt;
  bool retval = true;

  if (modinit == NULL)
    return false;

  moduleLock ();
  try
  {
    rslt = modinit (CGScriptFunctionRegistry_ptr (), CGScriptFunctionRegistrySize,
                    data, objtype, static_cast <void *> (this), NULL);
    CHECK_SIGNAL
  }
  catch (std::exception& e)
  {
#ifdef DEBUG
    qDebug () << "Module '" % moduleName % "' reports " % QString (e.what ()) % " in Loop()" << endl;
#endif

    debugdialog->appendText ("Module '" % moduleName % "' reports " % QString (e.what ()) % " in Init()");
    if (QString (e.what ()) == "Floating-point exception")
      rslt = CGSERR_FPE;
    else
    if (QString (e.what ()) == "Segmentation fault")
      rslt = CGSERR_SEGV;
    else
    if (QString (e.what ()) == "Illegal instruction")
      rslt = CGSERR_ILL;
    else
      rslt = CGSERR_SIGNAL;
  }

  if (*objtype == -1)
  {
    debugdialog->appendText ("Module '" % moduleName % "' reports invalid object type in Init()");
  }

  if (rslt != CGSERR_OK || *objtype == -1)
  {
    retval = false;
    setForDelete ();
    enabled = false;
  }
  moduleUnlock ();

  return retval;
}

// call module's Loop() function
bool
QTACObject::moduleLoop ()
{
  int rslt;
  bool retval = true;

  if (!enabled)
    return false;

  if (modloop == NULL)
    return false;

  moduleLock ();
  try
  {
    rslt = modloop ();
    CHECK_SIGNAL
  }
  catch (std::exception& e)
  {
#ifdef DEBUG
    qDebug () << "Module '" % moduleName % "' reports " % QString (e.what ()) % " in Loop()" << endl;
#endif

    debugdialog->appendText ("Module '" % moduleName % "' reports " % QString (e.what ()) % " in Loop()");
    if (QString (e.what ()) == "Floating-point exception")
      rslt = CGSERR_FPE;
    else
    if (QString (e.what ()) == "Segmentation fault")
      rslt = CGSERR_SEGV;
    else
    if (QString (e.what ()) == "Illegal instruction")
      rslt = CGSERR_ILL;
    else
      rslt = CGSERR_SIGNAL;
  }

  if (rslt != CGSERR_OK)
  {
    enabled = false;
    setForDelete ();
    foreach (QTACObject *obj, Object)
    {
      obj->enabled = false;
      obj->setForDelete ();
    }

    debugdialog->appendText ("== Module '" % moduleName % "' stopped by " % APPNAME);
    retval = false;
  }
  moduleUnlock ();

  return retval;
}

// call module's Event() function
bool
QTACObject::moduleEvent (int ev)
{
  int rslt = CGSERR_OK;
  bool retval = true;

  if (!enabled || deleteit)
    return false;

  if (parentObject != NULL)
    if (!parentObject->enabled || parentObject->deleteit)
      return false;

  if (modevent == NULL)
    return false;

  moduleLock ();
  try
  {
    modevent (ev);
    CHECK_SIGNAL
  }
  catch (std::exception& e)
  {
#ifdef DEBUG
    qDebug () << "Module '" % moduleName % "' reports " % QString (e.what ()) % " in Event()" << endl;
#endif

    debugdialog->appendText ("Module '" % moduleName % "' reports " % QString (e.what ()) % " in Event()");
    if (QString (e.what ()) == "Floating-point exception")
      rslt = CGSERR_FPE;
    else
    if (QString (e.what ()) == "Segmentation fault")
      rslt = CGSERR_SEGV;
    else
    if (QString (e.what ()) == "Illegal instruction")
      rslt = CGSERR_ILL;
    else
      rslt = CGSERR_SIGNAL;
  }

  if (rslt != CGSERR_OK)
  {
    enabled = false;
    setForDelete ();
    foreach (QTACObject *obj, Object)
    {
      obj->enabled = false;
      obj->setForDelete ();
    }

    debugdialog->appendText ("== Module '" % moduleName % "' stopped by " % APPNAME);
    retval = false;
  }
  moduleUnlock ();

  return retval;
}
