# TODO list

## conversion
- phpc_convert_to_string_ex -> phpc_convert_to_string

## ZVAL 
- MAKE_STD_ZVAL -> PHPC_MAKE_VAL (works phpcval and do nothing for PHP 7)
- Z_*_PP : Z_*_P -> PHPC_*
  - Z_TYPE_PP : Z_TYPE_P -> PHPC_TYPE
  - ...

## ZPP (phpc_parse_parameters)
- P, S (is it needed?)
- Z is not supported

## HashTable
- FOREACH macros
