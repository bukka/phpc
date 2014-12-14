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
- add macros for extra handler in 7
  - `handlers.offset = XtOffsetOf(php_object_struct, std)`
  - `handlers.free_obj = object_free_storage_function`

### Class and handler callbacks

#### create_object (ex method for clone and new)
- return
  - 5: `zend_object_value  (retval)`
  - 7: `zend_object *      (&intern->std)`
- intern
  - 5: `calloc(1, sizeof(php_object_struct))`
  - 7: `ecalloc(1, sizeof(php_object_struct) + sizeof(zval) * (class_type->default_properties_count - 1))`
- init props
  - 5: always
  - 7: only for new object (not for clone)
- save new intern
  - 5: for clone
  - 7: never (it's taken from offset)
- handlers
  - 5: `retval.handlers`
  - 7: `intern->std.handlers`
- object store
  - 5: `zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) object_free_storage_function, NULL TSRMLS_CC)`
  - 7: none (it's set in init)
