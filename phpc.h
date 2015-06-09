/*
 * Copyright (c) 2014 Jakub Zelenka. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */


#ifndef PHPC_H
#define	PHPC_H

#if PHP_VERSION_ID < 50199
#error "Only PHP 5.2+ supported"
#endif

/* no operation */
#define PHPC_NOOP ((void) 0)

/* COMMON (used definitions) */

/* common object macros */
#define PHPC_CLASS_TYPE _phpc_class_type
#define PHPC_OBJ_STRUCT_NAME(_name) struct _phpc_##_name##__obj
#define PHPC_OBJ_STRUCT_DECLARE(_name, _ptr) PHPC_OBJ_STRUCT_NAME(_name) *_ptr
#define PHPC_OBJ_GET_HANDLER_FCE(_name, _type) _name##__##_type
#define PHPC_OBJ_DEFINE_HANDLER_FCE(_rtype, _name, _type) \
	static _rtype PHPC_OBJ_GET_HANDLER_FCE(_name, _type)
#define PHPC_OBJ_GET_HANDLER_FCE_INLINE_DEF(_rtype, _name, _type) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(inline _rtype, _name, _type)
#define PHPC_OBJ_GET_HANDLER_VAR_NAME(_name) _phpc_##_name##__handlers
#define PHPC_OBJ_DEFINE_HANDLER_VAR(_name) \
	static zend_object_handlers PHPC_OBJ_GET_HANDLER_VAR_NAME(_name)
#define PHPC_OBJ_INIT_HANDLERS(_name) \
	memcpy(&PHPC_OBJ_GET_HANDLER_VAR_NAME(_name), \
		zend_get_std_object_handlers(), sizeof(zend_object_handlers))
#define PHPC_CLASS_SET_HANDLER_CREATE(_class_entry, _name) \
	_class_entry.create_object = PHPC_OBJ_GET_HANDLER_FCE(_name, create)
#define PHPC_OBJ_SET_HANDLER_CLONE(_name) \
	PHPC_OBJ_GET_HANDLER_VAR_NAME(_name).clone_obj = PHPC_OBJ_GET_HANDLER_FCE(_name, clone)
#define PHPC_OBJ_SET_HANDLER_COMPARE(_name) \
	PHPC_OBJ_GET_HANDLER_VAR_NAME(_name).compare_objects = PHPC_OBJ_GET_HANDLER_FCE(_name, compare)


#if PHP_MAJOR_VERSION == 5

/* INT */
typedef long  phpc_long_t;
typedef ulong phpc_ulong_t;
typedef off_t phpc_off_t;

#define PHPC_LONG_TO_LONG_EX2(_plv, _lv, _exc_over, _exc_under) _lv = _plv
#define PHPC_LONG_TO_LONG_EX(_plv, _lv, _exc) PHPC_LONG_TO_LONG_EX2(_plv, _lv, _exc, _exc)
#define PHPC_LONG_TO_LONG(_plv, _lv) PHPC_LONG_TO_LONG_EX2(_plv, _lv, _lv = LONG_MAX, _lv = LONG_MIN)


/* STRING */
/* length type */
typedef int  phpc_str_size_t;
/* accessor and convertor macros */
#define PHPC_STR_VAL(_name)                   _name##__val
#define PHPC_STR_LEN(_name)                   _name##__len
#define PHPC_STR_LEN_FMT                      "d"
#define PHPC_STR_LEN_UNUSED(_name)            (void) PHPC_STR_LEN(_name)
#define PHPC_STR_LEN_FROM_VAL(_name)          strlen(PHPC_STR_VAL(_name))
#define PHPC_STR_LEN_DECLARE(_name)           int PHPC_STR_LEN(_name)
#define PHPC_STR_LEN_FETCH(_name)             PHPC_STR_LEN(_name) = PHPC_STR_LEN_FROM_VAL(_name)
#define PHPC_STR_LEN_DECLARE_AND_FETCH(_name) int PHPC_STR_LEN_FETCH(_name)
#define PHPC_STR_EXISTS(_name)                PHPC_STR_VAL(_name)
#define PHPC_STR_DECLARE(_name)               char *PHPC_STR_VAL(_name); int PHPC_STR_LEN(_name)
#define PHPC_STR_ARG(_name)                   char *PHPC_STR_VAL(_name), int PHPC_STR_LEN(_name)
#define PHPC_STR_ARG_VAL(_name)               char *PHPC_STR_VAL(_name)
#define PHPC_STR_ARG_PTR(_name)               char **PHPC_STR_VAL(_name), int *PHPC_STR_LEN(_name)
#define PHPC_STR_ARG_PTR_VAL(_name)           char **PHPC_STR_VAL(_name)
#define PHPC_STR_PASS(_name)                  PHPC_STR_VAL(_name), PHPC_STR_LEN(_name)
#define PHPC_STR_PASS_VAL(_name)              PHPC_STR_VAL(_name)
#define PHPC_STR_PASS_PTR(_name)              &PHPC_STR_VAL(_name), &PHPC_STR_LEN(_name)
#define PHPC_STR_PASS_PTR_VAL(_name)          &PHPC_STR_VAL(_name)
#define PHPC_STR_FROM_PTR_STR(_str, _strp) \
	PHPC_STR_VAL(_str) = *PHPC_STR_VAL(_strp); \
	PHPC_STR_LEN(_str) = *PHPC_STR_LEN(_strp)
#define PHPC_STR_FROM_PTR_VAL(_str, _strpv) \
	PHPC_STR_VAL(_str) = *PHPC_STR_VAL(_strpv); \
	PHPC_STR_LEN(_str) = strlen(*PHPC_STR_VAL(_strpv))
#define PHPC_STR_RETURN(_name) \
	RETURN_STRINGL(PHPC_STR_VAL(_name), PHPC_STR_LEN(_name), 0)

/* wrapper macros */
#define PHPC_STR_INIT(_name, _cstr, _len) do { \
		PHPC_STR_VAL(_name) = emalloc(_len + 1); \
		memcpy(PHPC_STR_VAL(_name), _cstr, _len); \
		PHPC_STR_VAL(_name)[_len] = 0; \
		PHPC_STR_LEN(_name) = _len; \
	} while (0)
#define PHPC_STR_ALLOC(_name, _len) do { \
		PHPC_STR_VAL(_name) = emalloc(_len + 1); \
		PHPC_STR_LEN(_name) = _len; \
	} while (0)
#define PHPC_STR_REALLOC(_name, _len) do { \
		PHPC_STR_VAL(_name) = erealloc(PHPC_STR_VAL(_name), _len + 1); \
		PHPC_STR_LEN(_name) = _len; \
	} while (0)
#define PHPC_STR_RELEASE(_name) efree(PHPC_STR_VAL(_name))

/* C string */
#define PHPC_CSTRL_RETURN(_name, _len) RETURN_STRINGL(_name, _len, 1)
#define PHPC_CSTR_RETURN(_name) RETURN_STRING(_name, 1)

/* ZPP path flag */
#if PHP_VERSION_ID < 50399
#define PHPC_ZPP_PATH_FLAG "s"
#else
#define PHPC_ZPP_PATH_FLAG "p"
#endif

/* Smart string */
#if defined(PHPC_SMART_STR_INCLUDE) || defined(PHPC_SMART_CSTR_INCLUDE)
#include "ext/standard/php_smart_str.h"

#ifdef PHPC_SMART_CSTR_INCLUDE
/* smart_str for C string has been renamed in PHP 7 so we have to wrap it */
#define phpc_smart_cstr                 smart_str
#define phpc_smart_cstr_alloc           smart_str_alloc
#define phpc_smart_cstr_free            smart_str_free
#define phpc_smart_cstr_append          smart_str_append
#define phpc_smart_cstr_appends         smart_str_appends
#define phpc_smart_cstr_appendl         smart_str_appendl
#define phpc_smart_cstr_appendc         smart_str_appendc
#define phpc_smart_cstr_append_long     smart_str_append_long
#define phpc_smart_cstr_append_unsigned smart_str_append_unsigned
#define phpc_smart_cstr_0               smart_str_0
#endif /* PHPC_SMART_CSTR_INCLUDE */

#endif /* PHPC_SMART_STR_INCLUDE || PHPC_SMART_CSTR_INCLUDE */


/* OBJECT */
#define PHPC_CLASS_REGISTER_EX(_orig_class_entry, _parent_ce, _parent_name) \
	zend_register_internal_class_ex(&_orig_class_entry, _parent_ce, _parent_name TSRMLS_CC)
#define PHPC_CLASS_REGISTER(_orig_class_entry) \
	zend_register_internal_class(&_orig_class_entry TSRMLS_CC)

/* struct spec */
#define PHPC_OBJ_STRUCT_MEMBER_FIRST zend_object std;
#define PHPC_OBJ_STRUCT_MEMBER_LAST
#define PHPC_OBJ_STRUCT_BEGIN(_name) \
	PHPC_OBJ_STRUCT_NAME(_name) { PHPC_OBJ_STRUCT_MEMBER_FIRST
#define PHPC_OBJ_STRUCT_END() \
	PHPC_OBJ_STRUCT_MEMBER_LAST };

/* object convertors */
#define PHPC_OBJ_FROM_ZOBJ(_name, _object) \
	(PHPC_OBJ_STRUCT_NAME(_name) *) _object
#define PHPC_OBJ_FROM_ZVAL(_name, _zv) \
	(PHPC_OBJ_STRUCT_NAME(_name) *) zend_object_store_get_object(_zv TSRMLS_CC)
#define PHPC_OBJ_FROM_SELF(_name) \
	PHPC_OBJ_FROM_ZVAL(_name, _phpc_self)
#define PHP_OBJ_GET_HANDLER_OBJ_FROM_ZOBJ(_name) \
	PHPC_OBJ_FROM_ZOBJ(_name, _phpc_object)

/* create_ex object handler helper */
#define PHPC_OBJ_HANDLER_CREATE_EX(_name) \
	PHPC_OBJ_GET_HANDLER_FCE_INLINE_DEF(zend_object_value, _name, create_ex) \
	(zend_class_entry *PHPC_CLASS_TYPE, PHPC_OBJ_STRUCT_NAME(_name) **_phpc_objptr TSRMLS_DC)
#define PHPC_OBJ_HANDLER_CREATE_EX_DECLARE() zend_object_value _phpc_retval
#define PHPC_OBJ_HANDLER_CREATE_EX_ALLOC(_name) \
	 ecalloc(1, sizeof(PHPC_OBJ_STRUCT_NAME(_name)));
#define PHPC_OBJ_HANDLER_INIT_CREATE_EX_PROPS(_intern) \
	do { \
		if (_phpc_objptr) { \
			*_phpc_objptr = _intern; \
		} \
		zend_object_std_init(&_intern->std, PHPC_CLASS_TYPE TSRMLS_CC); \
		object_properties_init(&_intern->std, PHPC_CLASS_TYPE); \
	} while(0)
#define PHPC_OBJ_HANDLER_CREATE_EX_RETURN_EX(_name, _intern) \
	do { \
		_phpc_retval.handle = zend_objects_store_put((_intern), \
			(zend_objects_store_dtor_t) zend_objects_destroy_object, \
			(zend_objects_free_object_storage_t) PHPC_OBJ_GET_HANDLER_FCE(_name, free), \
			NULL TSRMLS_CC); \
		_phpc_retval.handlers = &PHPC_OBJ_GET_HANDLER_VAR_NAME(_name); \
		return _phpc_retval; \
	} while(0)

/* create object handler */
#define PHPC_OBJ_HANDLER_CREATE(_name) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(zend_object_value, _name, create) \
	(zend_class_entry *PHPC_CLASS_TYPE TSRMLS_DC)
#define PHPC_OBJ_HANDLER_CREATE_RETURN(_name) \
	return PHPC_OBJ_GET_HANDLER_FCE(_name, create_ex)(PHPC_CLASS_TYPE, NULL TSRMLS_CC)

/* clone object handler */
#define PHPC_OBJ_HANDLER_CLONE(_name) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(zend_object_value, _name, clone)(zval *_phpc_self TSRMLS_DC)
#define PHPC_OBJ_HANDLER_CLONE_DECLARE() zend_object_value _phpc_retval
#define PHPC_OBJ_HANDLER_CLONE_MEMBERS(_name, _new_obj, _old_obj) \
	do { \
		_phpc_retval = PHPC_OBJ_GET_HANDLER_FCE(_name, create_ex)(_old_obj->std.ce, &_new_obj TSRMLS_CC); \
		zend_objects_clone_members(&_new_obj->std, _phpc_retval, &_old_obj->std, Z_OBJ_HANDLE_P(_phpc_self) TSRMLS_CC); \
	} while(0)
#define PHPC_OBJ_HANDLER_CLONE_RETURN_EX(_new_obj) return _phpc_retval

/* free object handler */
#define PHPC_OBJ_HANDLER_FREE(_name) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(void, _name, free)(void *_phpc_object TSRMLS_DC)
#define PHPC_OBJ_HANDLER_FREE_DTOR(_intern) \
	do { \
		zend_object_std_dtor(&(_intern)->std TSRMLS_CC); \
		efree(_intern); \
	} while(0)

/* compare object handler */
#define PHPC_OBJ_HANDLER_COMPARE(_name) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(int, _name, compare)(zval *_phpc_obj1, zval *_phpc_obj2 TSRMLS_DC)
#define PHPC_OBJ_HANDLER_COMPARE_FETCH(_name, _id, _obj) \
	PHPC_OBJ_STRUCT_DECLARE(_name, _obj) = PHPC_OBJ_FROM_ZVAL(_name, _phpc_obj ## _id)

/* handler setters */
#define PHPC_OBJ_SET_HANDLER_OFFSET(_name) PHPC_NOOP
#define PHPC_OBJ_SET_HANDLER_FREE(_name) PHPC_NOOP

/* read propery */
#define PHPC_READ_PROPERTY_RV_NAME
#define PHPC_READ_PROPERTY_RV_DECLARE PHPC_NOOP
#define PHPC_READ_PROPERTY(_scope, _object, _name, _name_len, _silent) \
	zend_read_property(_scope, _object, _name, _name_len, _silent TSRMLS_CC)

/* HASH */

/* update */
#define PHPC_HASH_INDEX_UPDATE(_ht, _idx, _pzv) \
	zend_hash_index_update(_ht, _idx, &_pzv, sizeof(_pzv), NULL)
#define PHPC_HASH_STR_UPDATE(_ht, _str, _pzv) \
	zend_hash_update(_ht, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str) + 1, &_pzv, sizeof(_pzv), NULL)
#define PHPC_HASH_CSTRL_UPDATE(_ht, _cstr_value, _cstr_len, _pzv) \
	zend_hash_update(_ht, _cstr_value, (_cstr_len) + 1, &_pzv, sizeof(_pzv), NULL)
#define PHPC_HASH_CSTR_UPDATE(_ht, _cstr_value, _pzv) \
	zend_hash_update(_ht, _cstr_value, strlen(_cstr_value) + 1, &_pzv, sizeof(_pzv), NULL)

/* delete */
#define PHPC_HASH_INDEX_DELETE(_ht, _idx) \
	zend_hash_index_del(_ht, _idx)
#define PHPC_HASH_STR_DELETE(_ht, _str) \
	 zend_hash_del(_ht, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str) + 1)
#define PHPC_HASH_CSTRL_DELETE(_ht, _cstr_value, _cstr_len) \
	zend_hash_del(_ht, _cstr_value, (_cstr_len) + 1)
#define PHPC_HASH_CSTR_DELETE(_ht, _cstr_value) \
	zend_hash_del(_ht, _cstr_value, strlen(_cstr_value) + 1)

/* exists */
#define PHPC_HASH_INDEX_EXISTS(_ht, _idx) \
	zend_hash_index_exists(_ht, _idx)
#define PHPC_HASH_STR_EXISTS(_ht, _str) \
	 zend_hash_exists(_ht, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str) + 1)
#define PHPC_HASH_CSTRL_EXISTS(_ht, _cstr_value, _cstr_len) \
	zend_hash_exists(_ht, _cstr_value, (_cstr_len) + 1)
#define PHPC_HASH_CSTR_EXISTS(_ht, _cstr_value) \
	zend_hash_exists(_ht, _cstr_value, strlen(_cstr_value) + 1)

/* find */
#define PHPC_HASH_INDEX_FIND(_ht, _idx, _ppv) \
	zend_hash_index_find(_ht, _idx, (void **) &_ppv)
#define PHPC_HASH_STR_FIND(_ht, _str, _ppv) \
	 zend_hash_find(_ht, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str) + 1, (void **) &_ppv)
#define PHPC_HASH_CSTRL_FIND(_ht, _cstr_value, _cstr_len, _ppv) \
	zend_hash_find(_ht, _cstr_value, (_cstr_len) + 1, (void **) &_ppv)
#define PHPC_HASH_CSTR_FIND(_ht, _cstr_value, _ppv) \
	zend_hash_find(_ht, _cstr_value, strlen(_cstr_value) + 1, (void **) &_ppv)

/* key and data getter */
#define PHPC_HASH_GET_CURRENT_KEY_EX(_ht, _str, _num_index, _pos) \
	zend_hash_get_current_key_ex(_ht, &PHPC_STR_VAL(_str), &PHPC_STR_LEN(_str), &_num_index, 0, _pos)
#define PHPC_HASH_GET_CURRENT_DATA_EX(_ht, _val, _pos) \
	zend_hash_get_current_data_ex(_ht, (void **) &(_val), _pos)

#define PHPC_HASH_GET_CURRENT_KEY(_ht, _str, _num_index) \
	zend_hash_get_current_key(_ht, &PHPC_STR_VAL(_str), &_num_index, 0)
#define PHPC_HASH_GET_CURRENT_DATA(_ht, _val) \
	zend_hash_get_current_data(_ht, (void **) &(_val))

/* iteration for each element */
#define PHPC_HASH_FOREACH_VAL(_ht, _ppv) do { \
	HashPosition _pos; \
	for (zend_hash_internal_pointer_reset_ex((_ht), &_pos); \
			zend_hash_get_current_data_ex((_ht), (void **) &(_ppv), &_pos) == SUCCESS; \
			zend_hash_move_forward_ex((_ht), &_pos) ) {

#define _PHPC_HASH_FOREACH_KEY_VAL(_ht, _ph, _key, _ppv, _use_h) \
	PHPC_HASH_FOREACH_VAL(_ht, _ppv) \
		uint _str_length; \
		ulong _num_index, *_pnum_index; \
		if (_use_h) { \
			_pnum_index = _ph; \
		} else { \
			_pnum_index = &_num_index; \
		} \
		int _key_type = zend_hash_get_current_key_ex(_ht, &PHPC_STR_VAL(_key), &_str_length, _pnum_index, 0, &_pos); \
		if (_key_type == HASH_KEY_IS_STRING) { \
			PHPC_STR_LEN(_key) = (int) _str_length; \
		} else { \
			PHPC_STR_VAL(_key) = NULL; \
			PHPC_STR_LEN(_key) = 0; \
		}

#define PHPC_HASH_FOREACH_KEY_VAL(_ht, _h, _key, _ppv) \
	_PHPC_HASH_FOREACH_KEY_VAL(_ht, &_h, _key, _ppv, 1)

#define PHPC_HASH_FOREACH_STR_KEY_VAL(_ht, _key, _ppv) \
	_PHPC_HASH_FOREACH_KEY_VAL(_ht, NULL, _key, _ppv, 0)

#define PHPC_HASH_FOREACH_END() } } while (0)

/* hash to zval */
#define PHPC_HASH_PZVAL(_ht, _pzv) \
	do { \
		Z_ARRVAL_P(_pzv) = _ht; \
		Z_TYPE_P(_pzv) = IS_ARRAY; \
	} while(0)
#define PHPC_HASH_RETVAL(_ht) \
	PHPC_HASH_PZVAL(_ht, return_value)
#define PHPC_HASH_RETURN(_ht) \
	do { \
		PHPC_HASH_RETVAL(_ht); \
		return; \
	} while(0)


/* ARRAY */
#define PHPC_ARRAY_ADD_ASSOC_NULL_EX(_arr, _key, _key_len) \
    add_assoc_null_ex(_arr, _key, (_key_len) + 1)
#define PHPC_ARRAY_ADD_ASSOC_BOOL_EX(_arr, _key, _key_len, _b) \
    add_assoc_bool_ex(_arr, _key, (_key_len) + 1, _b)
#define PHPC_ARRAY_ADD_ASSOC_LONG_EX(_arr, _key, _key_len, _n) \
    add_assoc_long_ex(_arr, _key, (_key_len) + 1, _n)
#define PHPC_ARRAY_ADD_ASSOC_RESOURCE_EX(_arr, _key, _key_len, _r) \
add_assoc_resource_ex(_arr, _key, (_key_len) + 1, _r)
#define PHPC_ARRAY_ADD_ASSOC_DOUBLE_EX(_arr, _key, _key_len, _d) \
	add_assoc_double_ex(_arr, _key, (_key_len) + 1, _d)
#define PHPC_ARRAY_ADD_ASSOC_STR_EX(_arr, _key, _key_len, _str) \
	add_assoc_stringl_ex(_arr, _key, (_key_len) + 1, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str), 0)
#define PHPC_ARRAY_ADD_ASSOC_CSTR_EX(_arr, _key, _key_len, _cstr) \
	add_assoc_string_ex(_arr, _key, (_key_len) + 1, _cstr, 1)
#define PHPC_ARRAY_ADD_ASSOC_CSTRL_EX(_arr, _key, _key_len, _cstr, _cstr_len) \
	add_assoc_stringl_ex(_arr, _key, (_key_len) + 1, _cstr, _cstr_len, 1)
#define PHPC_ARRAY_ADD_ASSOC_VAL_EX(_arr, _key, _key_len, _pv) \
	add_assoc_zval_ex(_arr, _key, (_key_len) + 1, _pv)

#define PHPC_ARRAY_ADD_ASSOC_STR(_arr, _key, _str) \
	add_assoc_stringl(_arr, _key, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str), 0)
#define PHPC_ARRAY_ADD_ASSOC_CSTR(_arr, _key, _cstr) \
	add_assoc_string(_arr, _key, _cstr, 1)
#define PHPC_ARRAY_ADD_ASSOC_CSTRL(_arr, _key, _cstr, _cstr_len) \
	add_assoc_stringl(_arr, _key, _cstr, _cstr_len, 1)
#define PHPC_ARRAY_ADD_ASSOC_VAL      add_assoc_zval

#define PHPC_ARRAY_ADD_INDEX_STR(_arr, _idx, _str) \
	add_index_stringl(_arr, _idx, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str), 0)
#define PHPC_ARRAY_ADD_INDEX_CSTR(_arr, _idx, _cstr) \
	add_index_string(_arr, _idx, _cstr, 1)
#define PHPC_ARRAY_ADD_INDEX_CSTRL(_arr, _idx, _cstr, _cstr_len) \
	add_index_stringl(_arr, _idx, _cstr, _cstr_len, 1)
#define PHPC_ARRAY_ADD_INDEX_VAL      add_index_zval

#define PHPC_ARRAY_ADD_NEXT_INDEX_STR(_arr, _str) \
	add_next_index_stringl(_arr, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str), 0)
#define PHPC_ARRAY_ADD_NEXT_INDEX_CSTR(_arr, _cstr) \
	add_next_index_string(_arr, _cstr, 1)
#define PHPC_ARRAY_ADD_NEXT_INDEX_CSTRL(_arr, _cstr, _cstr_len) \
	add_next_index_stringl(_arr, _cstr, _cstr_len, 1)
#define PHPC_ARRAY_ADD_NEXT_INDEX_VAL add_next_index_zval


/* ZVAL */
typedef zval * phpc_val;

#define PHPC_TYPE        Z_TYPE_P
#define PHPC_TYPE_P      Z_TYPE_PP

#define PHPC_LVAL        Z_LVAL_P
#define PHPC_LVAL_P      Z_LVAL_PP

#define PHPC_DVAL        Z_DVAL_P
#define PHPC_DVAL_P      Z_DVAL_PP

#define PHPC_STRVAL      Z_STRVAL_P
#define PHPC_STRVAL_P    Z_STRVAL_PP

#define PHPC_STRLEN      Z_STRLEN_P
#define PHPC_STRLEN_P    Z_STRLEN_PP

#define PHPC_ARRVAL      Z_ARRVAL_P
#define PHPC_ARRVAL_P    Z_ARRVAL_PP

#define PHPC_VAL_TO_ZVAL(_pv, _zv) _zv = *(_pv)
#define PHPC_VAL_TO_PZVAL(_pv, _zv) _zv = _pv
#define PHPC_PVAL_TO_PZVAL(_pv, _zv) _zv = *(_pv)

#define PHPC_VAL_MAKE MAKE_STD_ZVAL

#define PHPC_VAL_STR(_pv, _str) \
	ZVAL_STRINGL(_pv, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str), 0)
#define PHPC_VAL_CSTR(_pv, _cstr) \
    ZVAL_STRING(_pv, _cstr, 1)
#define PHPC_VAL_CSTRL(_pv, _cstr, _cstr_len) \
	ZVAL_STRINGL(_pv, _cstr, _cstr_len, 1)


/* Function end */
#if (PHP_MINOR_VERSION == 3 && PHP_RELEASE_VERSION >= 7) || (PHP_MINOR_VERSION >= 4)
#define PHPC_FE_END PHP_FE_END
#else
#define PHPC_FE_END {NULL,NULL,NULL}
#endif


#else /* PHP 7 */

/* INT */
typedef zend_long  phpc_long_t;
typedef zend_ulong phpc_ulong_t;
typedef zend_off_t phpc_off_t;

#define PHPC_LONG_TO_LONG_EX2(_plv, _lv, _exc_over, _exc_under) \
	if (_lv > LONG_MAX) { \
		_exc_over; \
	} else if (_lv < LONG_MIN) { \
		_exc_under; \
	} \
	_lv = (long) _plv
#define PHPC_LONG_TO_LONG_EX(_plv, _lv, _exc) PHPC_LONG_TO_LONG_EX2(_plv, _lv, _exc, _exc)
#define PHPC_LONG_TO_LONG(_plv, _lv) PHPC_LONG_TO_LONG_EX2(_plv, _lv, _lv = LONG_MAX, _lv = LONG_MIN)


/* STRING */
/* length type */
typedef size_t    phpc_str_size_t;
/* accessor and convertor macros */
#define PHPC_STR_VAL(_name)                   (_name)->val
#define PHPC_STR_LEN(_name)                   (_name)->len
#define PHPC_STR_LEN_FMT                      "zu"
#define PHPC_STR_LEN_UNUSED(_name)            PHPC_NOOP
#define PHPC_STR_LEN_FROM_VAL                 PHPC_STR_LEN
#define PHPC_STR_LEN_DECLARE(_name)           PHPC_NOOP
#define PHPC_STR_LEN_FETCH(_name)             PHPC_NOOP
#define PHPC_STR_LEN_DECLARE_AND_FETCH(_name) PHPC_NOOP
#define PHPC_STR_EXISTS(_name)                (_name)
#define PHPC_STR_DECLARE(_name)               zend_string *_name
#define PHPC_STR_ARG(_name)                   zend_string *_name
#define PHPC_STR_ARG_VAL                      PHPC_STR_ARG
#define PHPC_STR_ARG_PTR(_name)               zend_string **_name
#define PHPC_STR_ARG_PTR_VAL                  PHPC_STR_ARG_PTR
#define PHPC_STR_PASS(_name)                  _name
#define PHPC_STR_PASS_VAL                     PHPC_STR_PASS
#define PHPC_STR_PASS_PTR(_name)              &_name
#define PHPC_STR_PASS_PTR_VAL                 PHPC_STR_PASS_PTR
#define PHPC_STR_FROM_PTR_STR(_str, _strp)    _str = *_strp
#define PHPC_STR_FROM_PTR_VAL(_str, _strpv)   _str = *_strpv
#define PHPC_STR_RETURN                       RETURN_STR
/* wrapper macros */

#define PHPC_STR_INIT(_name, _cstr, _len) _name = zend_string_init(_cstr, _len, 0)
#define PHPC_STR_ALLOC(_name, _len) _name = zend_string_alloc(_len, 0)
#define PHPC_STR_REALLOC(_name, _len) _name = zend_string_realloc(_name, _len, 0)
#define PHPC_STR_RELEASE(_name) zend_string_release(_name)

/* C string */
#define PHPC_CSTRL_RETURN(_name, _len) RETURN_STRINGL(_name, _len)
#define PHPC_CSTR_RETURN(_name) RETURN_STRING(_name)

/* ZPP path flag */
#define PHPC_PATH_ZPP_FLAG "p"

/* Smart string */
#ifdef PHPC_SMART_STR_INCLUDE
#include "zend_smart_str.h"
#endif /* PHPC_SMART_STR_INCLUDE */

#ifdef PHPC_SMART_CSTR_INCLUDE
#include "ext/standard/php_smart_string.h"
/* smart_str for C string has been renamed in PHP 7 so we have to wrap it */
#define phpc_smart_cstr                 smart_string
#define phpc_smart_cstr_alloc           smart_string_alloc
#define phpc_smart_cstr_free            smart_string_free
#define phpc_smart_cstr_append          smart_string_append
#define phpc_smart_cstr_appends         smart_string_appends
#define phpc_smart_cstr_appendl         smart_string_appendl
#define phpc_smart_cstr_appendc         smart_string_appendc
#define phpc_smart_cstr_append_long     smart_string_append_long
#define phpc_smart_cstr_append_unsigned smart_string_append_unsigned
#define phpc_smart_cstr_0               smart_string_0
#endif /* PHPC_SMART_CSTR_INCLUDE */


/* OBJECT */
#define PHPC_CLASS_REGISTER_EX(_orig_class_entry, _parent_ce, _parent_name) \
	zend_register_internal_class_ex(&_orig_class_entry, _parent_ce)
#define PHPC_CLASS_REGISTER(_orig_class_entry) \
	zend_register_internal_class(&_orig_class_entry)

/* struct spec */
#define PHPC_OBJ_STRUCT_MEMBER_FIRST
#define PHPC_OBJ_STRUCT_MEMBER_LAST zend_object std;
#define PHPC_OBJ_STRUCT_BEGIN(_name) \
	PHPC_OBJ_STRUCT_NAME(_name) { PHPC_OBJ_STRUCT_MEMBER_FIRST
#define PHPC_OBJ_STRUCT_END() \
	PHPC_OBJ_STRUCT_MEMBER_LAST };

#define PHPC_OBJ_FROM_ZOBJ(_name, _object) \
	(PHPC_OBJ_STRUCT_NAME(_name) *)((char*)(_object) - XtOffsetOf(PHPC_OBJ_STRUCT_NAME(_name), std))
#define PHPC_OBJ_FROM_ZVAL(_name, _zv) \
	PHPC_OBJ_FROM_ZOBJ(_name, Z_OBJ_P(_zv))
#define PHPC_OBJ_FROM_SELF(_name) \
	PHPC_OBJ_FROM_ZVAL(_name, _phpc_self)
#define PHP_OBJ_GET_HANDLER_OBJ_FROM_ZOBJ(_name) \
	PHPC_OBJ_FROM_ZOBJ(_name, _phpc_object)

/* create_ex object handler helper */
#define PHPC_OBJ_HANDLER_CREATE_EX(_name) \
	PHPC_OBJ_GET_HANDLER_FCE_INLINE_DEF(zend_object *, _name, create_ex) \
	(zend_class_entry *PHPC_CLASS_TYPE, int _phpc_init_props)
#define PHPC_OBJ_HANDLER_CREATE_EX_DECLARE() PHPC_NOOP
#define PHPC_OBJ_HANDLER_CREATE_EX_ALLOC(_name) \
	 ecalloc(1, sizeof(PHPC_OBJ_STRUCT_NAME(_name)) + sizeof(zval) * (PHPC_CLASS_TYPE->default_properties_count - 1));
#define PHPC_OBJ_HANDLER_INIT_CREATE_EX_PROPS(_intern) \
	do { \
		zend_object_std_init(&_intern->std, PHPC_CLASS_TYPE); \
		if (_phpc_init_props) { \
			object_properties_init(&_intern->std, PHPC_CLASS_TYPE); \
		} \
	} while(0)
#define PHPC_OBJ_HANDLER_CREATE_EX_RETURN_EX(_name, _intern) \
	do { \
		_intern->std.handlers = &PHPC_OBJ_GET_HANDLER_VAR_NAME(_name); \
		return &_intern->std; \
	} while(0)

/* create object handler */
#define PHPC_OBJ_HANDLER_CREATE(_name) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(zend_object *, _name, create) \
	(zend_class_entry *PHPC_CLASS_TYPE)
#define PHPC_OBJ_HANDLER_CREATE_RETURN(_name) \
	return PHPC_OBJ_GET_HANDLER_FCE(_name, create_ex)(PHPC_CLASS_TYPE, 1)

/* clone object handler */
#define PHPC_OBJ_HANDLER_CLONE(_name) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(zend_object *, _name, clone)(zval *_phpc_self)
#define PHPC_OBJ_HANDLER_CLONE_DECLARE() PHPC_NOOP
#define PHPC_OBJ_HANDLER_CLONE_MEMBERS(_name, _new_obj, _old_obj) \
	do { \
		_new_obj = PHPC_OBJ_FROM_ZOBJ(_name, PHPC_OBJ_GET_HANDLER_FCE(_name, create_ex)(_old_obj->std.ce, 0)); \
		zend_objects_clone_members(&_new_obj->std, &_old_obj->std); \
	} while(0)
#define PHPC_OBJ_HANDLER_CLONE_RETURN_EX(_new_obj) return &_new_obj->std;

/* free object handler */
#define PHPC_OBJ_HANDLER_FREE(_name) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(void, _name, free)(zend_object *_phpc_object)
#define PHPC_OBJ_HANDLER_FREE_DTOR(_intern) \
	zend_object_std_dtor(&(_intern)->std)

/* compare object handler */
#define PHPC_OBJ_HANDLER_COMPARE(_name) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(int, _name, compare)(zval *_phpc_obj1, zval *_phpc_obj2)
#define PHPC_OBJ_HANDLER_COMPARE_FETCH(_name, _id, _obj) \
	PHPC_OBJ_STRUCT_DECLARE(_name, _obj) = PHPC_OBJ_FROM_ZVAL(_name, _phpc_obj ## _id)

/* handler setters */
#define PHPC_OBJ_SET_HANDLER_OFFSET(_name) \
	PHPC_OBJ_GET_HANDLER_VAR_NAME(_name).offset = XtOffsetOf(PHPC_OBJ_STRUCT_NAME(_name), std)
#define PHPC_OBJ_SET_HANDLER_FREE(_name) \
	PHPC_OBJ_GET_HANDLER_VAR_NAME(_name).free_obj = PHPC_OBJ_GET_HANDLER_FCE(_name, free)

/* read propery */
#define PHPC_READ_PROPERTY_RV_NAME _phpc_read_property_rv
#define PHPC_READ_PROPERTY_RV_DECLARE zval PHPC_READ_PROPERTY_RV_NAME
#define PHPC_READ_PROPERTY(_scope, _object, _name, _name_len, _silent) \
	zend_read_property(_scope, _object, _name, _name_len, _silent, &PHPC_READ_PROPERTY_RV_NAME)


/* HASH */

/* update */
#define PHPC_HASH_INDEX_UPDATE zend_hash_index_update
#define PHPC_HASH_STR_UPDATE   zend_hash_update
#define PHPC_HASH_CSTRL_UPDATE zend_hash_str_update
#define PHPC_HASH_CSTR_UPDATE(_ht, _cstr_value, _pzv) \
	zend_hash_str_update(_ht, _cstr_value, strlen(_cstr_value), _pzv)

/* delete */
#define PHPC_HASH_INDEX_DELETE zend_hash_index_del
#define PHPC_HASH_STR_DELETE   zend_hash_del
#define PHPC_HASH_CSTRL_DELETE zend_hash_str_del
#define PHPC_HASH_CSTR_DELETE(_ht, _cstr_value) \
	zend_hash_str_del(_ht, _cstr_value, strlen(_cstr_value))

/* exists */
#define PHPC_HASH_INDEX_EXISTS zend_hash_index_exists
#define PHPC_HASH_STR_EXISTS   zend_hash_exists
#define PHPC_HASH_CSTRL_EXISTS zend_hash_str_exists
#define PHPC_HASH_CSTR_EXISTS(_ht, _cstr_value) \
	zend_hash_str_exists(_ht, _cstr_value, strlen(_cstr_value))

/* find */
#define PHPC_HASH_STR_FIND(_ht, _str, _ppv) \
	_ppv = zend_hash_find(_ht, _str)
#define PHPC_HASH_CSTRL_FIND(_ht, _cstr_value, _cstr_len, _ppv) \
	_ppv = zend_hash_str_find(_ht, _cstr_value, _cstr_len)
#define PHPC_HASH_CSTR_FIND(_ht, _cstr_value, _ppv) \
	_ppv = zend_hash_str_find(_ht, _cstr_value, strlen(_cstr_value))
#define PHPC_HASH_INDEX_FIND(_ht, _idx, _ppv) \
	_ppv = zend_hash_index_find(_ht, _idx)

/* iteration for each element */
#define PHPC_HASH_FOREACH_VAL             ZEND_HASH_FOREACH_VAL
#define PHPC_HASH_FOREACH_KEY_VAL         ZEND_HASH_FOREACH_KEY_VAL
#define PHPC_HASH_FOREACH_STR_KEY_VAL     ZEND_HASH_FOREACH_STR_KEY_VAL
#define PHPC_HASH_FOREACH_END             ZEND_HASH_FOREACH_END

/* key and data getter */
#define PHPC_HASH_GET_CURRENT_KEY_EX(_ht, _str, _num_index, _pos) \
	zend_hash_get_current_key_ex(_ht, &_str, &_num_index, _pos)
#define PHPC_HASH_GET_CURRENT_DATA_EX(_ht, _val, _pos) \
	_val = zend_hash_get_current_data_ex(_ht, _pos)

#define PHPC_HASH_GET_CURRENT_KEY(_ht, _str, _num_index) \
	zend_hash_get_current_key(_ht, &_str, &_num_index)
#define PHPC_HASH_GET_CURRENT_DATA(_ht, _val) \
	_val = zend_hash_get_current_data(_ht)

/* hash to zval */
#define PHPC_HASH_PZVAL(_ht, _pzv) \
	ZVAL_ARR(_pzv, _ht)
#define PHPC_HASH_RETVAL RETVAL_ARR
#define PHPC_HASH_RETURN RETURN_ARR


/* ARRAY */

#define PHPC_ARRAY_ADD_ASSOC_NULL_EX     add_assoc_null_ex
#define PHPC_ARRAY_ADD_ASSOC_BOOL_EX     add_assoc_bool_ex
#define PHPC_ARRAY_ADD_ASSOC_LONG_EX     add_assoc_long_ex
#define PHPC_ARRAY_ADD_ASSOC_RESOURCE_EX add_assoc_resource_ex
#define PHPC_ARRAY_ADD_ASSOC_DOUBLE_EX   add_assoc_double_ex
#define PHPC_ARRAY_ADD_ASSOC_STR_EX      add_assoc_str_ex
#define PHPC_ARRAY_ADD_ASSOC_CSTR_EX     add_assoc_string_ex
#define PHPC_ARRAY_ADD_ASSOC_CSTRL_EX    add_assoc_stringl_ex
#define PHPC_ARRAY_ADD_ASSOC_VAL_EX(_arr, _key, _key_len, _pv) \
	add_assoc_zval_ex(_arr, _key, _key_len, &_pv)

#define PHPC_ARRAY_ADD_ASSOC_STR         add_assoc_str
#define PHPC_ARRAY_ADD_ASSOC_CSTR        add_assoc_string
#define PHPC_ARRAY_ADD_ASSOC_CSTRL       add_assoc_stringl
#define PHPC_ARRAY_ADD_ASSOC_VAL(_arr, _key, _pv) \
	add_assoc_zval(_arr, _key, &_pv)

#define PHPC_ARRAY_ADD_INDEX_STR         add_index_str
#define PHPC_ARRAY_ADD_INDEX_CSTR        add_index_string
#define PHPC_ARRAY_ADD_INDEX_CSTRL       add_index_stringl
#define PHPC_ARRAY_ADD_INDEX_VAL(_arr, _idx, _pv) \
	add_assoc_zval(_arr, _idx, &_pv)

#define PHPC_ARRAY_ADD_NEXT_INDEX_STR    add_next_index_str
#define PHPC_ARRAY_ADD_NEXT_INDEX_CSTR   add_next_index_string
#define PHPC_ARRAY_ADD_NEXT_INDEX_CSTRL  add_next_index_stringl
#define PHPC_ARRAY_ADD_NEXT_INDEX_VAL(_arr, _pv) \
	add_assoc_zval(_arr, &_pv)

/* ZVAL */
typedef zval  phpc_val;

#define PHPC_TYPE        Z_TYPE
#define PHPC_TYPE_P      Z_TYPE_P

#define PHPC_LVAL        Z_LVAL
#define PHPC_LVAL_P      Z_LVAL_P

#define PHPC_DVAL        Z_DVAL
#define PHPC_DVAL_P      Z_DVAL_P

#define PHPC_STRVAL      Z_STRVAL
#define PHPC_STRVAL_P    Z_STRVAL_P

#define PHPC_STRLEN      Z_STRLEN
#define PHPC_STRLEN_P    Z_STRLEN_P

#define PHPC_ARRVAL      Z_ARRVAL
#define PHPC_ARRVAL_P    Z_ARRVAL_P

#define PHPC_VAL_TO_ZVAL(_pv, _zv) _zv = _pv
#define PHPC_VAL_TO_PZVAL(_pv, _zv) _zv = &(_pv)
#define PHPC_PVAL_TO_PZVAL(_pv, _zv) _zv = _pv

#define PHPC_VAL_MAKE(_pv) PHPC_NOOP

#define PHPC_VAL_STR(_pv, _str) \
	ZVAL_STR(&_pv, _str)
#define PHPC_VAL_CSTR(_pv, _cstr) \
    ZVAL_STRING(&_pv, _cstr)
#define PHPC_VAL_CSTRL(_pv, _cstr, _cstr_len) \
	ZVAL_STRINGL(&_pv, _cstr, _cstr_len)


#define PHPC_FE_END PHP_FE_END


#endif /* PHP_MAJOR_VERSION */


/* COMMON (dependent definitions) */

/* object structure */
#define PHPC_OBJ_STRUCT_DECLARE_AND_FETCH_FROM_ZOBJ(_name, _ptr) \
	PHPC_OBJ_STRUCT_DECLARE(_name, _ptr) = PHP_OBJ_GET_HANDLER_OBJ_FROM_ZOBJ(_name)

/* this object */
#define PHPC_THIS _phpc_this
#define PHPC_THIS_DECLARE(_name) PHPC_OBJ_STRUCT_DECLARE(_name, PHPC_THIS)
#define PHPC_THIS_FETCH_FROM_ZVAL(_name, _zv) \
	PHPC_THIS = PHPC_OBJ_FROM_ZVAL(_name, _zv)
#define PHPC_THIS_FETCH(_name) \
	PHPC_THIS_FETCH_FROM_ZVAL(_name, getThis())
#define PHPC_THIS_DECLARE_AND_FETCH_FROM_ZVAL(_name, _zv) \
	PHPC_THIS_DECLARE(_name); \
	PHPC_THIS_FETCH_FROM_ZVAL(_name, _zv)
#define PHPC_THIS_DECLARE_AND_FETCH(_name) \
	PHPC_THIS_DECLARE_AND_FETCH_FROM_ZVAL(_name, getThis())

/* that object */
#define PHPC_THAT _phpc_that
#define PHPC_THAT_DECLARE(_name) PHPC_OBJ_STRUCT_DECLARE(_name, PHPC_THAT)

/* object helper create_ex */
#define PHPC_OBJ_HANDLER_CREATE_EX_INIT(_name) \
	PHPC_OBJ_HANDLER_CREATE_EX_DECLARE(); \
	PHPC_THIS_DECLARE(_name); \
	PHPC_THIS = PHPC_OBJ_HANDLER_CREATE_EX_ALLOC(_name); \
	PHPC_OBJ_HANDLER_INIT_CREATE_EX_PROPS(PHPC_THIS)
#define PHPC_OBJ_HANDLER_CREATE_EX_RETURN(_name) \
	PHPC_OBJ_HANDLER_CREATE_EX_RETURN_EX(_name, PHPC_THIS)

/* object handler clone */
#define PHPC_OBJ_HANDLER_CLONE_INIT(_name) \
	PHPC_OBJ_HANDLER_CLONE_DECLARE(); \
	PHPC_THIS_DECLARE(_name); \
	PHPC_THAT_DECLARE(_name); \
	PHPC_THIS = PHPC_OBJ_FROM_SELF(_name); \
	PHPC_OBJ_HANDLER_CLONE_MEMBERS(_name, PHPC_THAT, PHPC_THIS)
#define PHPC_OBJ_HANDLER_CLONE_RETURN() \
	PHPC_OBJ_HANDLER_CLONE_RETURN_EX(PHPC_THAT)

/* object handler free */
#define PHPC_OBJ_HANDLER_FREE_INIT(_name) \
	PHPC_OBJ_STRUCT_DECLARE_AND_FETCH_FROM_ZOBJ(_name, PHPC_THIS);
#define PHPC_OBJ_HANDLER_FREE_DESTROY() \
	PHPC_OBJ_HANDLER_FREE_DTOR(PHPC_THIS);

/* object handler compare */
#define PHPC_OBJ_HANDLER_COMPARE_INIT(_name) \
	PHPC_OBJ_HANDLER_COMPARE_FETCH(_name, 1, PHPC_THIS); \
	PHPC_OBJ_HANDLER_COMPARE_FETCH(_name, 2, PHPC_THAT)

/* hash */
#define PHPC_HASH_ALLOC                     ALLOC_HASHTABLE
#define PHPC_HASH_INIT                      zend_hash_init
#define PHPC_HASH_NUM_ELEMENTS              zend_hash_num_elements

#define PHPC_HASH_HAS_MORE_ELEMENTS_EX      zend_hash_has_more_elements_ex
#define PHPC_HASH_MOVE_FORWARD_EX           zend_hash_move_forward_ex
#define PHPC_HASH_MOVE_BACKWARDS_EX         zend_hash_move_backwards_ex
#define PHPC_HASH_GET_CURRENT_KEY_ZVAL_EX   zend_hash_get_current_key_zval_ex
#define PHPC_HASH_GET_CURRENT_KEY_TYPE_EX   zend_hash_get_current_key_type_ex
#define PHPC_HASH_INTERNAL_POINTER_RESET_EX zend_hash_internal_pointer_reset_ex
#define PHPC_HASH_INTERNAL_POINTER_END_EX   zend_hash_internal_pointer_end_ex

#define PHPC_HASH_HAS_MORE_ELEMENTS         zend_hash_has_more_elements
#define PHPC_HASH_MOVE_FORWARD              zend_hash_move_forward
#define PHPC_HASH_MOVE_BACKWARDS            zend_hash_move_backwards
#define PHPC_HASH_GET_CURRENT_KEY_ZVAL      zend_hash_get_current_key_zval
#define PHPC_HASH_GET_CURRENT_KEY_TYPE      zend_hash_get_current_key_type
#define PHPC_HASH_INTERNAL_POINTER_RESET    zend_hash_internal_pointer_reset
#define PHPC_HASH_INTERNAL_POINTER_END      zend_hash_internal_pointer_end

/* array */
#define PHPC_ARRAY_INIT array_init
#if PHP_VERSION_ID < 50299
#define PHPC_ARRAY_INIT_SIZE(_arr, _size) array_init(_arr)
#else
#define PHPC_ARRAY_INIT_SIZE array_init_size
#endif

#define PHPC_ARRAY_ADD_ASSOC_NULL          add_assoc_null
#define PHPC_ARRAY_ADD_ASSOC_BOOL          add_assoc_bool
#define PHPC_ARRAY_ADD_ASSOC_LONG          add_assoc_long
#define PHPC_ARRAY_ADD_ASSOC_RESOURCE      add_assoc_resource
#define PHPC_ARRAY_ADD_ASSOC_DOUBLE        add_assoc_double

#define PHPC_ARRAY_ADD_INDEX_NULL          add_index_null
#define PHPC_ARRAY_ADD_INDEX_BOOL          add_index_bool
#define PHPC_ARRAY_ADD_INDEX_LONG          add_index_long
#define PHPC_ARRAY_ADD_INDEX_RESOURCE      add_index_resource
#define PHPC_ARRAY_ADD_INDEX_DOUBLE        add_index_double

#define PHPC_ARRAY_ADD_NEXT_INDEX_NULL     add_next_index_null
#define PHPC_ARRAY_ADD_NEXT_INDEX_BOOL     add_next_index_bool
#define PHPC_ARRAY_ADD_NEXT_INDEX_LONG     add_next_index_long
#define PHPC_ARRAY_ADD_NEXT_INDEX_RESOURCE add_next_index_resource
#define PHPC_ARRAY_ADD_NEXT_INDEX_DOUBLE   add_next_index_double

#endif	/* PHPC_H */

