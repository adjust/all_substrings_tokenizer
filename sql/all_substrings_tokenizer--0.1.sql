
CREATE TYPE __substrings_token AS (token text);

CREATE FUNCTION all_substrings_set(IN text)
RETURNS SETOF __substrings_token
AS '$libdir/all_substrings_tokenizer.so'
LANGUAGE C IMMUTABLE STRICT;
