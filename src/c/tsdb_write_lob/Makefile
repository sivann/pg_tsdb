
MODULES = tsdb_write_lob
DATA_built = tsdb_write_lob.sql

PG_CPPFLAGS = -I/usr/local//lib  -L/usr/local/lib -lmsgpack -lz4 #-rpath=/usr/local/lib 
COPT = -L/usr/local/lib -lmsgpack /usr/local/lib/liblz4.a

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

