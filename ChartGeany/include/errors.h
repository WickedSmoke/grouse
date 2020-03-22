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

// Error codes
typedef enum
{
  CG_ERR_OK = 0,                // "No error"
  CG_ERR_OPEN_FILE,             // "Cannot open file"
  CG_ERR_CREATE_TEMPTABLE,      // "Cannot create temporary table"
  CG_ERR_CREATE_TABLE,          // "Cannot create table"
  CG_ERR_INSERT_DATA,           // "Cannot insert data"
  CG_ERR_DELETE_DATA,           // "Cannot delete data"
  CG_ERR_DBACCESS,              // "Cannot access database"
  CG_ERR_NETWORK,               // "Network error"
  CG_ERR_CREATE_TEMPFILE,       // "Cannot create temporary file"
  CG_ERR_WRITE_FILE,            // "Cannot write to file"
  CG_ERR_TRANSACTION,           // "Transaction error"
  CG_ERR_NOMEM,                 // "Not enough memory"
  CG_ERR_NOSYMBOL,              // "Symbol does not exist"
  CG_ERR_ACCESS_DATA,           // "Cannot access data"
  CG_ERR_NETWORK_TIMEOUT,       // "Network timeout"
  CG_ERR_INVALID_DATA,          // "Invalid data"
  CG_ERR_REQUEST_PENDING,       // "Request pending"
  CG_ERR_BUFFER_NOTFOUND,       // "Buffer not found"
  CG_ERR_NO_QUOTES,             // "No quotes for symbol"
  CG_ERR_FAILED,                // "Operation failed"
  CG_ERR_NO_COMPILER,           // "Compiler not found"
  CG_ERR_COMPILATION,           // "Compilation failed"
  CG_ERR_NO_DATA,               // "No data"
  CG_ERR_NO_API_KEY,            // "No api key"
  CG_ERR_OBJECT_TYPE            // "Invalid object type"
} CG_ERRORS;

typedef int CG_ERR_RESULT;
