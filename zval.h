/************************************************************************************************************

    NAME
	zval.h
	
    DESCRIPTION
	General header file for zval-related functions.
	
    AUTHOR
	Christian Vigh, 10/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# ifndef __ZVAL_H__
# 	define 	__ZVAL_H__

/*-----------------------------------------------------------------------------------------------------------

	External functions.
	
 *-----------------------------------------------------------------------------------------------------------*/
extern  PHP_FUNCTION		( zval_decref ) ;
extern  PHP_FUNCTION 		( zval_dump ) ;
extern  PHP_FUNCTION		( zval_incref ) ;
extern  PHP_FUNCTION  		( zval_isref ) ;
extern  PHP_FUNCTION		( zval_refcount ) ;
extern  PHP_FUNCTION		( zval_setref ) ;
extern  PHP_FUNCTION 		( zval_swap ) ;
extern  PHP_FUNCTION		( zval_type ) ;
extern  PHP_FUNCTION		( zval_type_constant ) ;

extern void 		zval_initialize		( THRAK_INIT_FUNC_ARGS ) ;
extern void 		zval_shutdown		( THRAK_SHUTDOWN_FUNC_ARGS ) ;


# endif		/* __ZVAL_H__ */
