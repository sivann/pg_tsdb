

select tsdb_insert('e76b8964-146c-11e2-ac1d-00163e856739','2012-10-19 03:15:00+03'::timestamp,'1234','1','2014-05-27 23:50:32+03'::timestamp);

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
