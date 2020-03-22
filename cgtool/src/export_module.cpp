#include <QFile>

#include "simplecrypt.h"
#include "cgtool.h"

// export module
int
export_module (const QString &moduleid, const QString &dbfile)
{
  // check if sqlite file exists
  if (!checkFileExistence (dbfile))
    return 1;
    
  // open sqlite file
  sqlite3 *db = nullptr;
  int rc = sqlite3_open_v2(dbfile.toUtf8 (), &db, SQLITE_OPEN_READWRITE, nullptr);

  if (db == nullptr)
  {
     err << "error: " << "unable to allocate memory" << endl;
     return 1;
  }

  if (rc != SQLITE_OK)
  {
    err << "error: " << sqlite3_errmsg (db) << endl;
    sqlite3_close (db);
    return 1;
  }
  
  QString SQL =
    QStringLiteral ("SELECT * FROM modules WHERE id LIKE '") %
    moduleid.left (6) % "%" % moduleid.right (2) % "' LIMIT 1;";

  ModuleVector modvec;
  rc = selectfromdb(db, SQL.toUtf8(), sqlcb_modules, static_cast <void *> (&modvec));
  if (rc != SQLITE_OK)
  {
    err << "error: "<< sqlite3_errmsg (db) << endl;
    sqlite3_close (db);
    return 1;
  }  
  
  if (modvec.size () == 0)
  {
    err << "error: module not found" << endl;
    return 1;
  }
  
  SQL = "DELETE FROM modules WHERE id = '" % modvec[0]->id % "';";
  SQL.append ("\n");
  SQL +=
    "INSERT INTO modules (id, name, source, author, type, version, status, platform, binary)\
   VALUES ('" % modvec[0]->id % "','"\
    + modvec[0]->name % "','"\
    + modvec[0]->source % "','"\
    + modvec[0]->author % "','"\
    + modvec[0]->type % "','"\
    + modvec[0]->version % "','"\
    % "DISABLED', '', '');";  
  
  QFile modscript (modvec[0]->name + ".cgm");
  if (!modscript.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    err << "error: cannot open output file" << endl;
    return 1;
  }
  
  SimpleCrypt crypto(ENCKEY);  
  QString encstr = crypto.encryptToString (SQL);
  modscript.write(encstr.toUtf8 ());
  modscript.close();
  
  sqlite3_close (db);
  	
  return 0;
}  	
