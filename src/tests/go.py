import csv
import os
import os.path
import time
import msgpack
import sys
import datetime
import dateutil.parser
import dateutil.tz
import pytz
import zlib

datadir="zdatamsrc-month-all/"

with open('ifms_db_psql9_pv_test_mev.tsv', 'rb') as csvfile:
	reader = csv.reader(csvfile,delimiter='\t')
	rowno=0
	for row in reader:
		#parse row
		rowno+=1
		msid=row[1]
		mtime=row[2]
		rtime=row[3]
		mvalue=row[4]
		mqual=row[5]
		if rowno % 100 == 0:
			print rowno, msid, 'rtime:',rtime, 'mtime:',mtime


		mtime_timestamp=dateutil.parser.parse(mtime)
		mtime_month=mtime_timestamp.strftime("%Y-%m")
		mtime_day=mtime_timestamp.strftime("%Y-%m-%d")
		#mtime_time=mtime_timestamp.strftime("%H%M%S")
		mtime_time=mtime_timestamp.strftime("%d%H%M%S")
		#month=timestamp.strftime("%Y-%m")

		# parse rtime and convert to UTC (to avoid having +xxx tz notation)
		rtime_timestamp=dateutil.parser.parse(rtime)
		rtime_timestamp_utc=rtime_timestamp.astimezone(tz=pytz.utc)
		rtime_date=rtime_timestamp_utc.strftime("%Y%m%d%H%M%S")

		#row_dict={'m':msid,'v':mvalue,'r':rtime_date, 't':mtime_time,'q':mqual}
		row_dict={'v':mvalue,'r':rtime_date, 't':mtime_time,'q':mqual}

		#print row_dict

		# read file
		outfname=msid+'-'+mtime_month
		try:
			f=open(datadir+outfname,"r+")
		except IOError as e:
			# if not exists create file
			f=open(datadir+outfname,"w+")

		f.seek(0)
		prev_data=f.read()
		if len(prev_data) > 2 :
			prev_data=zlib.decompress(prev_data)
			prev_data_unpacked=msgpack.unpackb(prev_data,use_list=True);
		else:
			prev_data_unpacked=list()

		#print 'Prev Unpacked:',prev_data_unpacked,'\n'
		#print 'New Unpacked:',prev_data_unpacked,'\n'
		prev_data_unpacked.append(row_dict)
		packed_new=msgpack.packb(prev_data_unpacked)

		f.seek(0)
		f.write(zlib.compress(packed_new))
		f.close()
		#if rowno == 10000000:
		#	sys.exit(0)



#fout=open("","a+")
#foud.write(xxx)


