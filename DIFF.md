# Difference between PHP 5 and PHP 7

## Object

### Class and handler callbacks
- extra handlers in 7
  - `handlers.offset = XtOffsetOf(php_object_struct, std)`
  - `handlers.free_obj = object_free_storage_function

#### create_object_ex (ex method for clone and new)
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

#### create_object
- return
  - 5: `zend_object_value  (retval)`
  - 7: `zend_object *      (&intern->std)`
- 2nd argument
  - 5: NULL (intern reference)
  - 7: 1    (whether to init props)