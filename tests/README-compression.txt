INPUT
-------------------------------------------------------------------
{'q': '1.00000000e+02', 'r': 41572859L, 't': 421200L, 'v': '7.54000000e+00'}
{'q': '1.00000000e+02', 'r': 41572859L, 't': 423900L, 'v': '7.54000000e+00'}
{'q': '1.00000000e+02', 'r': 41572859L, 't': 426600L, 'v': '7.54000000e+00'}
{'q': '1.00000000e+02', 'r': 41572859L, 't': 403200L, 'v': '7.54000000e+00'}
{'q': '1.00000000e+02', 'r': 41572859L, 't': 405900L, 'v': '7.54000000e+00'}
{'q': '1.00000000e+02', 'r': 41572859L, 't': 418500L, 'v': '7.54000000e+00'}
{'q': '1.00000000e+02', 'r': 41572859L, 't': 399600L, 'v': '7.54000000e+00'}
{'q': '1.00000000e+02', 'r': 41572859L, 't': 404100L, 'v': '7.54000000e+00'}
{'q': '1.00000000e+02', 'r': 41572859L, 't': 406800L, 'v': '7.54000000e+00'}

ZLIB
-------------------------------------------------------------------
root@vserver-dev-2:/opt/tsdbtest# ./compressiontest.py 
*** Json:116, Packed:90, PackCompressed:86, compr/json:1 
*** mean Bytes/TR: Json:116, Packed:90, PackCompressed:86 

Processing with 4 rows per file
*** Json:344, Packed:237, PackCompressed:101, compr/json:3 
*** mean Bytes/TR: Json:86, Packed:59, PackCompressed:25 

Processing with 24 rows per file
*** Json:1864, Packed:1219, PackCompressed:192, compr/json:9 
*** mean Bytes/TR: Json:77, Packed:50, PackCompressed:8 

Processing with 96 rows per file
*** Json:7336, Packed:4747, PackCompressed:503, compr/json:14 
*** mean Bytes/TR: Json:76, Packed:49, PackCompressed:5 

Processing with 672 rows per file
*** Json:51120, Packed:32971, PackCompressed:3124, compr/json:16 
*** mean Bytes/TR: Json:76, Packed:49, PackCompressed:4 

Processing with 2880 rows per file
1000 ffd92b88-146b-11e2-8cd1-00163e856739 rtime: 2014-05-28 04:47:15+03 mtime: 2013-02-16 00:00:00+02
2000 ffd92b88-146b-11e2-8cd1-00163e856739 rtime: 2014-05-28 05:30:23+03 mtime: 2013-02-25 23:45:00+02
*** Json:220368, Packed:141015, PackCompressed:13468, compr/json:16 
*** mean Bytes/TR: Json:76, Packed:48, PackCompressed:4 

Processing with 8640 rows per file
1000 ffd92b88-146b-11e2-8cd1-00163e856739 rtime: 2014-05-28 04:47:15+03 mtime: 2013-02-16 00:00:00+02
2000 ffd92b88-146b-11e2-8cd1-00163e856739 rtime: 2014-05-28 05:30:23+03 mtime: 2013-02-25 23:45:00+02
3000 ffd92b88-146b-11e2-8cd1-00163e856739 rtime: 2014-05-28 06:16:44+03 mtime: 2013-03-09 09:30:00+02
4000 ffd92b88-146b-11e2-8cd1-00163e856739 rtime: 2014-05-28 07:23:34+03 mtime: 2013-03-19 10:15:00+02
5000 ffd92b88-146b-11e2-8cd1-00163e856739 rtime: 2014-05-28 08:26:11+03 mtime: 2013-03-29 20:15:00+02
6000 ffd92b88-146b-11e2-8cd1-00163e856739 rtime: 2014-05-28 09:24:09+03 mtime: 2013-04-09 03:45:00+03
7000 ffd92b88-146b-11e2-8cd1-00163e856739 rtime: 2014-05-28 11:31:13+03 mtime: 2013-04-19 10:30:00+03
8000 ffd92b88-146b-11e2-8cd1-00163e856739 rtime: 2014-05-28 11:34:07+03 mtime: 2013-04-29 21:15:00+03
*** Json:661528, Packed:422979, PackCompressed:40535, compr/json:16 
*** mean Bytes/TR: Json:76, Packed:48, PackCompressed:4 


LZ4 (probably buggy python implementation, but it gives an insight)
-------------------------------------------------------------------
root@vserver-dev-2:/opt/tsdbtest# ./compressiontest.py 
Processing with 1 rows per file
*** Json:116, Packed:90, PackCompressed:92, compr/json:1 
*** mean Bytes/TR: Json:116, Packed:90, PackCompressed:92 

Processing with 4 rows per file
*** Json:344, Packed:237, PackCompressed:115, compr/json:2 
*** mean Bytes/TR: Json:86, Packed:59, PackCompressed:28 

Processing with 24 rows per file
*** Json:1864, Packed:1219, PackCompressed:237, compr/json:7 
*** mean Bytes/TR: Json:77, Packed:50, PackCompressed:9 

Processing with 96 rows per file
*** Json:7336, Packed:4747, PackCompressed:689, compr/json:10 
*** mean Bytes/TR: Json:76, Packed:49, PackCompressed:7 

Processing with 672 rows per file
*** Json:51120, Packed:32971, PackCompressed:4660, compr/json:10 
*** mean Bytes/TR: Json:76, Packed:49, PackCompressed:6 

Processing with 2880 rows per file
1000 ffd92b88-146b-11e2-8cd1-00163e856739 rtime: 2014-05-28 04:47:15+03 mtime: 2013-02-16 00:00:00+02
Traceback (most recent call last):
  File "./compressiontest.py", line 90, in <module>
      prev_data=lz4.loads(prev_data)
	  ValueError: corrupt input at byte 9201
