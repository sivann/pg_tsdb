1. TEST Types
==============================================================================

A. Sample pg table
-----------------------
CREATE TABLE mev_tsdb(
  measurement_source_id uuid,  -- 7 bytes
  measurement_start_date, -- 4 bytes, truncated to i.e. 1 month granularity
  granularity_days smallint, -- 2 bytes, days per oid
  datatype smallint, -- 2 bytes, 0: compressed, 1: uncompressed, ...
  data oid default null -- actual data, msgpack, compressed
);


B. Data column format
-----------------------
1) compressed array of dicts.
e.g.: [{'v':mvalue,'r':rtime_date, 't':mtime_time,'q':mqual}, ...]

all dates are strings.
dates are 22 bytes each


2) same as 1, but time expressed as delta from measurement_start_date
e.g.: [{'v':mvalue,'r':rtime_date, 't':mtime,'q':mqual}, ...]
rtime_date: delta in seconds (positive but could be negative when there is a clock synchronization problem) (32 bit, 4 bytes)
mtime_time: delta in minutes (positive). 2 Options: [32bit, with "unlimited" data/OID]  OR  [16 bit, 2 bytes, gives up to 45 days per OID]


3) same as 1, but without compression

4) same as 2, but without compression

5) same as 2, but per week files

6) re-run the best of 1,2,3,4,5 and log min,max,avg,total file sizes every 100000 rows





2. RESULTS
==============================================================================

long delta dates per month (ffd92b88-146b-11e2-8cd1-00163e856739-2013-12)
----
>>> sys.getsizeof(str(data2)) #json
227952
>>> sys.getsizeof(data1) #msgpack
145719
>>> sys.getsizeof(data) #msgpack+compress
14036
>>> len(data2)
2976
>>> data2[0]
{'q': '1.00000000e+02', 'r': 15442341, 't': 21600, 'v': '7.54000000e+00'}


string dates per month (ffd92b88-146b-11e2-8cd1-00163e856739-2013-12)
----
{'q': '1.00000000e+02', 'r': '20140528143221', 't': '01003000', 'v': '7.54000000e+00'}
>>> sys.getsizeof(str(data2)) #json
261928
>>> sys.getsizeof(data1)  #msgpack
187531
>>> sys.getsizeof(data) #compressed + msgpack
9606
>>> len(data2) #rows (TR)
2976
>>> data2[0]
{'q': '1.00000000e+02', 'r': '20140528143221', 't': '01060000', 'v': '7.54000000e+00'}
>>> sys.getsizeof(zlib.compress(str(data2))) #compressed json
10288


A. TESTS with format 1.B.1
----------------------------

1)
1 file / day, 100Klines, 530 msrcs
first 100K lines test
txt: 19MB

uncompressed
300 files
data: 11M

2)
compress whole day:
first 100K lines test
300 files
zdata: 1.4M

3)
1 file /msrc/day
first 100K lines test
compressed
1579 files
zdatamsrc: 6.3M

4)
1 file /msrc/month
first 100K lines test
compressed
775 files
zdatamsrc: 3.1M

36750000 (37M)lines from dump file, 2370 measurement sources, ~4yrs
14973 files compressed
117916994 bytes (154MB) (zdatamsrc-month-36750000)
original dump (37Mlines): 6982500000 bytes (6.6GB), gzipped: 206096198 bytes (1:33)
tsdb compression over dump file: 1:59
estimating about 35:1 in DB


B. TESTS with format 1.B.2
----------------------------
1)
1 file/msrc/month
36750000 (37M)lines from dump file, 2370 measurement sources, ~4yrs
14973 files compressed
172722450 bytes (203MB) (zdatamsrc-month-delta-36750000)



