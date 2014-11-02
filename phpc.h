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

#if PHP_MAJOR_VERSION == 5

/* LONG */
typedef long phpc_long_t;

#define PHPC_LONG_TO_LONG_EX2(_plv, _lv, _exc_over, _exc_under) _lv = _plv
#define PHPC_LONG_TO_LONG_EX(_plv, _lv, _exc) PHPC_LONG_TO_LONG_EX2(_plv, _lv, _exc, _exc)
#define PHPC_LONG_TO_LONG(_plv, _lv) PHPC_LONG_TO_LONG_EX2(_plv, _lv, _lv = LONG_MAX, _lv = LONG_MIN)


/* STRING */
/* length type */
typedef int  phpc_str_size_t;
/* accessor macros */
#define PHPC_STR_VAL(_name) _name##__val
#define PHPC_STR_LEN(_name) _name##__len
#define PHPC_STR_LEN_FMT "d"
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
#define PHPC_STR_RELEASE(_name) efree(PHPC_STR_VAL(_name))

/* ZPP path flag */
#if PHP_VERSION_ID < 50399
#define PHPC_ZPP_PATH_FLAG "s"
#else
#define PHPC_ZPP_PATH_FLAG "p"
#endif


/* HASH */
#define PHPC_HASH_FOREACH_VAL(ht, _val) do { \
		HashPosition _pos; \
		for (zend_hash_internal_pointer_reset_ex(ht, &_pos); \
			zend_hash_get_current_data_ex(ht, (void **) &_val, &_pos) == SUCCESS; \
			zend_hash_move_forward_ex(ht, &_pos) )

#define PHPC_HASH_FOREACH_END() } while (0)


/* ZVAL */
typedef zval * phpc_val;

#define PHPC_TYPE   Z_TYPE_P
#define PHPC_TYPE_P Z_TYPE_PP

#define PHPC_VAL_TO_ZVAL(_pv, _zv) _zv = *(_pv)
#define PHPC_VAL_TO_PZVAL(_pv, _zv) (void) _pv
#define PHPC_PVAL_TO_PZVAL(_pv, _zv) _zv = *(_pv)

/* Function end */
#if (PHP_MINOR_VERSION == 3 && PHP_RELEASE_VERSION >= 7) || (PHP_MINOR_VERSION >= 4)
#define PHPC_FE_END PHP_FE_END
#else
#define PHPC_FE_END {NULL,NULL,NULL}
#endif


#else /* PHP 7 */

/* LONG */
typedef zend_long phpc_long_t;

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
#define PHPC_STR_LEN_FMT "zu"
#define PHPC_STR_DECLARE(_name) zend_string *_name
#define PHPC_STR_ARG(_name) zend_string *_name
#define PHPC_STR_PASS(_name) _name
#define PHPC_STR_REF(_name)  _name
#define PHPC_STR_RETURN(_name) RETURN_STR(_name)
/* wrapper macros */
#define PHPC_STR_INIT(_name, _cstr, _len) _name = zend_string_init(_cstr, _len, 0)
#define PHPC_STR_ALLOC(_name, _len) _name = zend_string_alloc(_len, 0)
#define PHPC_STR_RELEASE(_name) zend_string_release(_name)

/* ZPP path flag */
#define PHPC_PATH_ZPP_FLAG "p"


/* HASH */
#define PHPC_HASH_FOREACH_VAL ZEND_HASH_FOREACH_VAL
#define PHPC_HASH_FOREACH_END ZEND_HASH_FOREACH_END


/* ZVAL */
typedef zval  phpc_val;

#define PHPC_TYPE   Z_TYPE
#define PHPC_TYPE_P Z_TYPE_P

#define PHPC_VAL_TO_ZVAL(_pv, _zv) (void) _pv
#define PHPC_VAL_TO_PZVAL(_pv, _zv) _zv = &(_pv)
#define PHPC_PVAL_TO_PZVAL(_pv, _zv) (void) _pv

#define PHPC_FE_END PHP_FE_END


#endif /* PHP_MAJOR_VERSION */

#endif	/* PHPC_H */

