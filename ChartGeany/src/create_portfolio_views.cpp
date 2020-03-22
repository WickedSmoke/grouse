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
 
// create portfolio views
QString
createportfolioviews (QString viewname)
{
  QString SQL = "";

// progressive quantity
  SQL +=
    "CREATE VIEW V_NAMEpq AS \
     SELECT t1.tr_id, \
     ((SELECT coalesce (SUM (t2.quantity),0) FROM \
      (SELECT * FROM V_NAME WHERE trtype = 'BUY' AND V_NAME.symbol = t1.symbol) AS t2 \
       WHERE t2.tr_date <= t1.tr_date) - \
      (SELECT coalesce (SUM (t3.quantity),0) FROM \
      (SELECT * FROM V_NAME WHERE trtype = 'SELL' AND V_NAME.symbol = t1.symbol) AS t3 \
       WHERE t3.tr_date <= t1.tr_date)) AS PROGQUANT \
     FROM (SELECT * FROM V_NAME) AS t1;";

// progressive cash
  SQL +=
    "CREATE VIEW V_NAMEpc AS \
      SELECT t3.tr_id, \
      ((SELECT coalesce (SUM (amount), 0) FROM V_NAME \
        WHERE t3.tr_date  > V_NAME.tr_date) + \
       (SELECT coalesce (SUM (amount), 0) FROM V_NAME \
        WHERE t3.tr_date  = V_NAME.tr_date AND \
              t2.tr_id  >= t3.tr_id  AND \
              t3.tr_id  > V_NAME.tr_id)  + t3.amount) AS PROGCASH \
	  FROM \
		(SELECT tr_date, MAX (tr_id) AS tr_id \
		 FROM V_NAME \
		 GROUP BY tr_date \
		 ORDER BY tr_date, tr_id) AS t2, \
		(SELECT * FROM V_NAME \
		 ORDER BY tr_date, tr_id) AS t3 \
	  WHERE t2.tr_date = t3.tr_date \
	  ORDER BY t2.tr_date, t2.tr_id;";

// transactions with progressive cash and quantities
  SQL +=
    " CREATE VIEW V_NAMEfull AS \
		SELECT V_NAME.*, \
             V_NAMEpq.progquant AS PROGQUANT, \
	         V_NAMEpc.progcash AS PROGCASH \
      FROM  V_NAME, V_NAMEpq, V_NAMEpc \
      WHERE \
            V_NAME.tr_id = V_NAMEpq.tr_id AND \
            V_NAME.tr_id = V_NAMEpc.tr_id \
      ORDER BY V_NAME.tr_date DESC, V_NAME.tr_id DESC;";

// last transaction of each symbol
  SQL +=
    "CREATE VIEW V_NAMEfull2 AS\
		SELECT\
		    V_NAMEfull.symbol,\
		    V_NAMEfull.tr_date,\
			MAX (V_NAMEfull.tr_id) AS tr_id\
		FROM\
			V_NAMEfull,\
			(SELECT symbol, MAX (tr_date) AS tr_date FROM V_NAMEfull\
			 WHERE trtype = 'BUY' OR trtype = 'SELL'\
			 GROUP BY symbol) AS t1\
		WHERE\
			t1.symbol = V_NAMEfull.symbol AND\
			t1.tr_date =V_NAMEfull.tr_date\
		GROUP BY\
			V_NAMEfull.symbol, V_NAMEfull.tr_date;";

// portfolio summary
  SQL +=
    "CREATE VIEW V_NAMEsummary AS \
      SELECT V_NAMEfull.symbol AS SYMBOL, V_NAMEfull.progquant AS QUANTITY, \
       (SELECT coalesce ((SUM (amount) * -1)/SUM(quantity), 0) FROM V_NAME AS t1 \
      WHERE t1.TRTYPE = 'BUY' AND t1.SYMBOL = V_NAMEfull.symbol) AS PRICEPAID, \
      V_NAMEfull.datafeed AS DATAFEED \
      FROM V_NAMEfull, V_NAMEfull2 AS t1 \
      WHERE V_NAMEfull.tr_id = t1.tr_id ;";
// portfolio current view
  SQL +=
    "CREATE VIEW V_NAMEcview AS \
	  SELECT \
	  s.SYMBOL AS SYMBOL, \
      coalesce ((SELECT p.price FROM prices AS p WHERE p.feed = s.DATAFEED \
      AND p.symbol = s.SYMBOL), 0) AS PRICE, \
      coalesce ((SELECT p.change FROM prices AS p WHERE p.feed = s.DATAFEED \
      AND p.symbol = s.SYMBOL), '0') AS CHANGE, \
      coalesce ((SELECT p.prcchange FROM prices AS p WHERE p.feed = s.DATAFEED \
      AND p.symbol = s.SYMBOL), '0') AS PRCCHANGE, \
      coalesce ((SELECT p.date FROM prices AS p WHERE p.feed = s.DATAFEED \
      AND p.symbol = s.SYMBOL), '') AS DATE, \
      coalesce ((SELECT p.time FROM prices AS p WHERE p.feed = s.DATAFEED \
      AND p.symbol = s.SYMBOL), '') AS TIME, \
      coalesce ((SELECT p.volume FROM prices AS p WHERE p.feed = s.DATAFEED \
      AND p.symbol = s.SYMBOL), '0') AS VOLUME, \
      s.QUANTITY AS QUANTITY, s.PRICEPAID AS PRICEPAID, \
      (coalesce ((SELECT p.price FROM prices AS p WHERE p.feed = s.DATAFEED \
      AND p.symbol = s.SYMBOL), 0) * s.QUANTITY)AS MARKETVALUE \
      FROM V_NAMEsummary AS s WHERE s.QUANTITY <> 0 ORDER BY SYMBOL;";

// portfolio current view with gain and gain %
  SQL +=
    "CREATE VIEW V_NAMEcview2 AS \
	  SELECT \
	  c.*, \
	  ((c.quantity * c.price) - (c.quantity * c.pricepaid)) AS GAIN, \
	  coalesce (((((c.quantity * c.price) / (c.quantity * c.pricepaid)) - 1) * 100), 0) AS PRCGAIN, \
	  (c.quantity * c.change) AS DAYCHANGE,\
	  CHAR(32)||c.symbol AS ORDERCOL \
	  FROM V_NAMEcview AS c ORDER BY SYMBOL;";

// portfolio cash
  SQL +=
    "CREATE VIEW V_NAMEcview3 AS\
		SELECT 'Cash' AS symbol,\
		0 AS PRICE,\
		0 AS CHANGE,\
		0 AS PRCCHANGE,\
		'' AS DATE,\
		'' AS TIME,\
		0 AS VOLUME,\
		0 AS QUANTITY,\
		0 AS PRICEPAID,\
		progcash AS MARKETVALUE,\
		0 AS GAIN,\
		0 AS PRCGAIN,\
		0 AS DAYCHANGE,\
		CHAR(250) AS ORDERCOL\
		FROM V_NAMEfull ORDER BY tr_date DESC, tr_id DESC LIMIT 1;";

// portfolio total
  SQL +=
    "CREATE VIEW  V_NAMEcview4 AS\
        SELECT 'Total' AS symbol, 0 AS PRICE, 0 AS CHANGE,\
	      ((t2.DAYCHANGE / ((t2.MARKETVALUE +\
	      t3.MARKETVALUE) - t2.DAYCHANGE)) * 100) AS PRCCHANGE,\
	      '' AS DATE, '' AS TIME, 0 AS VOLUME, 0 AS QUANTITY, 0 AS PRICEPAID,\
	      (t2.MARKETVALUE + t3.MARKETVALUE) AS MARKETVALUE,\
	      t2.GAIN AS GAIN,\
	     (t2.GAIN / ((t2.MARKETVALUE + t3.MARKETVALUE) + (t2.GAIN * -1)) * 100) AS PRCGAIN,\
	     t2.DAYCHANGE AS DAYCHANGE, CHAR(251) AS ORDERCOL\
    FROM\
	  (SELECT SUM (DAYCHANGE) AS DAYCHANGE,\
	   SUM (MARKETVALUE) AS MARKETVALUE,\
	   SUM (GAIN) AS GAIN FROM  V_NAMEcview2) AS t2,\
	  (SELECT MARKETVALUE FROM  V_NAMEcview3) AS t3;";

// portfolio with cash and total
  SQL +=
    "CREATE VIEW V_NAMEcview5 AS \
        SELECT * FROM V_NAMEcview2 UNION \
        SELECT * FROM V_NAMEcview3 UNION \
        SELECT * FROM V_NAMEcview4;";

// portfolio with cash and total filtering nulls
  SQL +=
    "CREATE VIEW V_NAMEcview6 AS \
        SELECT symbol, \
        COALESCE (price, 0) AS PRICE, \
        COALESCE (change, 0) AS CHANGE, \
        COALESCE (prcchange, 0) AS PRCCHANGE, \
        DATE, TIME, \
        COALESCE (volume, 0) AS VOLUME, \
        COALESCE (quantity, 0) AS QUANTITY,\
        COALESCE (pricepaid, 0) AS PRICEPAID, \
        COALESCE (marketvalue, 0) AS MARKETVALUE, \
        COALESCE (gain, 0) AS GAIN, \
        COALESCE (prcgain, 0) AS PRCGAIN, \
        COALESCE (daychange, 0) AS DAYCHANGE, \
        ORDERCOL\
        FROM V_NAMEcview5\
        ORDER BY ordercol;";

  SQL.replace ("V_NAME", viewname);
  return SQL;
}
