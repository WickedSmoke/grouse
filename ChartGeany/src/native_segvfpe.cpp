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

#ifndef Q_OS_WIN        // this is used exclusively by unix platforms

#include <unistd.h>
#include <ucontext.h>
#include <csignal>
#include <cstring>

#include "libdis.h"

// x86 and x86_64 max instruction size
static const int INSTR_SIZE = 15;

#ifdef Q_OS_LINUX
//for i386
#ifdef __i386__
#define INCREMENT(x, l) (x)->uc_mcontext.gregs[REG_EIP]+=l
//for x64
#elif __x86_64__
#define INCREMENT(x, l) (x)->uc_mcontext.gregs[REG_RIP]+=l
#endif
#endif // Q_OS_LINUX

#ifdef Q_OS_MAC
//for i386
#ifdef __i386__
#define INCREMENT(x, l) (x)->uc_mcontext->_ss.__eip+=l
//for x64
#elif __x86_64__
#define INCREMENT(x, l) (x)->uc_mcontext->__ss.__rip+=l
#endif
#endif

// last signal received
volatile static std::sig_atomic_t lastsig = 0;

// get last signal
int
get_last_signal ()
{
  int result = static_cast <int> (lastsig);

  lastsig = 0;

  return result;
}

// the signal handler
static void
handler(int sig, siginfo_t *si, void *vcontext)
{
  ucontext_t *context = static_cast <ucontext_t *> (vcontext);
  unsigned char buff[INSTR_SIZE];
  x86_insn_t insn;
  int isize;
  char err[128];

  Q_UNUSED (si)

#ifdef Q_OS_LINUX
#ifdef __i386__
  memcpy (buff,
          reinterpret_cast <void *> ((context->uc_mcontext.gregs[REG_EIP])),
          INSTR_SIZE);
#else
  memcpy (buff,
          reinterpret_cast <void *> ((context->uc_mcontext.gregs[REG_RIP])),
          INSTR_SIZE);
#endif
#endif

#ifdef Q_OS_MAC
#ifdef __i386__
  memcpy (buff,
          reinterpret_cast <void *> ((context->uc_mcontext->__ss.__eip)),
          INSTR_SIZE);
#else
  memcpy (buff,
          reinterpret_cast <void *> ((context->uc_mcontext->__ss.__rip)),
          INSTR_SIZE);
#endif
#endif

  x86_init (opt_none, nullptr, nullptr);
  isize = x86_disasm (buff, INSTR_SIZE, 0, 0, &insn);

  if (isize > 0)
    INCREMENT (context, isize);

  lastsig = sig;

#ifdef DEBUG
  if (isize == 0)
  {
    strcpy (err, "instruction size zero \n");
    write (2, (void *) err, strlen (err));
  }

  if (sig == SIGFPE)
  {
    strcpy (err, "fpe in handler \n");
    write (2, (void *) err, strlen (err));
  }

  if (sig == SIGSEGV)
  {
    strcpy (err, "segv in handler \n");
    write (2, (void *) err, strlen (err));
  }

  if (sig == SIGILL)
  {
    strcpy (err, "ill in handler \n");
    write (2, (void *) err, strlen (err));
  }
#endif

  if (sig != SIGSEGV && sig != SIGFPE && sig != SIGILL)
  {
    char ssig[3];
    ssig[0] = static_cast <char> ((sig / 10) + '0');
    ssig[1] = static_cast <char> ((sig % 10) + '0');
    ssig[2] = 0;

    strcpy (err, ssig);
    strcat (err, " unhandled signal \n");
    write (2, static_cast <void *> (err), strlen (err));
  }

  x86_oplist_free (&insn);
  return;
}

// install SIGFPE handler
static bool
install_fpe_handler ()
{
  struct sigaction fpe;

  fpe.sa_sigaction = handler;
  fpe.sa_flags = SA_SIGINFO;
  sigemptyset(&fpe.sa_mask);

  if (sigaction(SIGFPE, &fpe, nullptr) > 0)
    return false;

  return true;
}

// install SIGSEGV handler
static bool
install_segv_handler ()
{
  struct sigaction segv;

  segv.sa_sigaction = handler;
  segv.sa_flags = SA_SIGINFO;
  sigemptyset(&segv.sa_mask);

  if (sigaction(SIGSEGV, &segv, nullptr) > 0)
    return false;

  return true;
}

// install SIGILL handler
static bool
install_ill_handler ()
{
  struct sigaction ill;

  ill.sa_sigaction = handler;
  ill.sa_flags = SA_SIGINFO;
  sigemptyset(&ill.sa_mask);

  if (sigaction(SIGILL, &ill, nullptr) > 0)
    return false;

  return true;
}


void
install_signal_handlers ()
{
  bool rslt;

  rslt = install_fpe_handler ();
  rslt = install_segv_handler ();
  rslt = install_ill_handler ();

  Q_UNUSED (rslt)
}

#endif // !Q_OS_WIN
