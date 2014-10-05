# TODO list

## Types
- long : zend_long -> phpc_long
- int : size_t -> phpc_strlen_t

## conversion
- phpc_convert_to_string_ex -> phpc_convert_to_string

## ZVAL 
- zval * : zval -> phpcval
- zval ** : zval* -> phpcpval
- PHPCVAL_TO_ZVAL(from, to), ZVAL_TO_PHPCVAL(from, to)
- PHPCPVAL_TO_PZVAL(from, to), PZVAL_TO_PHPPCVAL(from, to)
- MAKE_STD_ZVAL -> PHPC_MAKE_VAL (works phpcval and do nothing for PHP 7)
- Z_*_PP : Z_*_P -> PHPC_*
  - Z_TYPE_PP : Z_TYPE_P -> PHPC_TYPE
  - ...

## ZPP (phpc_parse_parameters)
- l => phpc_long
- P, S ?
- Z is not supported

## HashTable
- FOREACH macros
