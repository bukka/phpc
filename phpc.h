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

/* LONG */
typedef long  phpc_long_t;
typedef ulong phpc_ulong_t;

#define PHPC_LONG_TO_LONG_EX2(_plv, _lv, _exc_over, _exc_under) _lv = _plv
#define PHPC_LONG_TO_LONG_EX(_plv, _lv, _exc) PHPC_LONG_TO_LONG_EX2(_plv, _lv, _exc, _exc)
#define PHPC_LONG_TO_LONG(_plv, _lv) PHPC_LONG_TO_LONG_EX2(_plv, _lv, _lv = LONG_MAX, _lv = LONG_MIN)


/* STRING */
/* length type */
typedef int  phpc_str_size_t;
/* accessor macros */
#define PHPC_STR_VAL(_name) _name##__val
#define PHPC_STR_LEN(_name) _name##__len
#define PHPC_STR_LEN_UNUSED(_name) (void) PHPC_STR_LEN(_name);
#define PHPC_STR_LEN_FMT "d"
#define PHPC_STR_EXISTS(_name) PHPC_STR_VAL(_name)
#define PHPC_STR_DECLARE(_name) char *PHPC_STR_VAL(_name); int PHPC_STR_LEN(_name)
#define PHPC_STR_ARG(_name) char *PHPC_STR_VAL(_name), int PHPC_STR_LEN(_name)
#define PHPC_STR_PASS(_name) PHPC_STR_VAL(_name), PHPC_STR_LEN(_name)
#define PHPC_STR_REF(_name) &PHPC_STR_VAL(_name), &PHPC_STR_LEN(_name)
#define PHPC_STR_RETURN(_name) RETURN_STRINGL(PHPC_STR_VAL(_name), PHPC_STR_LEN(_name), 0)
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
#define PHPC_CSTR_WITH_LEN_RETURN(_name, _len) RETURN_STRINGL(_name, _len, 1)
#define PHPC_CSTR_RETURN(_name) RETURN_STRING(_name, 1)

/* ZPP path flag */
#if PHP_VERSION_ID < 50399
#define PHPC_ZPP_PATH_FLAG "s"
#else
#define PHPC_ZPP_PATH_FLAG "p"
#endif


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
#define PHPC_OBJ_FROM_ZOBJ(_object, _name) \
	(PHPC_OBJ_STRUCT_NAME(_name) *) _object
#define PHPC_OBJ_FROM_ZVAL(_zv, _name) \
	(PHPC_OBJ_STRUCT_NAME(_name) *) zend_object_store_get_object(_zv TSRMLS_CC)
#define PHPC_OBJ_FROM_SELF(_name) \
	PHPC_OBJ_FROM_ZVAL(_phpc_self, _name)
#define PHP_OBJ_GET_HANDLER_OBJ_FROM_ZOBJ(_name) \
	PHPC_OBJ_FROM_ZOBJ(_phpc_object, _name)

/* create_ex object handler helper */
#define PHPC_OBJ_HANDLER_CREATE_EX(_name) \
	PHPC_OBJ_GET_HANDLER_FCE_INLINE_DEF(zend_object_value, _name, create_ex) \
	(zend_class_entry *_phpc_class_type, PHPC_OBJ_STRUCT_NAME(_name) **_phpc_objptr TSRMLS_DC)
#define PHPC_OBJ_HANDLER_CREATE_EX_DECLARE() zend_object_value _phpc_retval
#define PHPC_OBJ_HANDLER_CREATE_EX_ALLOC(_name) \
	 ecalloc(1, sizeof(PHPC_OBJ_STRUCT_NAME(_name)));
#define PHPC_OBJ_HANDLER_INIT_CREATE_EX_PROPS(_intern) \
	do { \
		if (_phpc_objptr) { \
			*_phpc_objptr = _intern; \
		} \
		zend_object_std_init(&_intern->std, _phpc_class_type TSRMLS_CC); \
		object_properties_init(&_intern->std, _phpc_class_type); \
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
	(zend_class_entry *_phpc_class_type TSRMLS_DC)
#define PHPC_OBJ_HANDLER_CREATE_RETURN(_name) \
	return PHPC_OBJ_GET_HANDLER_FCE(_name, create_ex)(_phpc_class_type, NULL TSRMLS_CC)

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
	PHPC_OBJ_STRUCT_DECLARE(_name, _obj) = PHPC_OBJ_FROM_ZVAL(_phpc_obj ## _id, _name)

/* handler setters */
#define PHPC_OBJ_SET_HANDLER_OFFSET(_name) PHPC_NOOP
#define PHPC_OBJ_SET_HANDLER_FREE(_name) PHPC_NOOP


/* HASH */
#define PHPC_HASH_FOREACH_VAL(ht, _val) do { \
	HashPosition _pos; \
	for (zend_hash_internal_pointer_reset_ex((ht), &_pos); \
			zend_hash_get_current_data_ex((ht), (void **) &(_val), &_pos) == SUCCESS; \
			zend_hash_move_forward_ex((ht), &_pos) ) {

#define _PHPC_HASH_FOREACH_KEY_VAL(ht, _ph, _key, _val, _use_h) \
	PHPC_HASH_FOREACH_VAL(ht, _val) \
		uint _str_length; \
		ulong _num_index, *_pnum_index; \
		if (_use_h) { \
			_pnum_index = _ph; \
		} else { \
			_pnum_index = &_num_index; \
		} \
		int _key_type = zend_hash_get_current_key_ex(ht, &PHPC_STR_VAL(_key), &_str_length, _pnum_index, 0, &_pos); \
		if (_key_type == HASH_KEY_IS_STRING) { \
			PHPC_STR_LEN(_key) = (int) _str_length; \
		} else { \
			PHPC_STR_VAL(_key) = NULL; \
			PHPC_STR_LEN(_key) = 0; \
		}

#define PHPC_HASH_FOREACH_KEY_VAL(ht, _h, _key, _val) \
	_PHPC_HASH_FOREACH_KEY_VAL(ht, &_h, _key, _val, 1)

#define PHPC_HASH_FOREACH_STR_KEY_VAL(ht, _key, _val) \
	_PHPC_HASH_FOREACH_KEY_VAL(ht, NULL, _key, _val, 0)

#define PHPC_HASH_FOREACH_END() } } while (0)


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
#define PHPC_VAL_TO_PZVAL(_pv, _zv) (void) _pv
#define PHPC_PVAL_TO_PZVAL(_pv, _zv) _zv = *(_pv)

#define PHPC_VAL_MAKE MAKE_STD_ZVAL

#define PHPC_VAL_STR(_pv, _str) \
	ZVAL_STRINGL(_pv, PHPC_STR_VAL(_str), PHPC_STR_LEN(_str), 0)
#define PHPC_VAL_CSTR(_pv, _cstr) \
    ZVAL_STRING(_pv, _cstr, 1)
#define PHPC_VAL_CSTRL(_pv, _cstr_len) \
	ZVAL_STRINGL(_pv, _cstr, _cstr_len, 1)

/* Function end */
#if (PHP_MINOR_VERSION == 3 && PHP_RELEASE_VERSION >= 7) || (PHP_MINOR_VERSION >= 4)
#define PHPC_FE_END PHP_FE_END
#else
#define PHPC_FE_END {NULL,NULL,NULL}
#endif


#else /* PHP 7 */

/* LONG */
typedef zend_long  phpc_long_t;
typedef zend_ulong phpc_ulong_t;

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
/* accessor macros */
#define PHPC_STR_VAL(_name) (_name)->val
#define PHPC_STR_LEN(_name) (_name)->len
#define PHPC_STR_LEN_UNUSED(_name) PHPC_NOOP
#define PHPC_STR_LEN_FMT "zu"
#define PHPC_STR_EXISTS(_name) (_name)
#define PHPC_STR_DECLARE(_name) zend_string *_name
#define PHPC_STR_ARG(_name) zend_string *_name
#define PHPC_STR_PASS(_name) _name
#define PHPC_STR_REF(_name)  _name
#define PHPC_STR_RETURN(_name) RETURN_STR(_name)
/* wrapper macros */
#define PHPC_STR_INIT(_name, _cstr, _len) _name = zend_string_init(_cstr, _len, 0)
#define PHPC_STR_ALLOC(_name, _len) _name = zend_string_alloc(_len, 0)
#define PHPC_STR_REALLOC(_name, _len) _name = zend_string_realloc(_name, _len, 0)
#define PHPC_STR_RELEASE(_name) zend_string_release(_name)

/* C string */
#define PHPC_CSTR_WITH_LEN_RETURN(_name, _len) RETURN_STRINGL(_name, _len)
#define PHPC_CSTR_RETURN(_name) RETURN_STRING(_name)

/* ZPP path flag */
#define PHPC_PATH_ZPP_FLAG "p"


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

#define PHPC_OBJ_FROM_ZOBJ(_object, _name) \
	(PHPC_OBJ_STRUCT_NAME(_name) *)((char*)(_object) - XtOffsetOf(PHPC_OBJ_STRUCT_NAME(_name), std))
#define PHPC_OBJ_FROM_ZVAL(_zv, _name) \
	PHPC_OBJ_FROM_ZOBJ(Z_OBJ_P(_zv), _name)
#define PHPC_OBJ_FROM_SELF(_name) \
	PHPC_OBJ_FROM_ZVAL(_phpc_self, _name)
#define PHP_OBJ_GET_HANDLER_OBJ_FROM_ZOBJ(_name) \
	PHPC_OBJ_FROM_ZOBJ(_phpc_object, _name)

/* create_ex object handler helper */
#define PHPC_OBJ_HANDLER_CREATE_EX(_name) \
	PHPC_OBJ_GET_HANDLER_FCE_INLINE_DEF(zend_object *, _name, create_ex) \
	(zend_class_entry *_phpc_class_type, int _phpc_init_props)
#define PHPC_OBJ_HANDLER_CREATE_EX_DECLARE() PHPC_NOOP
#define PHPC_OBJ_HANDLER_CREATE_EX_ALLOC(_name) \
	 ecalloc(1, sizeof(PHPC_OBJ_STRUCT_NAME(_name)) + sizeof(zval) * (_phpc_class_type->default_properties_count - 1));
#define PHPC_OBJ_HANDLER_INIT_CREATE_EX_PROPS(_intern) \
	do { \
		zend_object_std_init(&_intern->std, _phpc_class_type); \
		if (_phpc_init_props) { \
			object_properties_init(&_intern->std, _phpc_class_type); \
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
	(zend_class_entry *_phpc_class_type)
#define PHPC_OBJ_HANDLER_CREATE_RETURN(_name) \
	return PHPC_OBJ_GET_HANDLER_FCE(_name, create_ex)(_phpc_class_type, 1)

/* clone object handler */
#define PHPC_OBJ_HANDLER_CLONE(_name) \
	PHPC_OBJ_DEFINE_HANDLER_FCE(zend_object *, _name, clone)(zval *_phpc_self)
#define PHPC_OBJ_HANDLER_CLONE_DECLARE() PHPC_NOOP
#define PHPC_OBJ_HANDLER_CLONE_MEMBERS(_name, _new_obj, _old_obj) \
	do { \
		_new_obj = PHPC_OBJ_FROM_ZOBJ(PHPC_OBJ_GET_HANDLER_FCE(_name, create_ex)(_old_obj->std.ce, 0), _name); \
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
	PHPC_OBJ_STRUCT_DECLARE(_name, _obj) = PHPC_OBJ_FROM_ZVAL(_phpc_obj ## _id, _name)

/* handler setters */
#define PHPC_OBJ_SET_HANDLER_OFFSET(_name) \
	PHPC_OBJ_GET_HANDLER_VAR_NAME(_name).offset = XtOffsetOf(PHPC_OBJ_STRUCT_NAME(_name), std)
#define PHPC_OBJ_SET_HANDLER_FREE(_name) \
	PHPC_OBJ_GET_HANDLER_VAR_NAME(_name).free_obj = PHPC_OBJ_GET_HANDLER_FCE(_name, free)


/* HASH */
#define PHPC_HASH_FOREACH_VAL             ZEND_HASH_FOREACH_VAL
#define PHPC_HASH_FOREACH_KEY_VAL         ZEND_HASH_FOREACH_KEY_VAL
#define PHPC_HASH_FOREACH_STR_KEY_VAL     ZEND_HASH_FOREACH_STR_KEY_VAL
#define PHPC_HASH_FOREACH_END             ZEND_HASH_FOREACH_END


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

#define PHPC_VAL_TO_ZVAL(_pv, _zv) (void) _pv
#define PHPC_VAL_TO_PZVAL(_pv, _zv) _zv = &(_pv)
#define PHPC_PVAL_TO_PZVAL(_pv, _zv) (void) _pv

#define PHPC_VAL_MAKE(_pv) PHPC_NOOP

#define PHPC_VAL_STR    ZVAL_STR
#define PHPC_VAL_CSTR   ZVAL_STRING
#define PHPC_VAL_CSTRL  ZVAL_STRINGL

#define PHPC_FE_END PHP_FE_END


#endif /* PHP_MAJOR_VERSION */


/* COMMON (dependent definitions) */

/* object structure */
#define PHPC_OBJ_STRUCT_DECLARE_AND_FETCH_FROM_ZOBJ(_name, _ptr) \
	PHPC_OBJ_STRUCT_DECLARE(_name, _ptr) = PHP_OBJ_GET_HANDLER_OBJ_FROM_ZOBJ(_name)

/* this object */
#define PHPC_THIS _phpc_this
#define PHPC_THIS_DECLARE(_name) PHPC_OBJ_STRUCT_DECLARE(_name, PHPC_THIS)
#define PHPC_THIS_FETCH(_name) PHPC_THIS = PHPC_OBJ_FROM_ZVAL(getThis(), _name)
#define PHPC_THIS_DECLARE_AND_FETCH(_name) \
	PHPC_THIS_DECLARE(_name); \
	PHPC_THIS_FETCH(_name);

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

#endif	/* PHPC_H */

