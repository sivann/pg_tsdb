
-- instert or update data
-- ARGS: measuremet_source_id, measurement_time, value, quality, reception_time
CREATE or replace FUNCTION tsdb_insert (msid_arg uuid, mt_arg timestamp, value_arg varchar, quality_arg varchar, rt_arg timestamp)
  RETURNS void AS $$

DECLARE
	var_mday date; -- measurement day
	var_granularity_days smallint;
	var_dataformat smallint;
	var_dataoid oid;
	var_result TEXT;

BEGIN
	var_mday := mt_arg::date;
	-- RAISE NOTICE 'var_mday is %', var_mday;

-- check also this example syntax for speed: result := case when a < result then a else coalesce(result, a) end; 

	SELECT granularity_days, dataformat, dataoid INTO var_granularity_days, var_dataformat, var_dataoid FROM mev_tsdb WHERE mev_tsdb.measurement_source_id=msid_arg AND mev_tsdb.measurement_start_date=var_mday;

	IF NOT FOUND THEN -- create new row
		var_dataformat:=0; 			-- defaults
		var_granularity_days:=1;	-- defaults

		SELECT lo_create(0) INTO var_dataoid ; -- returns new oid
		INSERT INTO mev_tsdb (measurement_source_id, measurement_start_date, granularity_days, dataformat, dataoid) 
			VALUES (msid_arg,var_mday,var_granularity_days,var_dataformat,var_dataoid);
	END IF;
	-- do the actual data insert
	SELECT tsdb_write_lob(var_dataformat,var_granularity_days,var_dataoid, value_arg, quality_arg, mt_arg, rt_arg);

END
$$ LANGUAGE plpgsql;



--CREATE TYPE mevresult1 (value varchar, quality  varchar, rt timestamp);

-- instert or update data
-- ARGS: measuremet_source_id, measurement_time, value, quality, reception_time
CREATE or replace FUNCTION tsdb_select (msid_arg uuid, mt_arg timestamp,  
 OUT value varchar, OUT quality  varchar, OUT rt timestamp)
  --RETURNS mevresult1 
  AS $$


DECLARE
    var_mday date; -- measurement day
    var_granularity_days smallint;
    var_dataformat smallint;
    var_dataoid oid;
    var_result TEXT;

BEGIN
	var_mday := mt_arg::date;
    SELECT granularity_days, dataformat, dataoid INTO var_granularity_days, var_dataformat, var_dataoid FROM mev_tsdb WHERE mev_tsdb.measurement_source_id=msid_arg AND mev_tsdb.measurement_start_date=var_mday;

    IF NOT FOUND THEN
		value:=null;
		quality:=null;
		rt:=null;
	-- RAISE NOTICE 'not found msid_arg:%, var_mday:', msid_arg,var_mday;
		RETURN;
	END IF;
	SELECT * FROM tsdb_getval_fromlob(var_dataformat,var_granularity_days,var_dataoid); -- must probably return with same order as OUT vars


END
$$ LANGUAGE plpgsql; 

