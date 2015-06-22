CREATE or replace FUNCTION lala1 (msid uuid, mt timestamp)
  RETURNS timestamp with time zone
AS $$
  if (mt is None):
    return None
  return mt
$$ LANGUAGE plpythonu;

CREATE TABLE mev_tsdb(
  measurement_event uuid, 
  measurement_date_start date,
  data oid default null
);

-- lob functions:
\dfS lo_*

-- lobs:
\dl

-- for lo_open:
#define INV_WRITE               0x00020000
#define INV_READ                0x00040000

-- for lo_seek:
#define SEEK_SET 0 /* Seek from beginning of file. */
#define SEEK_CUR 1 /* Seek from current position. */
#define SEEK_END 2 /* Seek from end of file */


-- show lobs: 
select lo_import('/tmp/koko.jpg');
select loid,pageno  from pg_largeobject; -- or \dl

BEGIN
 select lo_open (43172, CAST(x'20000' | x'40000' AS integer));
 select loread(0,10);         -- gia parametrous: select loread(colname,col2name) FROM tablename klp..
commit;
 
 
http://www.postgresql.org/docs/devel/static/plpython-database.html

http://www.slideshare.net/petereisentraut/programming-with-python-and-postgresql page 70-71
pl/python execute sql:
rv=plpy.execute("UPDATE ...")
return rv.nrows

or rv[0]['colname']

large object commands: 
\dfS lo_*

LOB:
http://michael.otacoo.com/postgresql-2/playing-with-large-objects-in-postgres/

LOB test code:
https://github.com/postgres/postgres/blob/master/src/test/regress/output/largeobject.source
