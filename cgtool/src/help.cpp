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

#include "cgtool.h"

int
help ()
{
  out <<
      "ChartGeany command line tool version " <<
      VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << endl <<
       APPCOPYRIGHT << endl << endl <<
       APPWEBPAGE << endl << endl <<
      "This program is free software; you can redistribute it and/or modify" << endl <<
      "it under the terms of the GNU General Public License as published by" << endl <<
      "the Free Software Foundation; either version 2 of the License, or" << endl <<
      "(at your option) any later version." << endl <<
      endl <<
      "This program is distributed in the hope that it will be useful," << endl <<
      "but WITHOUT ANY WARRANTY; without even the implied warranty of" << endl <<
      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" << endl <<
      "GNU General Public License for more details." << endl <<
      endl <<
      "You should have received a copy of the GNU General Public License" << endl <<
      "along with this program. If not, see http://www.gnu.org/licenses/." << endl <<
      endl <<

      "syntax: " << TOOLNAME << " mode option [PARAMETER] [DBFILE]" << endl << endl <<
      "mode: data" << endl <<
      "options: " << endl <<
      "--export TABLEID             table to psv with .csv extension" << endl <<
      "--export-tostdout TABLEID    table to stdout" << endl <<
      "--list                       list all data tables" << endl << endl <<
      "mode: help" << endl << endl <<
      "mode: info" << endl <<
      "--cgscript-toolchain TOOLCHAIN   CGScript toolchain information" << endl <<
      "--dbfile                         database file information" << endl << endl <<
      "mode: module" << endl <<
      "options:" << endl <<
      "--compile FILE               compile from cgs to cgm" << endl <<
      "--delete MODULEID            delete module" << endl <<
      "--decompile FILE             decompile cgm to cgs" << endl <<
      "--export MODULEID            export cgm" << endl <<
      "--import FILE                import cgm" << endl <<
      "--list                       list all modules" << endl <<
      "--verbose-compile FILE       compile from cgs to cgm" << endl;

  return 0;
}
