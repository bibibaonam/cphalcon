
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2012 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_phalcon.h"
#include "phalcon.h"

#include "Zend/zend_operators.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#include "kernel/main.h"
#include "kernel/memory.h"

#include "kernel/exception.h"
#include "kernel/object.h"
#include "kernel/array.h"
#include "kernel/operators.h"
#include "kernel/fcall.h"
#include "kernel/concat.h"

PHP_METHOD(Phalcon_Events_Manager, __construct){

	zval *a0 = NULL;

	PHALCON_MM_GROW();

	PHALCON_ALLOC_ZVAL_MM(a0);
	array_init(a0);
	zend_update_property(phalcon_events_manager_ce, this_ptr, SL("_events"), a0 TSRMLS_CC);

	PHALCON_MM_RESTORE();
}

PHP_METHOD(Phalcon_Events_Manager, attach){

	zval *event_type = NULL, *handler = NULL, *events = NULL;
	zval *a0 = NULL;
	int eval_int;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &event_type, &handler) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (Z_TYPE_P(handler) != IS_OBJECT) {
		PHALCON_THROW_EXCEPTION_STR(phalcon_events_exception_ce, "Event handler must be an Object");
		return;
	}
	
	PHALCON_INIT_VAR(events);
	phalcon_read_property(&events, this_ptr, SL("_events"), PH_NOISY_CC);
	eval_int = phalcon_array_isset(events, event_type);
	if (!eval_int) {
		PHALCON_ALLOC_ZVAL_MM(a0);
		array_init(a0);
		phalcon_array_update_zval(&events, event_type, &a0, PH_COPY | PH_SEPARATE TSRMLS_CC);
	}
	
	phalcon_array_update_multi_append_2(&events, event_type, handler, 0 TSRMLS_CC);
	phalcon_update_property_zval(this_ptr, SL("_events"), events TSRMLS_CC);
	
	PHALCON_MM_RESTORE();
}

PHP_METHOD(Phalcon_Events_Manager, dettach){

	zval *event_type = NULL, *handler = NULL, *events = NULL, *found = NULL, *events_type = NULL;
	zval *internal_handler = NULL, *number = NULL;
	zval *r0 = NULL, *r1 = NULL, *r2 = NULL;
	HashTable *ah0;
	HashPosition hp0;
	zval **hd;
	char *hash_index;
	uint hash_index_len;
	ulong hash_num;
	int hash_type;
	int eval_int;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &event_type, &handler) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(events);
	phalcon_read_property(&events, this_ptr, SL("_events"), PH_NOISY_CC);
	eval_int = phalcon_array_isset(events, event_type);
	if (eval_int) {
		PHALCON_INIT_VAR(found);
		ZVAL_BOOL(found, 0);
		
		PHALCON_ALLOC_ZVAL_MM(r0);
		
		PHALCON_ALLOC_ZVAL_MM(r1);
		phalcon_array_fetch(&r1, events, event_type, PH_NOISY_CC);
		phalcon_cast(r0, r1, IS_ARRAY);
		PHALCON_CPY_WRT(events_type, r0);
		if (!phalcon_valid_foreach(events_type TSRMLS_CC)) {
			return;
		}
		
		ALLOC_HASHTABLE(ah0);
		zend_hash_init(ah0, 0, NULL, NULL, 0);
		zend_hash_copy(ah0, Z_ARRVAL_P(events_type), NULL, NULL, sizeof(zval*));
		zend_hash_internal_pointer_reset_ex(ah0, &hp0);
		fes_daef_0:
			if(zend_hash_get_current_data_ex(ah0, (void**) &hd, &hp0) != SUCCESS){
				goto fee_daef_0;
			}
			
			PHALCON_INIT_VAR(number);
			PHALCON_GET_FOREACH_KEY(number, ah0, hp0);
			PHALCON_INIT_VAR(internal_handler);
			ZVAL_ZVAL(internal_handler, *hd, 1, 0);
			PHALCON_INIT_VAR(r2);
			is_equal_function(r2, internal_handler, handler TSRMLS_CC);
			if (zend_is_true(r2)) {
				PHALCON_SEPARATE(events_type);
				phalcon_array_unset(events_type, number);
				
				PHALCON_INIT_VAR(found);
				ZVAL_BOOL(found, 1);
			}
			zend_hash_move_forward_ex(ah0, &hp0);
			goto fes_daef_0;
		fee_daef_0:
		zend_hash_destroy(ah0);
		efree(ah0);
		
		if (zend_is_true(found)) {
			phalcon_array_update_zval(&events, event_type, &events_type, PH_COPY | PH_SEPARATE TSRMLS_CC);
			phalcon_update_property_zval(this_ptr, SL("_events"), events TSRMLS_CC);
		}
	}
	
	PHALCON_MM_RESTORE();
}

PHP_METHOD(Phalcon_Events_Manager, dettachAll){

	zval *type = NULL, *events = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &type) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(events);
	phalcon_read_property(&events, this_ptr, SL("_events"), PH_NOISY_CC);
	PHALCON_SEPARATE(events);
	phalcon_array_unset(events, type);
	phalcon_update_property_zval(this_ptr, SL("_events"), events TSRMLS_CC);
	
	PHALCON_MM_RESTORE();
}

PHP_METHOD(Phalcon_Events_Manager, fire){

	zval *event_type = NULL, *source = NULL, *event_parts = NULL, *type = NULL;
	zval *event_name = NULL, *events = NULL, *fire_events = NULL, *handler = NULL;
	zval *event = NULL;
	zval *c0 = NULL;
	zval *i0 = NULL;
	zval *r0 = NULL, *r1 = NULL;
	HashTable *ah0;
	HashPosition hp0;
	zval **hd;
	int eval_int;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &event_type, &source) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(c0);
	ZVAL_STRING(c0, ":", 1);
	PHALCON_INIT_VAR(event_parts);
	phalcon_fast_explode(event_parts, c0, event_type TSRMLS_CC);
	eval_int = phalcon_array_isset_long(event_parts, 1);
	if (!eval_int) {
		PHALCON_ALLOC_ZVAL_MM(i0);
		object_init_ex(i0, phalcon_events_exception_ce);
		PHALCON_ALLOC_ZVAL_MM(r0);
		PHALCON_CONCAT_SV(r0, "Invalid event type ", event_type);
		PHALCON_CALL_METHOD_PARAMS_1_NORETURN(i0, "__construct", r0, PH_CHECK);
		phalcon_throw_exception(i0 TSRMLS_CC);
		return;
	}
	
	PHALCON_INIT_VAR(type);
	phalcon_array_fetch_long(&type, event_parts, 0, PH_NOISY_CC);
	
	PHALCON_INIT_VAR(event_name);
	phalcon_array_fetch_long(&event_name, event_parts, 1, PH_NOISY_CC);
	
	PHALCON_INIT_VAR(events);
	phalcon_read_property(&events, this_ptr, SL("_events"), PH_NOISY_CC);
	eval_int = phalcon_array_isset(events, type);
	if (eval_int) {
		PHALCON_INIT_VAR(fire_events);
		phalcon_array_fetch(&fire_events, events, type, PH_NOISY_CC);
		if (!phalcon_valid_foreach(fire_events TSRMLS_CC)) {
			return;
		}
		
		ah0 = Z_ARRVAL_P(fire_events);
		zend_hash_internal_pointer_reset_ex(ah0, &hp0);
		fes_daef_1:
			if(zend_hash_get_current_data_ex(ah0, (void**) &hd, &hp0) != SUCCESS){
				goto fee_daef_1;
			}
			
			PHALCON_INIT_VAR(handler);
			ZVAL_ZVAL(handler, *hd, 1, 0);
			if (Z_TYPE_P(handler) == IS_OBJECT) {
				PHALCON_INIT_VAR(event);
				object_init_ex(event, phalcon_events_event_ce);
				PHALCON_CALL_METHOD_PARAMS_2_NORETURN(event, "__construct", event_name, source, PH_CHECK);
				
				PHALCON_INIT_VAR(r1);
				phalcon_get_class(r1, handler TSRMLS_CC);
				if (PHALCON_COMPARE_STRING(r1, "Closure")) {
				} else {
					if (phalcon_method_exists(handler, event_name TSRMLS_CC) == SUCCESS) {
						PHALCON_CALL_METHOD_PARAMS_2_NORETURN(handler, Z_STRVAL_P(event_name), event, source, PH_NO_CHECK);
					}
				}
			}
			zend_hash_move_forward_ex(ah0, &hp0);
			goto fes_daef_1;
		fee_daef_1:
		if(0){}
		
	}
	
	PHALCON_MM_RESTORE();
}

