CREATE TABLE mev_tsdb(
  measurement_source_id uuid NOT NULL,  -- 7 bytes
  measurement_start_date date NOT NULL, -- 4 bytes, 1day resolution
  granularity_days smallint, -- 2 bytes, days per oid
  dataformat smallint, -- 2 bytes, 0:uncompressed, 1:lz4, 2:zlib, x>=100:lz4-auto (lz4-auto: compress if packed size > x)
  dataoid oid default null, -- data LOB, msgpacked/canproto, compressed
  PRIMARY KEY (measurement_source_id,measurement_start_date)
);

-- also add columns:
-- islast: true if LOB in this holds the last measurement_value. All others: false.
