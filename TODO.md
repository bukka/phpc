# TODO list

## conversion
- phpc_convert_to_string_ex -> phpc_convert_to_string

## ZPP (phpc_parse_parameters)
- P, S (is it needed?)
- Z is not supported

## HashTable
- wrap util functions
  - zend_hash_merge
  - zend_hash_sort
  - zend_hash_compare
  - zend_hash_minmax
- add apply functions
  - zend_hash_apply
  - zend_hash_apply_with_argument
  - zend_hash_apply_with_arguments
- add possibility to duplicate key
  - zend_hash_get_current_key_ex

## Object
- Address all differences in [the DIFF list](DIFF.md)
