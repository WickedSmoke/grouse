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

#ifndef Q_OS_WIN
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <csignal>
#include <chartapp.h>

#include "unix_signals.h"

// setup unix signal handlers
static int setup_unix_signal_handlers ();

// Unix signal handlers.
static void hupSignalHandler (int unused);
static void termSignalHandler (int unused);
static void abrtSignalHandler (int unused);

// Socket file descriptors
static int sighupFd[2];
static int sigtermFd[2];
static int sigabrtFd[2];

// constructor
UnixSignals::UnixSignals (QObject *parent) : QObject(parent)
{
  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd))
    qFatal("Couldn't create HUP socketpair");

  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
    qFatal("Couldn't create TERM socketpair");

  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigabrtFd))
    qFatal("Couldn't create ABRT socketpair");

  setup_unix_signal_handlers ();

  snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
  connect(snHup, SIGNAL(activated(int)), this, SLOT(handleSigHup()));
  snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
  connect(snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));
  snAbrt = new QSocketNotifier(sigabrtFd[1], QSocketNotifier::Read, this);
  connect(snAbrt, SIGNAL(activated(int)), this, SLOT(handleSigAbrt()));
}

// destructor
UnixSignals::~UnixSignals ()
{
}

// hupSignalHandler
static void
hupSignalHandler (int)
{
  char a = 1;
  ::write(sighupFd[0], &a, sizeof(a));
}

// termSignalHandler
static void
termSignalHandler (int)
{
  char a = 1;
  ::write(sigtermFd[0], &a, sizeof(a));
}

// abrtSignalHandler
static void
abrtSignalHandler (int)
{
  char a = 1;
  ::write(sigtermFd[0], &a, sizeof(a));
}


// segvfpebusSignalHandler
static void
segvfpebusSignalHandler (int)
{
  qApp->exit (1);
#if defined (Q_OS_WIN) || defined (Q_OS_MAC)
  exit (1);
#else
  quick_exit (1);
#endif
}

// handleSigTerm
void
UnixSignals::handleSigTerm ()
{
  snTerm->setEnabled(false);
  char tmp;
  ::read(sigtermFd[1], &tmp, sizeof(tmp));

  emit sigTerm ();

  snTerm->setEnabled(true);
}

// handleSigHup
void
UnixSignals::handleSigHup ()
{
  snHup->setEnabled(false);
  char tmp;
  ::read(sighupFd[1], &tmp, sizeof(tmp));

  emit sigHup ();

  snHup->setEnabled(true);
}

// handleSigAbrt
void
UnixSignals::handleSigAbrt ()
{
  snAbrt->setEnabled(false);
  char tmp;
  ::read(sigabrtFd[1], &tmp, sizeof(tmp));

  emit sigAbrt ();

  snAbrt->setEnabled(true);
}

// setup_unix_signal_handlers
static int
setup_unix_signal_handlers ()
{
  struct sigaction hup, term, abrt;

  hup.sa_handler = hupSignalHandler;
  sigemptyset(&hup.sa_mask);
  hup.sa_flags = 0;
  hup.sa_flags |= SA_RESTART;

  if (sigaction(SIGHUP, &hup, 0) > 0)
    return 1;

  term.sa_handler = termSignalHandler;
  sigemptyset(&term.sa_mask);
  term.sa_flags |= SA_RESTART;

  if (sigaction(SIGTERM, &term, 0) > 0)
    return 2;

  abrt.sa_handler = abrtSignalHandler;
  sigemptyset(&abrt.sa_mask);
  abrt.sa_flags |= SA_RESTART;

  if (sigaction(SIGABRT, &abrt, 0) > 0)
    return 3;
/*
#ifdef Q_OS_MAC
  struct sigaction fpe, segv;

  fpe.sa_handler = segvfpebusSignalHandler;
  sigemptyset(&fpe.sa_mask);
  fpe.sa_flags |= SA_RESTART;

  if (sigaction(SIGFPE, &fpe, 0) > 0)
    return 4;

  segv.sa_handler = segvfpebusSignalHandler;
  sigemptyset(&segv.sa_mask);
  segv.sa_flags |= SA_RESTART;

  if (sigaction(SIGSEGV, &segv, 0) > 0)
    return 5;
#endif // Q_OS_MAC
*/

  struct sigaction bus;

  bus.sa_handler = segvfpebusSignalHandler;
  sigemptyset(&bus.sa_mask);
  bus.sa_flags |= SA_RESTART;

  if (sigaction(SIGBUS, &bus, 0) > 0)
    return 6;

  return 0;
}

#endif // Q_OS_WIN
