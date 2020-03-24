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

// form temporary path for source file
static QString
tempSourcePath ()
{
  // form temporary source path
  QDir dir;

  QByteArray ba =
    (QString::number(QDateTime::currentMSecsSinceEpoch ())).toLatin1 ();

  QString tmpdir = QString(QCryptographicHash::hash((ba),
                   QCryptographicHash::Md5).toHex());

  dir.mkdir (QDir::tempPath () % QDir::separator()  % tmpdir);
  QString tmppath =
    QDir::tempPath () % QDir::separator()  % tmpdir %
    QDir::separator()  % tmpdir % QStringLiteral (".c");

  return tmppath;
}

// create final script prepending top.h and appending bottom.h
// program: the code as it is
static CG_ERR_RESULT
createFinalScript (QString program,
                   QString programname,
                   QString compiletmpfname)
{
  QFile file(compiletmpfname);
  if (!file.open (QIODevice::WriteOnly|QIODevice::Text))
    return CG_ERR_OPEN_FILE;

// remove preprocessor directives
  QStringList lines = program.split(QStringLiteral ("\n"));
  for (int counter = 0; counter < lines.size (); counter ++)
  {
    QString line;
    line = lines[counter];
    line = line.trimmed ();
    if (line.left (1) == QStringLiteral ("#"))
    {
      lines[counter] = line.prepend (QStringLiteral ("/*"));
      lines[counter] = line.append (QStringLiteral ("*/"));
    }
  }
  QString source = lines.join ("\n");

// add top.h
  QFile TopFile;

#ifdef CGTOOL
  TopFile.setFileName (QString::fromUtf8 (":/source/ChartGeany/cgscript/include/top.h"));
#else
  TopFile.setFileName (QString::fromUtf8 (":/source/cgscript/include/top.h"));
#endif

#ifdef DEBUG
#ifndef CGTOOL
  if (Application_Options.platform.contains ("linux"))
    TopFile.setFileName (QString::fromUtf8 ("ChartGeany/cgscript/include/top.h"));
#endif
#endif // DEBUG

  TopFile.open (QIODevice::ReadOnly|QIODevice::Text);
  QTextStream ReadTopFile(&TopFile);
  QString topheader;
  topheader = ReadTopFile.readAll();
  TopFile.close ();
  topheader.replace (QStringLiteral ("_SOURCE_NAME_"), programname);
  source.prepend (topheader);

// add bottom.h
  QFile BottomFile;

#ifdef CGTOOL
  BottomFile.setFileName (QString::fromUtf8 (":/source/ChartGeany/cgscript/include/bottom.h"));
#else
  BottomFile.setFileName (QString::fromUtf8 (":/source/cgscript/include/bottom.h"));
#endif

#ifdef DEBUG
#ifndef CGTOOL
  if (Application_Options.platform.contains (QStringLiteral ("linux")))
    BottomFile.setFileName (QString::fromUtf8 ("ChartGeany/cgscript/include/bottom.h"));
#endif
#endif  // DEBUG

  BottomFile.open (QIODevice::ReadOnly|QIODevice::Text);
  QTextStream ReadBottomFile(&BottomFile);
  QString bottomheader;
  bottomheader = ReadBottomFile.readAll();
  BottomFile.close ();
  source.append (bottomheader);

// write to temporary source
  QTextStream TempStream (&file);
  TempStream << source;
  file.flush();
  file.close ();

#ifdef CGTOOL
  if (verbose ())
    out << source;
#endif

  return CG_ERR_OK;
}

// create module from object code
static CG_ERR_RESULT
createModule (QString program,            // the source
              QString &programname,       // module's name
              QString object,             // full path of the object
              sqlite3 *db = 0)            // sqlite3 database handler
{
  CG_ERR_RESULT result = CG_ERR_OK;
  QString
    modsource = program.replace ("'", "`"), modid = "", modauthor,
    modversion, modtype, modname, modplatform, modbinary;
  modsource.replace (";", ";" % QChar ('\n'));
  QStringList lines = modsource.split("\n");

  foreach (const QString line, lines)
  {
    if (line.contains (QStringLiteral ("Property ")) &&
        line.contains (QStringLiteral ("MODID")))
      modid = line.split(QStringLiteral ("\"")).at (1);
    if (line.contains (QStringLiteral ("Property ")) &&
        line.contains (QStringLiteral ("MODAUTHOR")))
      modauthor = line.split(QStringLiteral ("\"")).at (1);
    if (line.contains (QStringLiteral ("Property ")) &&
        line.contains (QStringLiteral ("MODVERSION")))
      modversion = line.split(QStringLiteral ("\"")).at (1);
    if (line.contains (QStringLiteral ("SetObjectType")) &&
        line.contains (QStringLiteral ("_OBJECT")))
    {
      modtype = line.split(QStringLiteral ("(")).at (1).split (QStringLiteral (")")).at (0).trimmed ();
      modtype.replace (QStringLiteral ("_OBJECT"), QStringLiteral (""));
    }
  }

  modname = programname;
#ifdef CGTOOL
  modplatform = platformString ();
#else
  modplatform = Application_Options->platform;
#endif

  QFile binfile (object);
  binfile.open (QIODevice::ReadOnly);
  QByteArray *loadedArray = new QByteArray(binfile.readAll());
  QByteArray *encodedFile = new QByteArray(loadedArray->toHex());
  modbinary = QString::fromStdString (encodedFile->data ());
  delete encodedFile;
  delete loadedArray;
  binfile.close ();

  // delete the previous version if exists
  QString
    SQL = QStringLiteral ("DELETE FROM modules WHERE id = '") %
          modid % QStringLiteral ("';");

  // insert the new version
  SQL += QStringLiteral ("INSERT INTO modules (id, name, source, platform, author, version, type, pak, binary) \
             VALUES ('") % modid % QStringLiteral ("','") %
             modname % QStringLiteral ("','") %
             modsource % QStringLiteral ("','") %
             modplatform % QStringLiteral ("','") %
             modauthor % QStringLiteral ("','") %
             modversion % QStringLiteral ("','") %
             modtype % QStringLiteral ("','") %
             QStringLiteral ("") % QStringLiteral ("','") %
             modbinary % QStringLiteral ("');");
#ifdef CGTOOL
  int rc = updatedb (db, SQL);
#else
  Q_UNUSED (db);
  int rc = updatedb (SQL);
#endif
  if (rc != SQLITE_OK)
    result = CG_ERR_ACCESS_DATA;

  return result;
}

#ifndef Q_OS_MAC
#include "libtcc.h"

// compilation error callback
static void
errfunc (void *opaque, const char *msg)
{
  QString *output = static_cast <QString *> (opaque);
  *output += QString (msg) % QStringLiteral ("\n");
}
#endif

// native compilation with libtcc
static CG_ERR_RESULT
nativeCompile (QString &programsource,  // full path of source code
               QString &objectoutput,   // full path of shared object/dll
               QString &platformstring, // linuxNN-cgs or winNN-cgs
               QString &messages,       // compiler output messages
               bool mode)
{
#ifndef Q_OS_MAC
  TCCState *state = tcc_new ();
  if (state == nullptr)
    return CG_ERR_NOMEM;

// set error functions
  tcc_set_error_func (state, static_cast <void *> (&messages), errfunc);

// common tcc options
  tcc_define_symbol (state, "_CGSCRIPT_MODULE", nullptr);
  tcc_set_options (state, "-std=c99");
  tcc_set_options (state, "-fsigned-char");
  tcc_set_options (state, "-nostdinc");
  tcc_set_options (state, "-shared");
  tcc_set_options (state, "-Wall");
  tcc_set_options (state, "-Werror");

// debug options
  if (mode) // debug
  {
    tcc_set_options (state, "-O0");
    tcc_define_symbol (state, "DEBUG_BUILD", nullptr);
    // tcc_set_options (state, "-g"); << segfaults
  }
//  else // release
    tcc_set_options (state, "-O2");

// arch options
  if (platformstring.contains (QStringLiteral ("32"))) // 32 bit
    tcc_set_options (state, "-m32");
  else
  if (platformstring.contains (QStringLiteral ("64"))) // 64 bit
    tcc_set_options (state, "-m64");

// os options
  if (platformstring.contains (QStringLiteral ("win"))) // windows
  {
    tcc_set_options (state, "-mms-bitfields");
    tcc_set_options (state, "-mno-sse");
    tcc_define_symbol (state, "_GGSCRIPT_NATIVE_WIN", nullptr);
  }
  else
    tcc_set_options (state, "-nostdlib"); // under windows, tcc tries to resolve
                                          // symbols with this option


// add source file
  if (tcc_add_file (state, programsource.toLocal8Bit ().constData ()) == -1)
  {
    tcc_delete (state);
    return CG_ERR_COMPILATION;
  }

// set output type (.so/.dll)
  if (tcc_set_output_type (state, TCC_OUTPUT_DLL) == -1)
  {
    tcc_delete (state);
    return CG_ERR_COMPILATION;
  }

// add output file
  if (tcc_output_file (state, objectoutput.toLocal8Bit ().constData ()) == -1)
  {
    tcc_delete (state);
    return CG_ERR_COMPILATION;
  }

  tcc_delete (state);
#else
Q_UNUSED (programsource)
Q_UNUSED (objectoutput)
Q_UNUSED (platformstring)
Q_UNUSED (messages)
Q_UNUSED (mode)
#endif // !Q_OS_MAC
  return CG_ERR_OK;
}

// compilation with external compiler
static CG_ERR_RESULT
externalCompile (QString &programsource,  // full path of source code
                 QString &objectoutput,   // full path of shared object/dll
                 QProcess *process,
                 bool debug)
{
  CG_ERR_RESULT result = CG_ERR_OK;

#ifdef Q_OS_WIN
  programsource = "\"" + programsource + "\"";
  objectoutput = "\"" + objectoutput + "\"";
#endif

  // form command lines for compile and link
  QString
    compilestr,
#ifdef CGTOOL
    CC = toolchain.compiler,
    CFLAGS_DEBUG = toolchain.compilerdbg,
    CFLAGS_RELEASE = toolchain.compilerrel,
    LFLAGS_DEBUG = toolchain.linkerdbg,
    LFLAGS_RELEASE = toolchain.linkerrel;
#else
    CC = Application_Options->compiler,
    CFLAGS_DEBUG = Application_Options->compilerdbg,
    CFLAGS_RELEASE = Application_Options->compilerrel,
    LFLAGS_DEBUG = Application_Options->linkerdbg,
    LFLAGS_RELEASE = Application_Options->linkerrel;
#endif
  LFLAGS_DEBUG += QStringLiteral (" -o ") % objectoutput % QStringLiteral (" ") % programsource;
  LFLAGS_RELEASE += QStringLiteral (" -o ") % objectoutput % QStringLiteral (" ") % programsource;

  // check compiler's existence
  QProcess proc_cc;
  QString commandToStart = CC;
  proc_cc.start(commandToStart);

  bool started = proc_cc.waitForStarted();
  if (!proc_cc.waitForFinished(5000)) // 5 Second timeout
    proc_cc.kill();

  int exitCode = proc_cc.exitCode();
  Q_UNUSED (exitCode);
  QString stdOutput = QString::fromLocal8Bit(proc_cc.readAllStandardOutput());
  QString stdError = QString::fromLocal8Bit(proc_cc.readAllStandardError());
  if (!started)
    return CG_ERR_NO_COMPILER;

#ifdef Q_CC_MSVC
  // We need this in order to inform module about the compiler we used to
  // compile the app
  CC = CC % QStringLiteral (" -D_MSC_USED ");
#endif // Q_CC_MSVC

  if (debug)
    compilestr =
      CC % QStringLiteral (" ") % CFLAGS_DEBUG % QStringLiteral (" ") % LFLAGS_DEBUG;
  else
    compilestr =
      CC % QStringLiteral (" ") % CFLAGS_RELEASE % QStringLiteral (" ") % LFLAGS_RELEASE;

  QString compile = compilestr;
  compile =
    compile.replace (QStringLiteral ("FNAME"),
                     (QChar (34) % programsource % QChar (34)));

#ifdef __GNUC__
#ifndef __clang__
  int gccver = __GNUC__;
  if (!compile.contains ("clang") && gccver < 5)
    compile.replace ("-fstack-protector", " ");
#endif // __clang__
#endif // __GNUC__<5

#ifdef CGTOOL
  if (verbose ())
    out << compile.trimmed ().replace ("   ", " ").replace ("  ", " ");

  Q_UNUSED (process);

  exitCode = QProcess::execute (compile);
  if (exitCode != 0)
    return CG_ERR_COMPILATION;
#else
  process->start(compile);
  if (process == nullptr)
    return  CG_ERR_NOMEM;

  if (process->waitForStarted (5000))
    process->waitForFinished(-1);
#endif

  return result;
}
