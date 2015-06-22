# pg_tsdb

PostgreSQL function to support writing timeseries with size efficiency.
Uses:
* 1 LOB per week or per day per measurement id
* LZ4 compression
* lazy compress (compress only on last event(s))
* msgpack

This is by no means finished.
