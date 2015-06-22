CREATE FUNCTION tsdb_write_lob(smallint, smallint, oid, VARCHAR,VARCHAR,timestamp,timestamp)
RETURNS void
as '$libdir/tsdb_write_lob' , 'tsdb_write_lob' 
LANGUAGE C STRICT;
