SELECT token, char_length(token), round(bit_length(token)/8.0, 0) bytes
FROM all_substrings_set('abc審defggh!:w^&^*&^£審');

SELECT token FROM all_substrings_set('abc');

SELECT token FROM all_substrings_set('二湖楽');

SELECT token FROM all_substrings_set('二湖');

-- respects min characters in token setting
SELECT token FROM all_substrings_set('aa');
