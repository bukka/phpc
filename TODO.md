# TODO list

## conversion
- phpc_convert_to_string_ex -> phpc_convert_to_string

## ZVAL 
- MAKE_STD_ZVAL -> PHPC_MAKE_VAL (works phpcval and do nothing for PHP 7)

## ZPP (phpc_parse_parameters)
- P, S (is it needed?)
- Z is not supported

## HashTable
- fix possible overflow for key length (cast from `uint` to `int`)
- add possibility to duplicate key (`zend_hash_get_current_key_ex`)

## Object

### Init
- Add macros for differences defined in [the DIFF list](DIFF.md)
