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

#include <QDateTime>
#include <QCryptographicHash>
#include <QTextStream>

#include "compile.h"
#include "common.h"

Compile::Compile ()
{
  process = new (std::nothrow) QProcess (this);
  if (process == nullptr)
    return;

  connect(process, SIGNAL(readyReadStandardOutput()), SLOT(onStdoutAvailable()));
  connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(onFinished(int,QProcess::ExitStatus)));
}

Compile::~Compile ()
{

}

#include "compile_static.cpp"

// compile source and create module
CG_ERR_RESULT
Compile::CGSCript (QString program,             // the source code
                   QString &programname,        // module's name
                   QString &messages,           // messages
                   bool mode)                   // true for debug
{
  CG_ERR_RESULT result = CG_ERR_OK;
  bool native =
    (Application_Settings->options.compiler.trimmed () == "");

  debug = mode;
  compiletmpfname = tempSourcePath ();

  // create the final script with top.h and bottom.h prepended and
  // appended respectively
  result = createFinalScript (program, programname, compiletmpfname);
  if (result != CG_ERR_OK)
    return result;

  QString compileoutputfname = compiletmpfname % SOEXT,
          pstring = platformString ();

  compilestatus = false;
  compileoutputfname = compiletmpfname % SOEXT;

  if (native) // native compile
  {
    result = nativeCompile (compiletmpfname, compileoutputfname,
                            pstring, output, mode);

    if (result == CG_ERR_OK)
       compilestatus = true;

    if (output.size () > 0)
      compilestatus = false;
  }
  else      // external compiler
  {
    result = externalCompile (compiletmpfname, compileoutputfname,
                              process, mode);

    if (result == CG_ERR_OK)
       compilestatus = true;

    if (output.size () > 0)
      compilestatus = false;
  }

  // create module
  if (compilestatus == true)
  {
    output +=
      QStringLiteral ("compilation succeeded on: ") +
      QDateTime::currentDateTime().toString ("yyyy-MM-dd hh:mm:ss") +
      QStringLiteral ("\n");

    result = createModule (program, programname, compileoutputfname);
    if (result == CG_ERR_OK)
      output += QStringLiteral ("module created ");
    else
      output += QStringLiteral ("module creation failed on: ") %
        QDateTime::currentDateTime().toString ("yyyy-MM-dd hh:mm:ss");
  }
  else
  if (native)
    output += QStringLiteral ("compilation failed on: ") %
              QDateTime::currentDateTime().toString ("yyyy-MM-dd hh:mm:ss");


  messages = output;

  QFileInfo fi (compiletmpfname);
  QDir dir;
  dir.remove (compiletmpfname);
  dir.remove (compiletmpfname % QStringLiteral (".o"));
  dir.remove (compiletmpfname % SOEXT);
  dir.rmdir (fi.dir ().absolutePath());

  return result;
}

/// slots
void
Compile::onStdoutAvailable(void)
{


}

// output available from stderr
void
Compile::onFinished(int code, QProcess::ExitStatus status)
{
  Q_UNUSED (status)

  output += QString::fromLocal8Bit (process->readAllStandardError().data());
  compilestatus = code;
  output.remove (compiletmpfname % QStringLiteral (":"));
  output.replace ("`", "'");
  output.replace ("‘", "'");
  output.replace ("â", "");
  output.replace ("â", "");
  if (code != 0)
    output += QStringLiteral ("compilation failed on: ") %
              QDateTime::currentDateTime().toString ("yyyy-MM-dd hh:mm:ss");
}

