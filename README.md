#### All Substrings Tokenizer

This is a PostgreSQL extension that provides functions to extract all substrings
from a given string. Currently there's a hard coded limit of 3 characters per
substring.

It's also worth as an example for writing set-returning functions in C and handling multi-byte characters.

Example usage:

    SELECT token FROM all_substrings_set('1二湖楽a');

      token
    ----------
    1二湖
    1二湖楽
    1二湖楽a
    二湖楽
    二湖楽a
    湖楽a
    (6 rows)

