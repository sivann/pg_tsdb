CREATE FUNCTION add(int, int)
RETURNS int
as '$libdir/add_func' , 'add_ab' 
LANGUAGE C STRICT;
