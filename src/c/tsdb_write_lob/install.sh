#!/bin/bash

set -e

make
make install

echo ""
/etc/init.d/postgresql restart  # restart pg to install new version

#DROP function
#dropcmd=`grep CREATE /usr/share/postgresql/9.3/contrib/tsdb_write_lob.sql |sed -e 's/CREATE/DROP/'`
funcargs=`su - postgres -c "echo '\df'|psql ifms_db_psql9_pv_test "|grep tsdb_write_lob|cut -d'|' -f4`
dropcmd="DROP function tsdb_write_lob (${funcargs})"
su - postgres -c "psql ifms_db_psql9_pv_test -c '$dropcmd'"

#Create function
su - postgres -c 'psql ifms_db_psql9_pv_test -f /usr/share/postgresql/9.3/contrib/tsdb_write_lob.sql'

