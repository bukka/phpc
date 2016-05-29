/*
 * Copyright (c) 2014-2016 Jakub Zelenka. All rights reserved.
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
#define PHPC_SELF       _phpc_self
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
#define PHPC_OBJ_HAS_HANDLER_GET_GC (PHP_VERSION_ID > 50399)
#if PHPC_OBJ_HAS_HANDLER_GET_GC
#define PHPC_OBJ_SET_HANDLER_GET_GC(_name) \
	PHPC_OBJ_GET_HANDLER_VAR_NAME(_name).get_gc = PHPC_OBJ_GET_HANDLER_FCE(_name, get_gc)
#else
#define PHPC_OBJ_SET_HANDLER_GET_GC(_name) PHPC_NOOP
#endif
#define PHPC_OBJ_HAS_HANDLER_GET_DEBUG_INFO (PHP_VERSION_ID > 50299)
#if PHPC_OBJ_HAS_HANDLER_GET_DEBUG_INFO
#define PHPC_OBJ_SET_HANDLER_GET_DEBUG_INFO(_name) \
	PHPC_OBJ_GET_HANDLER_VAR_NAME(_name).get_debug_info = PHPC_OBJ_GET_HANDLER_FCE(_name, get_debug_info)
#else
#define PHPC_OBJ_SET_HANDLER_GET_DEBUG_INFO(_name) PHPC_NOOP
#endif
/* there is such handler in 5.2 but we would have to re-implement zend_std_get_properties */
#define PHPC_OBJ_HAS_HANDLER_GET_PROPERTIES (PHP_VERSION_ID > 50299)
#if PHPC_OBJ_HAS_HANDLER_GET_PROPERTIES
#define PHPC_OBJ_SET_HANDLER_GET_PROPERTIES(_name) \
	PHPC_OBJ_GET_HANDLER_VAR_NAME(_name).get_properties = PHPC_OBJ_GET_HANDLER_FCE(_name, get_properties)
#else
#define PHPC_OBJ_SET_HANDLER_GET_PROPERTIES(_name) PHPC_NOOP
#endif

#if PHP_VERSION_ID < 50299
#define phpc_function_entry zend_function_entry
#else
#define phpc_function_entry const zend_function_entry
#endif


/* initializing properties in obejct (object_properties_init was added in PHP 5.4) */
#if PHP_VERSION_ID < 50399
#define PHPC_OBJ_PROPERTIES_INIT(zo, class_type) \
	{ \
		zval *tmp; \
		zend_hash_copy((*(zo)).properties, \
			&(class_type)->default_properties, \
			(copy_ctor_func_t) zval_add_ref, \
			(void *) &tmp, \
			sizeof(zval *)); \
	}
#else
#define PHPC_OBJ_PROPERTIES_INIT object_properties_init
#endif

/* common fcall macros */
#define PHPC_FCALL_PARAMS_NAME(_pname) _phpc_fcall_params__ ## _pname

/* integer conversions */
#define PHPC_CONVERT_NUMBER(_pn, _n, _exc_over, _exc_under, _type_from, _type_to, _max, _min) \
	if (_pn > (_type_from) _max) { \
		_exc_over; \
	} else if (_pn < (_type_from) _min) { \
		_exc_under; \
	} \
	_n = (_type_to) _pn

#define PHPC_CONVERT_NUMBER_SIGNED(_pn, _n, _exc_over, _type_from, _type_to, _max) \
	if (_pn > (_type_from) _max) { \
		_exc_over; \
	} \
	_n = (_type_to) _pn

#define PHPC_LONG_TO_INT_EX2(_plv, _lv, _exc_over, _exc_under) \
	PHPC_CONVERT_NUMBER(_plv, _lv, _exc_over, _exc_under, long, int, INT_MAX, INT_MIN)
#define PHPC_LONG_TO_INT_EX(_plv, _lv, _exc) \
	PHPC_LONG_TO_INT_EX2(_plv, _lv, _exc, _exc)
#define PHPC_LONG_TO_INT(_plv, _lv) \
	PHPC_LONG_TO_INT_EX2(_plv, _lv, _lv = INT_MAX, _lv = INT_MIN)


#if PHP_MAJOR_VERSION == 5

/* MODULE */

#ifdef ZTS
#define PHPC_MODULE_GLOBALS_ACCESSOR(_module, _v) \
	TSRMG(_module ## _globals_id, zend_ ## _module ## _globals *, _v)
#else
#define PHPC_MODULE_GLOBALS_ACCESSOR(_module, _v) \
	(_module ## _globals._v)
#endif

#define PHPC_MODULE_HAS_THREAD_CACHE 0


/* INTEGER */

/* long type */
typedef long  phpc_long_t;
/* unsigned long type */
typedef unsigned long phpc_ulong_t;
/* offset type */
typedef off_t phpc_off_t;
/* string length type */
typedef int phpc_str_size_t;

#define PHPC_LONG_TO_LONG_EX2(_plv, _lv, _exc_over, _exc_under) \
	_lv = _plv
#define PHPC_LONG_TO_LONG_EX(_plv, _lv, _exc) \
	_lv = _plv
#define PHPC_LONG_TO_LONG(_plv, _lv) \
	_lv = _plv

#define PHPC_SIZE_TO_LONG_EX  PHPC_LONG_TO_LONG_EX
#define PHPC_SIZE_TO_LONG     PHPC_LONG_TO_LONG

#define PHPC_SIZE_TO_INT_EX  PHPC_LONG_TO_LONG_EX
#define PHPC_SIZE_TO_INT     PHPC_LONG_TO_LONG


/* STRING */

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
#define PHPC_STR_DEREF_VAL(_name)             *PHPC_STR_VAL(_name)
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
#define PHPC_CSTR_RETURN(_name)        RETURN_STRING(_name, 1)
#define PHPC_CSTRL_RETVAL(_name, _len) RETVAL_STRINGL(_name, _len, 1)
#define PHPC_CSTR_RETVAL(_name)        RETVAL_STRING(_name, 1)

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


/* RESOURCE */
typedef zend_rsrc_list_entry phpc_res_entry_t;
typedef long phpc_res_value_t;

#define PHPC_RES_REGISTER(_entry, _type) \
	zend_list_insert(_entry, _type TSRMLS_CC)
#define PHPC_RES_FETCH(_pz_res, _res_type_name, _res_type) \
	zend_fetch_resource(&(_pz_res) TSRMLS_CC, -1, _res_type_name, NULL, 1, _res_type)
#define PHPC_RES_FETCH2(_pz_res, _res_type_name, _res_type_1, _res_type_2) \
	zend_fetch_resource(&(_pz_res) TSRMLS_CC, -1, _res_type_name, NULL, 2, _res_type_1, _res_type_2)
#define PHPC_RES_DELETE(_pz_res) \
	zend_list_delete(Z_LVAL_P(_pz_res))

/* resource to zval */
#define PHPC_RES_PZVAL(_res, _pzv) \
	ZVAL_RESOURCE(_pzv, _res)
#define PHPC_RES_RETVAL RETVAL_RESOURCE
#define PHPC_RES_RETURN RETURN_RESOURCE


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
	PHPC_OBJ_FROM_ZVAL(_name, PHPC_SELF)
#define PHP_OBJ_GET_HANDLER_OBJ_FROM_ZOBJ(_name) \
	PHPC_OBJ_FROM_ZOBJ(_name, _phpc_object)

/* create_ex object handler helper */
#define PHPC_OBJ_HANDLER_CREATE_EX(_name) \
	PHPC_OBJ_GET_HANDLER_FCE_INLINE_DEF(zend_object_value, _name, create_ex) \
	(zend_class_entry *PHPC_CLASS_TYPE, PHPC_OBJ_STRUCT_NAME(_name) **_phpc_objptr TSRMLS_DC)
#define PHPC_OBJ_HANDLER_CREATE_EX_IS_NEW() (_phpc_objptr == NULL)
#define PHPC_OBJ_HANDLER_CREATE_EX_DECLARE() zend_object_value _phpc_retval
#define PHPC_OBJ_HANDLER_CREATE_EX_ALLOC(_name) \
	 ecalloc(1, sizeof(PHPC_OBJ_STRUCT_NAME(_name)));
#define PHPC_OBJ_HANDLER_INIT_CREATE_EX_PROPS(_intern) \
	do { \
		if (_phpc_objptr) { \
			*_phpc_objptr = _intern; \
		} \
		zend_object_std_init(&_intern->std, PHPC_CLASS_TYPE TSRMLS_CC); \
		PHPC_OBJ_PROPERTIES_INIT(&_intern->std, PHPC_CLASS_TYPE); \
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
	PHPC_OBJ_DEFINE_HANDLER_FCE(zend_object_value, _name, clone)(zval *PHPC_SELF TSRMLS_DC)
#define PHPC_OBJ_HANDLER_CLONE_DECLARE() zend_object_value _phpc_retval
#define PHPC_OBJ_HANDLER_CLONE_MEMBERS(_name, _new_obj, _old_obj) \
	do { \
		_phpc_retval = PHPC_OBJ_GET_HANDLER_FCE(_name, create_ex)(_old_obj->std.ce, &_new_obj TSRMLS_CC); \
		zend_objects_clone_members(&_new_obj->std, _phpc_retval, &_old_obj->std, Z_OBJ_HANDLE_P(PHPC_SELF) TSRMLS_CC); \
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

/* apply count */
#define PHPC_HASH_GET_APPLY_COUNT(_ht)  _ht->nApplyCount
#define PHPC_HASH_INC_APPLY_COUNT(_ht)  _ht->nApplyCount++
#define PHPC_HASH_DEC_APPLY_COUNT(_ht)  _ht->nApplyCount--
#define PHPC_HASH_APPLY_PROTECTION(_ht) 1

/* add ptr */
#define PHPC_HASH_INDEX_ADD_PTR(_ht, _idx, _ptr, _ptr_size) \
	zend_hash_index_update(_ht, _idx, _ptr, _ptr_size, NULL)
#define PHPC_HASH_STR_ADD_PTR(_ht, _str, _ptr, _ptr_size) \
	zend_hash_add(_ht, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str) + 1, _ptr, _ptr_size, NULL)
#define PHPC_HASH_CSTRL_ADD_PTR(_ht, _cstr_value, _cstr_len, _ptr, _ptr_size) \
	zend_hash_add(_ht, _cstr_value, (_cstr_len) + 1, _ptr, _ptr_size, NULL)
#define PHPC_HASH_CSTR_ADD_PTR(_ht, _cstr_value, _ptr, _ptr_size) \
	zend_hash_add(_ht, _cstr_value, strlen(_cstr_value) + 1, _ptr, _ptr_size, NULL)
/* add */
#define PHPC_HASH_INDEX_ADD(_ht, _idx, _pzv) \
	zend_hash_index_update(_ht, _idx, &_pzv, sizeof(_pzv), NULL)
#define PHPC_HASH_STR_ADD(_ht, _str, _pzv) \
	zend_hash_add(_ht, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str) + 1, &_pzv, sizeof(_pzv), NULL)
#define PHPC_HASH_CSTRL_ADD(_ht, _cstr_value, _cstr_len, _pzv) \
	zend_hash_add(_ht, _cstr_value, (_cstr_len) + 1, &_pzv, sizeof(_pzv), NULL)
#define PHPC_HASH_CSTR_ADD(_ht, _cstr_value, _pzv) \
	zend_hash_add(_ht, _cstr_value, strlen(_cstr_value) + 1, &_pzv, sizeof(_pzv), NULL)

/* update ptr */
#define PHPC_HASH_INDEX_UPDATE_PTR(_ht, _idx, _ptr, _ptr_size) \
	zend_hash_index_update(_ht, _idx, _ptr, _ptr_size, NULL)
#define PHPC_HASH_STR_UPDATE_PTR(_ht, _str, _ptr, _ptr_size) \
	zend_hash_update(_ht, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str) + 1, _ptr, _ptr_size, NULL)
#define PHPC_HASH_CSTRL_UPDATE_PTR(_ht, _cstr_value, _cstr_len, _ptr, _ptr_size) \
	zend_hash_update(_ht, _cstr_value, (_cstr_len) + 1, _ptr, _ptr_size, NULL)
#define PHPC_HASH_CSTR_UPDATE_PTR(_ht, _cstr_value, _ptr, _ptr_size) \
	zend_hash_update(_ht, _cstr_value, strlen(_cstr_value) + 1, _ptr, _ptr_size, NULL)
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
/* find ptr */
#define PHPC_HASH_STR_FIND_PTR   PHPC_HASH_STR_FIND
#define PHPC_HASH_INDEX_FIND_PTR PHPC_HASH_INDEX_FIND
#define PHPC_HASH_CSTRL_FIND_PTR PHPC_HASH_CSTRL_FIND
#define PHPC_HASH_CSTR_FIND_PTR  PHPC_HASH_CSTR_FIND
/* not found value */
#define PHPC_HASH_NOT_FOUND FAILURE

/* key and data getter */
#define PHPC_HASH_GET_CURRENT_KEY_EX(_ht, _str, _num_index, _pos) \
	do { \
		unsigned int _phpc_current_key_len; \
		zend_hash_get_current_key_ex(_ht, &PHPC_STR_VAL(_str), \
			&_phpc_current_key_len, &_num_index, 0, _pos); \
		PHPC_STR_LEN(_str) = _phpc_current_key_len - 1; \
	} while(0)
#define PHPC_HASH_GET_CURRENT_DATA_EX(_ht, _val, _pos) \
	zend_hash_get_current_data_ex(_ht, (void **) &(_val), _pos)

#define PHPC_HASH_GET_CURRENT_KEY(_ht, _str, _num_index) \
	zend_hash_get_current_key(_ht, &PHPC_STR_VAL(_str), &_num_index, 0)
#define PHPC_HASH_GET_CURRENT_DATA(_ht, _val) \
	zend_hash_get_current_data(_ht, (void **) &(_val))

/* copy */
#define PHPC_HASH_COPY_EX(_target, _source, _copy_ctr) \
	zend_hash_copy(_target, _source, NULL, NULL, sizeof(zval *))

/* iteration for each element */
#define PHPC_HASH_FOREACH_KEY(_ht, _h, _key) do { \
	HashPosition _pos; \
	uint _str_length; \
	int _key_type; \
	for (zend_hash_internal_pointer_reset_ex((_ht), &_pos); \
			(_key_type = zend_hash_get_current_key_ex( \
				(_ht), &PHPC_STR_VAL(_key), &_str_length, &_h, 0, &_pos)) != HASH_KEY_NON_EXISTENT; \
			zend_hash_move_forward_ex((_ht), &_pos) ) { \
		if (_key_type == HASH_KEY_IS_STRING) { \
			PHPC_STR_LEN(_key) = (int) _str_length - 1; \
		} else { \
			PHPC_STR_VAL(_key) = NULL; \
			PHPC_STR_LEN(_key) = 0; \
		}

#define PHPC_HASH_FOREACH_VAL(_ht, _ppv) do { \
	HashPosition _pos; \
	for (zend_hash_internal_pointer_reset_ex((_ht), &_pos); \
			zend_hash_get_current_data_ex((_ht), (void **) &(_ppv), &_pos) == SUCCESS; \
			zend_hash_move_forward_ex((_ht), &_pos) ) {

#define PHPC_HASH_FOREACH_VAL_IND PHPC_HASH_FOREACH_VAL

#define _PHPC_HASH_FOREACH_KEY_VAL(_ht, _ph, _key, _ppv, _use_h) \
	PHPC_HASH_FOREACH_VAL(_ht, _ppv) \
		int _key_type; \
		uint _str_length; \
		ulong _num_index, *_pnum_index; \
		if (_use_h) { \
			_pnum_index = _ph; \
		} else { \
			_pnum_index = &_num_index; \
		} \
		_key_type = zend_hash_get_current_key_ex(_ht, &PHPC_STR_VAL(_key), &_str_length, _pnum_index, 0, &_pos); \
		if (_key_type == HASH_KEY_IS_STRING) { \
			PHPC_STR_LEN(_key) = (int) _str_length - 1; \
		} else { \
			PHPC_STR_VAL(_key) = NULL; \
			PHPC_STR_LEN(_key) = 0; \
		}

#define PHPC_HASH_FOREACH_KEY_VAL(_ht, _h, _key, _ppv) \
	_PHPC_HASH_FOREACH_KEY_VAL(_ht, &_h, _key, _ppv, 1)


#define PHPC_HASH_FOREACH_KEY_VAL_IND PHPC_HASH_FOREACH_KEY_VAL

#define PHPC_HASH_FOREACH_STR_KEY_VAL(_ht, _key, _ppv) \
	_PHPC_HASH_FOREACH_KEY_VAL(_ht, NULL, _key, _ppv, 0)

#define PHPC_HASH_FOREACH_STR_KEY_VAL_IND PHPC_HASH_FOREACH_STR_KEY_VAL

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
#define PHPC_ARRAY_ADD_ASSOC_ZVAL_EX(_arr, _key, _key_len, _zv) \
	add_assoc_zval_ex(_arr, _key, (_key_len) + 1, _zv)
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


/* FCALL */
#define _PHPC_FCALL_VARS_NAME(_pname) _phpc_fcall_vars__ ## _pname

#define PHPC_FCALL_PARAMS_DECLARE(_pname, _count) \
	int _phpc_fcall_params_count = _count; \
	zval *_PHPC_FCALL_VARS_NAME(_pname)[_count]; \
	zval **PHPC_FCALL_PARAMS_NAME(_pname)[_count]

#define PHPC_FCALL_PARAMS_INIT(_pname) \
	do { \
		int _idx; \
		for (_idx = 0; _idx <  _phpc_fcall_params_count; _idx++) \
			PHPC_FCALL_PARAMS_NAME(_pname)[_idx] = &_PHPC_FCALL_VARS_NAME(_pname)[_idx]; \
	} while(0)

#define PHPC_FCALL_PARAM_PZVAL(_pname, _idx) \
	_PHPC_FCALL_VARS_NAME(_pname)[_idx]

#define PHPC_FCALL_PARAM_VAL(_pname, _idx) \
	_PHPC_FCALL_VARS_NAME(_pname)[_idx]

#define PHPC_FCALL_PARAM_UNDEF(_pname, _idx) \
	ZVAL_NULL(PHPC_FCALL_PARAM_PZVAL(_pname, _idx))

#define PHPC_FCALL_RETVAL(_fci, _pv) \
	(_fci).retval_ptr_ptr = &(_pv)

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

#define PHPC_OBJCE       Z_OBJCE_P
#define PHPC_OBJCE_P     Z_OBJCE_PP

#define PHPC_OBJPROP     Z_OBJPROP_P
#define PHPC_OBJPROP_P   Z_OBJPROP_PP

#define PHPC_PZVAL_CAST_TO_PVAL(_pzv) &_pzv

#define PHPC_VAL_CAST_TO_ZVAL(_pv)    *(_pv)
#define PHPC_VAL_CAST_TO_PZVAL(_pv)   _pv
#define PHPC_PVAL_CAST_TO_PZVAL(_ppv) *(_ppv)

#define PHPC_VAL_TO_ZVAL(_pv, _zv)    _zv = *(_pv)
#define PHPC_VAL_TO_PZVAL(_pv, _zv)   _zv = _pv
#define PHPC_PVAL_TO_PZVAL(_ppv, _zv) _zv = *(_ppv)

#define PHPC_VAL_MAKE             MAKE_STD_ZVAL
#define PHPC_VAL_FREE             FREE_ZVAL
#define PHPC_VAL_UNDEF(_pv)       _pv = NULL
#define PHPC_VAL_ISUNDEF(_pv)     _pv == NULL
#define PHPC_VAL_COPY(_pv, _zv)   ZVAL_ZVAL(_pv, _zv, 1, 0)
#define PHPC_VAL_ASSIGN(_pv, _zv) _pv = _zv

#define PHPC_PVAL_DEREF(_pv)      PHPC_NOOP
#define PHPC_PZVAL_DEREF(_pv)     PHPC_NOOP

#define PHPC_VAL_STR(_pv, _str) \
	ZVAL_STRINGL(_pv, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str), 0)
#define PHPC_VAL_CSTR(_pv, _cstr) \
    ZVAL_STRING(_pv, _cstr, 1)
#define PHPC_VAL_CSTRL(_pv, _cstr, _cstr_len) \
	ZVAL_STRINGL(_pv, _cstr, _cstr_len, 1)

#define PHPC_PZVAL_STR   PHPC_VAL_STR
#define PHPC_PZVAL_CSTR  PHPC_VAL_CSTR
#define PHPC_PZVAL_CSTRL PHPC_VAL_CSTRL

#define PHPC_ZVAL_IS_TRUE(_zv) \
	(Z_TYPE(_zv) == IS_BOOL && Z_BVAL(_zv))
#define PHPC_ZVAL_IS_FALSE(_zv) \
	(Z_TYPE(_zv) == IS_BOOL && !Z_BVAL(_zv))

#define PHPC_VAL_IS_TRUE(_pv)  PHPC_ZVAL_IS_TRUE(*_pv)
#define PHPC_VAL_IS_FALSE(_pv) PHPC_ZVAL_IS_FALSE(*_pv)

/* Function end */
#if (PHP_MINOR_VERSION == 3 && PHP_RELEASE_VERSION >= 7) || (PHP_MINOR_VERSION >= 4)
#define PHPC_FE_END PHP_FE_END
#else
#define PHPC_FE_END {NULL,NULL,NULL}
#endif


/* ZPP */

/* path flag */
#if PHP_VERSION_ID < 50399
#define PHPC_ZPP_PATH_FLAG "s"
#else
#define PHPC_ZPP_PATH_FLAG "p"
#endif

/* args */
#define PHPC_ZPP_ARGS_DECLARE() \
	int _phpc_zpp_args_count = 0; \
	zval ***_phpc_zpp_args_array

#if PHP_API_VERSION < 20090626
#define PHPC_ZPP_ARGS_LOAD_EX(_flag, _num_args, _return) \
	do { \
		const char *_phpc_zpp_args_type_str = #_flag; \
		_phpc_zpp_args_count = _num_args; \
		_phpc_zpp_args_array = (zval ***) safe_emalloc(_num_args, sizeof (zval **), 0); \
		if ((_phpc_zpp_args_type_str[0] == '+' && _num_args == 0) || \
				zend_get_parameters_array_ex(_phpc_zpp_args_count, _phpc_zpp_args_array) == FAILURE) { \
			efree(_phpc_zpp_args_array); \
			zend_wrong_param_count(TSRMLS_C); \
			_return; \
		} \
	} while (0)
#else
#define PHPC_ZPP_ARGS_LOAD_EX(_flag, _num_args, _return) \
	do { \
		if (zend_parse_parameters(_num_args TSRMLS_CC, #_flag, \
				&_phpc_zpp_args_array, &_phpc_zpp_args_count) == FAILURE) { \
			_return; \
		} \
	} while(0)
#endif

#define PHPC_ZPP_ARGS_GET_PVAL(_arg_pos) \
	_phpc_zpp_args_array[_arg_pos]

#define PHPC_ZPP_ARGS_FREE() \
	if (_phpc_zpp_args_count > 0) \
		efree(_phpc_zpp_args_array)


/* STREAM */
#if PHP_VERSION_ID < 50600
typedef char phpc_stream_opener_char_t;
#else
typedef const char phpc_stream_opener_char_t;
#endif
#define PHPC_STREAM_WRAPPERDATA_ALLOC(stream) MAKE_STD_ZVAL(stream->wrapperdata)
#define PHPC_STREAM_WRAPPERDATA_ISSET(stream) stream->wrapperdata
#define PHPC_STREAM_WRAPPERDATA_UNSET(stream) \
	do { \
		zval_ptr_dtor(&stream->wrapperdata); \
		stream->wrapperdata = NULL; \
	} while(0)
#define PHPC_STREAM_CONTEXT_GET_OPTION(_ctx, _wrappername, _optionname, _ppv) \
	php_stream_context_get_option(_ctx, _wrappername, _optionname, &_ppv)
#define PHPC_STREAM_CONTEXT_OPTION_NOT_FOUND FAILURE


#else /* PHP 7 */

/* MODULE */

#define PHPC_MODULE_GLOBALS_ACCESSOR ZEND_MODULE_GLOBALS_ACCESSOR

#define PHPC_MODULE_HAS_THREAD_CACHE defined(ZTS) && defined(COMPILE_DL_JSON)


/* INTEGER */

/* long type */
typedef zend_long  phpc_long_t;
/* unsigned long type */
typedef zend_ulong phpc_ulong_t;
/* offset type */
typedef zend_off_t phpc_off_t;
/* string length type */
typedef size_t    phpc_str_size_t;

#define PHPC_LONG_TO_LONG_EX2(_plv, _lv, _exc_over, _exc_under) \
	PHPC_CONVERT_NUMBER(_plv, _lv, _exc_over, _exc_under, phpc_long_t, long, LONG_MAX, LONG_MIN)
#define PHPC_LONG_TO_LONG_EX(_plv, _lv, _exc) \
	PHPC_LONG_TO_LONG_EX2(_plv, _lv, _exc, _exc)
#define PHPC_LONG_TO_LONG(_plv, _lv) \
	PHPC_LONG_TO_LONG_EX2(_plv, _lv, _lv = LONG_MAX, _lv = LONG_MIN)

#define PHPC_SIZE_TO_LONG_EX(_plv, _lv, _exc_over) \
	PHPC_CONVERT_NUMBER_SIGNED(_plv, _lv, _exc_over, size_t, long, LONG_MAX)
#define PHPC_SIZE_TO_LONG(_plv, _lv) \
	PHPC_SIZE_TO_LONG_EX(_plv, _lv, _lv = LONG_MAX)

#define PHPC_SIZE_TO_INT_EX(_plv, _lv, _exc_over) \
	PHPC_CONVERT_NUMBER_SIGNED(_plv, _lv, _exc_over, size_t, int, INT_MAX)
#define PHPC_SIZE_TO_INT(_plv, _lv) \
	PHPC_SIZE_TO_LONG_EX(_plv, _lv, _lv = INT_MAX)


/* STRING */

/* accessor and convertor macros */
#define PHPC_STR_VAL                          ZSTR_VAL
#define PHPC_STR_LEN                          ZSTR_LEN
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
#define PHPC_STR_DEREF_VAL(_name)             *_name
#define PHPC_STR_FROM_PTR_STR(_str, _strp)    _str = *_strp
#define PHPC_STR_FROM_PTR_VAL(_str, _strpv)   _str = *_strpv
#define PHPC_STR_RETURN                       RETURN_STR
/* wrapper macros */
#define PHPC_STR_INIT(_name, _cstr, _len) \
	_name = zend_string_init(_cstr, _len, 0)
#define PHPC_STR_ALLOC(_name, _len) \
	_name = zend_string_alloc(_len, 0)
#define PHPC_STR_REALLOC(_name, _len) \
	_name = zend_string_realloc(_name, _len, 0)
#define PHPC_STR_RELEASE(_name) \
	zend_string_release(_name)

/* C string */
#define PHPC_CSTRL_RETURN                     RETURN_STRINGL
#define PHPC_CSTR_RETURN                      RETURN_STRING
#define PHPC_CSTRL_RETVAL                     RETVAL_STRINGL
#define PHPC_CSTR_RETVAL                      RETVAL_STRING

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


/* RESOURCE */
typedef zend_resource phpc_res_entry_t;
typedef zend_resource * phpc_res_value_t;

#define PHPC_RES_REGISTER zend_register_resource
#define PHPC_RES_FETCH(_pz_res, _res_type_name, _res_type) \
	zend_fetch_resource(Z_RES_P(_pz_res), _res_type_name, _res_type)
#define PHPC_RES_FETCH2(_pz_res, _res_type_name, _res_type_1, _res_type_2) \
	zend_fetch_resource2(Z_RES_P(_pz_res), _res_type_name, _res_type_1, _res_type_2)
#define PHPC_RES_DELETE(_pz_res) \
	zend_list_delete(Z_RES_P(_pz_res))

/* resource to zval */
#define PHPC_RES_PZVAL(_res, _pzv) \
	ZVAL_RES(_pzv, _res)
#define PHPC_RES_RETVAL RETVAL_RES
#define PHPC_RES_RETURN RETURN_RES


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
	PHPC_OBJ_FROM_ZVAL(_name, PHPC_SELF)
#define PHP_OBJ_GET_HANDLER_OBJ_FROM_ZOBJ(_name) \
	PHPC_OBJ_FROM_ZOBJ(_name, _phpc_object)

/* create_ex object handler helper */
#define PHPC_OBJ_HANDLER_CREATE_EX(_name) \
	PHPC_OBJ_GET_HANDLER_FCE_INLINE_DEF(zend_object *, _name, create_ex) \
	(zend_class_entry *PHPC_CLASS_TYPE, int _phpc_init_props)
#define PHPC_OBJ_HANDLER_CREATE_EX_IS_NEW() _phpc_init_props
#define PHPC_OBJ_HANDLER_CREATE_EX_DECLARE() PHPC_NOOP
#define PHPC_OBJ_HANDLER_CREATE_EX_ALLOC(_name) \
	 ecalloc(1, sizeof(PHPC_OBJ_STRUCT_NAME(_name)) + sizeof(zval) * (PHPC_CLASS_TYPE->default_properties_count - 1));
#define PHPC_OBJ_HANDLER_INIT_CREATE_EX_PROPS(_intern) \
	do { \
		zend_object_std_init(&_intern->std, PHPC_CLASS_TYPE); \
		if (_phpc_init_props) { \
			PHPC_OBJ_PROPERTIES_INIT(&_intern->std, PHPC_CLASS_TYPE); \
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
	PHPC_OBJ_DEFINE_HANDLER_FCE(zend_object *, _name, clone)(zval *PHPC_SELF)
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

/* apply count */
#define PHPC_HASH_GET_APPLY_COUNT  ZEND_HASH_GET_APPLY_COUNT
#define PHPC_HASH_INC_APPLY_COUNT  ZEND_HASH_INC_APPLY_COUNT
#define PHPC_HASH_DEC_APPLY_COUNT  ZEND_HASH_DEC_APPLY_COUNT
#define PHPC_HASH_APPLY_PROTECTION ZEND_HASH_APPLY_PROTECTION

/* add ptr */
#define PHPC_HASH_INDEX_ADD_PTR(_ht, _idx, _ptr, _ptr_size) \
	zend_hash_index_add_ptr(_ht, _idx, _ptr)
#define PHPC_HASH_STR_ADD_PTR(_ht, _str, _ptr, _ptr_size) \
	zend_hash_add_ptr(_ht, _str, _ptr)
#define PHPC_HASH_CSTRL_ADD_PTR(_ht, _cstr_value, _cstr_len, _ptr, _ptr_size) \
	zend_hash_str_add_ptr(_ht, _cstr_value, _cstr_len, _ptr)
#define PHPC_HASH_CSTR_ADD_PTR(_ht, _cstr_value, _ptr, _ptr_size) \
	zend_hash_str_add_ptr(_ht, _cstr_value, strlen(_cstr_value), _ptr)
/* add */
#define PHPC_HASH_INDEX_ADD zend_hash_index_add
#define PHPC_HASH_STR_ADD   zend_hash_add
#define PHPC_HASH_CSTRL_ADD zend_hash_str_add
#define PHPC_HASH_CSTR_ADD(_ht, _cstr_value, _pzv) \
	zend_hash_str_add(_ht, _cstr_value, strlen(_cstr_value), _pzv)

/* update ptr */
#define PHPC_HASH_INDEX_UPDATE_PTR(_ht, _idx, _ptr, _ptr_size) \
	zend_hash_index_update_ptr(_ht, _idx, _ptr)
#define PHPC_HASH_STR_UPDATE_PTR(_ht, _str, _ptr, _ptr_size) \
	zend_hash_update_ptr(_ht, _str, _ptr)
#define PHPC_HASH_CSTRL_UPDATE_PTR(_ht, _cstr_value, _cstr_len, _ptr, _ptr_size) \
	zend_hash_str_update_ptr(_ht, _cstr_value, _cstr_len, _ptr)
#define PHPC_HASH_CSTR_UPDATE_PTR(_ht, _cstr_value, _ptr, _ptr_size) \
	zend_hash_str_update_ptr(_ht, _cstr_value, strlen(_cstr_value), _ptr)
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
/* find ptr */
#define PHPC_HASH_STR_FIND_PTR(_ht, _str, _ptr) \
	_ptr = zend_hash_find_ptr(_ht, _str)
#define PHPC_HASH_CSTRL_FIND_PTR(_ht, _cstr_value, _cstr_len, _ptr) \
	_ptr = zend_hash_str_find_ptr(_ht, _cstr_value, _cstr_len)
#define PHPC_HASH_CSTR_FIND_PTR(_ht, _cstr_value, _ptr) \
	_ptr = zend_hash_str_find_ptr(_ht, _cstr_value, strlen(_cstr_value))
#define PHPC_HASH_INDEX_FIND_PTR(_ht, _idx, _ptr) \
	_ptr = zend_hash_index_find_ptr(_ht, _idx)
/* not found value */
#define PHPC_HASH_NOT_FOUND NULL

/* iteration for each element */
#define PHPC_HASH_FOREACH_KEY             ZEND_HASH_FOREACH_KEY
#define PHPC_HASH_FOREACH_VAL             ZEND_HASH_FOREACH_VAL
#define PHPC_HASH_FOREACH_VAL_IND         ZEND_HASH_FOREACH_VAL_IND
#define PHPC_HASH_FOREACH_KEY_VAL         ZEND_HASH_FOREACH_KEY_VAL
#define PHPC_HASH_FOREACH_KEY_VAL_IND     ZEND_HASH_FOREACH_KEY_VAL_IND
#define PHPC_HASH_FOREACH_STR_KEY_VAL     ZEND_HASH_FOREACH_STR_KEY_VAL
#define PHPC_HASH_FOREACH_STR_KEY_VAL_IND ZEND_HASH_FOREACH_STR_KEY_VAL_IND
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

/* copy */
#define PHPC_HASH_COPY_EX(_target, _source, _copy_ctr) \
	zend_hash_copy(_target, _source, NULL)

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
#define PHPC_ARRAY_ADD_ASSOC_ZVAL_EX     add_assoc_zval_ex
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
	add_index_zval(_arr, _idx, &_pv)

#define PHPC_ARRAY_ADD_NEXT_INDEX_STR    add_next_index_str
#define PHPC_ARRAY_ADD_NEXT_INDEX_CSTR   add_next_index_string
#define PHPC_ARRAY_ADD_NEXT_INDEX_CSTRL  add_next_index_stringl
#define PHPC_ARRAY_ADD_NEXT_INDEX_VAL(_arr, _pv) \
	add_next_index_zval(_arr, &_pv)

/* FCALL */
#define PHPC_FCALL_PARAMS_DECLARE(_pname, _count) \
	zval PHPC_FCALL_PARAMS_NAME(_pname)[_count]

#define PHPC_FCALL_PARAMS_INIT(_pname) PHPC_NOOP

#define PHPC_FCALL_PARAM_PZVAL(_pname, _idx) \
	&PHPC_FCALL_PARAMS_NAME(_pname)[_idx]

#define PHPC_FCALL_PARAM_VAL(_pname, _idx) \
	PHPC_FCALL_PARAMS_NAME(_pname)[_idx]

#define PHPC_FCALL_PARAM_UNDEF(_pname, _idx) \
	ZVAL_UNDEF(PHPC_FCALL_PARAM_PZVAL(_pname, _idx))

#define PHPC_FCALL_RETVAL(_fci, _pv) \
	(_fci).retval = (&_pv)


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

#define PHPC_OBJCE       Z_OBJCE
#define PHPC_OBJCE_P     Z_OBJCE_P

#define PHPC_OBJPROP     Z_OBJPROP
#define PHPC_OBJPROP_P   Z_OBJPROP_P

#define PHPC_PZVAL_CAST_TO_PVAL(_pzv) _pzv

#define PHPC_VAL_CAST_TO_ZVAL(_pv)    _pv
#define PHPC_VAL_CAST_TO_PZVAL(_pv)   &(_pv)
#define PHPC_PVAL_CAST_TO_PZVAL(_ppv) _ppv

#define PHPC_VAL_TO_ZVAL(_pv, _zv)    _zv = _pv
#define PHPC_VAL_TO_PZVAL(_pv, _zv)   _zv = &(_pv)
#define PHPC_PVAL_TO_PZVAL(_ppv, _zv) _zv = _ppv

#define PHPC_VAL_MAKE(_pv)      PHPC_NOOP
#define PHPC_VAL_FREE(_pv)      PHPC_NOOP
#define PHPC_VAL_UNDEF(_pv)     ZVAL_UNDEF(&(_pv))
#define PHPC_VAL_ISUNDEF(_pv)   Z_ISUNDEF(_pv)
#define PHPC_VAL_COPY(_pv, _zv) ZVAL_COPY(&(_pv), _zv)
#define PHPC_VAL_ASSIGN         PHPC_VAL_COPY

#define PHPC_PVAL_DEREF         ZVAL_DEREF
#define PHPC_PZVAL_DEREF        ZVAL_DEREF

#define PHPC_VAL_STR(_pv, _str) \
	ZVAL_STR(&_pv, _str)
#define PHPC_VAL_CSTR(_pv, _cstr) \
    ZVAL_STRING(&_pv, _cstr)
#define PHPC_VAL_CSTRL(_pv, _cstr, _cstr_len) \
	ZVAL_STRINGL(&_pv, _cstr, _cstr_len)

#define PHPC_PZVAL_STR   ZVAL_STR
#define PHPC_PZVAL_CSTR  ZVAL_STRING
#define PHPC_PZVAL_CSTRL ZVAL_STRINGL

#define PHPC_ZVAL_IS_TRUE(_zv) \
	(Z_TYPE(_zv) == IS_TRUE)
#define PHPC_ZVAL_IS_FALSE(_zv) \
	(Z_TYPE(_zv) == IS_FALSE)

#define PHPC_VAL_IS_TRUE  PHPC_ZVAL_IS_TRUE
#define PHPC_VAL_IS_FALSE PHPC_ZVAL_IS_FALSE

#define PHPC_FE_END PHP_FE_END


/* ZPP */

/* path flag */
#define PHPC_ZPP_PATH_FLAG "p"

/* args */
#define PHPC_ZPP_ARGS_DECLARE() \
	int _phpc_zpp_args_count = 0; \
	zval *_phpc_zpp_args_array

#define PHPC_ZPP_ARGS_LOAD_EX(_flag, _num_args, _return) \
	do { \
		if (zend_parse_parameters(_num_args, #_flag, \
				&_phpc_zpp_args_array, &_phpc_zpp_args_count) == FAILURE) { \
			_return; \
		} \
	} while(0)

#define PHPC_ZPP_ARGS_GET_PVAL(_arg_pos) \
	&_phpc_zpp_args_array[_arg_pos]

#define PHPC_ZPP_ARGS_FREE() PHPC_NOOP


/* STREAM */
typedef const char phpc_stream_opener_char_t;
#define PHPC_STREAM_WRAPPERDATA_ALLOC(stream) PHPC_NOOP
#define PHPC_STREAM_WRAPPERDATA_ISSET(stream) (Z_TYPE(stream->wrapperdata) != IS_UNDEF)
#define PHPC_STREAM_WRAPPERDATA_UNSET(stream) \
	do { \
		zval_ptr_dtor(&stream->wrapperdata); \
		ZVAL_UNDEF(&stream->wrapperdata); \
	} while(0)
#define PHPC_STREAM_CONTEXT_GET_OPTION(_ctx, _wrappername, _optionname, _ppv) \
	_ppv = php_stream_context_get_option(_ctx, _wrappername, _optionname)
#define PHPC_STREAM_CONTEXT_OPTION_NOT_FOUND NULL

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
	PHPC_THIS_DECLARE(_name) = PHPC_THIS_FETCH_FROM_ZVAL(_name, _zv)
#define PHPC_THIS_FETCH_FROM_SELF(_name) \
	PHPC_THIS = PHPC_OBJ_FROM_ZVAL(_name, PHPC_SELF)
#define PHPC_THIS_DECLARE_AND_FETCH_FROM_SELF(_name) \
	PHPC_THIS_DECLARE(_name) = PHPC_OBJ_FROM_SELF(_name)
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
	PHPC_OBJ_HANDLER_FREE_DTOR(PHPC_THIS)

/* object handler compare */
#define PHPC_OBJ_HANDLER_COMPARE_INIT(_name) \
	PHPC_OBJ_HANDLER_COMPARE_FETCH(_name, 1, PHPC_THIS); \
	PHPC_OBJ_HANDLER_COMPARE_FETCH(_name, 2, PHPC_THAT)

/* object handler get_gc */
#define PHPC_GC_TABLE _phpc_gc_table
#define PHPC_GC_N _phpc_gc_n
#define PHPC_OBJ_HANDLER_GET_GC(_name) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(HashTable *, _name, get_gc)\
		(zval *PHPC_SELF, phpc_val **PHPC_GC_TABLE, int *PHPC_GC_N TSRMLS_DC)

/* object handler get_debug_info */
#define PHPC_DEBUG_INFO_IS_TEMP _phpc_debug_info_is_temp
#define PHPC_OBJ_HANDLER_GET_DEBUG_INFO(_name) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(HashTable *, _name, get_debug_info)\
		(zval *PHPC_SELF, int *PHPC_DEBUG_INFO_IS_TEMP TSRMLS_DC)

/* object handler get_properties */
#define PHPC_OBJ_HANDLER_GET_PROPERTIES(_name) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(HashTable *, _name, get_properties)\
		(zval *PHPC_SELF TSRMLS_DC)

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

/* find */
#define PHPC_HASH_IS_FOUND(_found) ((_found) != PHPC_HASH_NOT_FOUND)
/* find in cond */
#define PHPC_HASH_STR_FIND_IN_COND(_ht, _str, _ppv) \
	PHPC_HASH_IS_FOUND(PHPC_HASH_STR_FIND(_ht, _str, _ppv))
#define PHPC_HASH_CSTRL_FIND_IN_COND(_ht, _cstr_value, _cstr_len, _ppv) \
	PHPC_HASH_IS_FOUND(PHPC_HASH_CSTRL_FIND(_ht, _cstr_value, _cstr_len, _ppv))
#define PHPC_HASH_CSTR_FIND_IN_COND(_ht, _cstr_value, _ppv) \
	PHPC_HASH_IS_FOUND(PHPC_HASH_CSTR_FIND(_ht, _cstr_value, _ppv))
#define PHPC_HASH_INDEX_FIND_IN_COND(_ht, _idx, _ppv) \
	PHPC_HASH_IS_FOUND(PHPC_HASH_INDEX_FIND(_ht, _idx, _ppv))
/* find ptr in cond */
#define PHPC_HASH_STR_FIND_PTR_IN_COND(_ht, _str, _ptr) \
	PHPC_HASH_IS_FOUND(PHPC_HASH_STR_FIND_PTR(_ht, _str, _ptr))
#define PHPC_HASH_CSTRL_FIND_PTR_IN_COND(_ht, _cstr_value, _cstr_len, _ptr) \
	PHPC_HASH_IS_FOUND(PHPC_HASH_CSTRL_FIND_PTR(_ht, _cstr_value, _cstr_len, _ptr))
#define PHPC_HASH_CSTR_FIND_PTR_IN_COND(_ht, _cstr_value, _ptr) \
	PHPC_HASH_IS_FOUND(PHPC_HASH_CSTR_FIND_PTR(_ht, _cstr_value, _ptr))
#define PHPC_HASH_INDEX_FIND_PTR_IN_COND(_ht, _idx, _ptr) \
	PHPC_HASH_IS_FOUND(PHPC_HASH_INDEX_FIND_PTR(_ht, _idx, _ptr))

/* copy */
#define PHPC_HASH_COPY(_target, _source) \
	PHPC_HASH_COPY_EX(_target, _source, NULL)

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
#define PHPC_ARRAY_ADD_ASSOC_ZVAL          add_assoc_zval

#define PHPC_ARRAY_ADD_INDEX_NULL          add_index_null
#define PHPC_ARRAY_ADD_INDEX_BOOL          add_index_bool
#define PHPC_ARRAY_ADD_INDEX_LONG          add_index_long
#define PHPC_ARRAY_ADD_INDEX_RESOURCE      add_index_resource
#define PHPC_ARRAY_ADD_INDEX_DOUBLE        add_index_double
#define PHPC_ARRAY_ADD_INDEX_ZVAL          add_index_zval

#define PHPC_ARRAY_ADD_NEXT_INDEX_NULL     add_next_index_null
#define PHPC_ARRAY_ADD_NEXT_INDEX_BOOL     add_next_index_bool
#define PHPC_ARRAY_ADD_NEXT_INDEX_LONG     add_next_index_long
#define PHPC_ARRAY_ADD_NEXT_INDEX_RESOURCE add_next_index_resource
#define PHPC_ARRAY_ADD_NEXT_INDEX_DOUBLE   add_next_index_double
#define PHPC_ARRAY_ADD_NEXT_INDEX_ZVAL     add_next_index_zval

/* ZPP */
/* alias for path flag */
#define PHPC_PATH_ZPP_FLAG PHPC_ZPP_PATH_FLAG

/* args loading */
#define PHPC_ZPP_ARGS_LOAD(_flag) PHPC_ZPP_ARGS_LOAD_EX(_flag, ZEND_NUM_ARGS(), return)

#define PHPC_ZPP_ARGS_LOOP_START_EX(_start) \
	do { \
		int _phpc_zpp_args_i; \
		for (_phpc_zpp_args_i = _start; _phpc_zpp_args_i < _phpc_zpp_args_count; _phpc_zpp_args_i++)

#define PHPC_ZPP_ARGS_LOOP_START() PHPC_ZPP_ARGS_LOOP_START_EX(0)

#define PHPC_ZPP_ARGS_LOOP_END() \
	} while(0)

#define PHPC_ZPP_ARGS_GET_CURRENT_PVAL() PHPC_ZPP_ARGS_GET_PVAL(_phpc_zpp_args_i)

#define PHPC_ZPP_ARGS_COUNT _phpc_zpp_args_count

/* FCALL */
#if PHP_API_VERSION < 20090626
#define PHPC_FCALL_INFO_INIT(_callable, _check_flags, _fci, _fci_cache, _callable_name, _error) \
	zend_fcall_info_init(_callable, &fci, &fci_cache TSRMLS_CC)
#else
#define PHPC_FCALL_INFO_INIT(_callable, _check_flags, _fci, _fci_cache, _callable_name, _error) \
	zend_fcall_info_init(_callable, _check_flags, _fci, _fci_cache, _callable_name, _error TSRMLS_CC)
#endif

/* stream */
#define PHPC_STREAM_CONTEXT_GET_OPTION_IN_COND(_ctx, _wrappername, _optionname, _ppv) \
	((PHPC_STREAM_CONTEXT_GET_OPTION(_ctx, _wrappername, _optionname, _ppv)) != \
		PHPC_STREAM_CONTEXT_OPTION_NOT_FOUND)

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 6) || (PHP_MAJOR_VERSION > 5)
#define PHPC_STREAM_LOCATE_URL_WRAPPER(_path, _path_for_open, _options) \
	php_stream_locate_url_wrapper(_path, (const char **) _path_for_open, _options TSRMLS_CC)
#else
#define PHPC_STREAM_LOCATE_URL_WRAPPER(_path, _path_for_open, _options) \
	php_stream_locate_url_wrapper(_path, (char **) _path_for_open, _options TSRMLS_CC)
#endif

#endif	/* PHPC_H */

