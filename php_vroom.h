/************************************************************************************************************

    NAME
	php_vroom.h
	
    DESCRIPTION
	General header file for the Vroom utility functions.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/

# ifndef __PHP_VROOM_H__
# 	define 	__PHP_VROOM_H__

/* Thrak helper API */
# include 	"thrak.h"


# define PHP_VROOM_VERSION 			"0.1.0"
# define PHP_VROOM_EXTENSION_NAME		"vroom" 


/*-----------------------------------------------------------------------------------------------------------

	External variables.
	
 *-----------------------------------------------------------------------------------------------------------*/
extern 		THRAK_MODULE ( vroom ) ;
extern 		THRAK_FUNCTION_TABLE ( vroom ) ;


/*-----------------------------------------------------------------------------------------------------------

	External user functions.
	
 *-----------------------------------------------------------------------------------------------------------*/
extern 	PHP_MINIT_FUNCTION 	( vroom ) ;
extern 	PHP_MSHUTDOWN_FUNCTION	( vroom ) ;
extern 	PHP_MINFO_FUNCTION 	( vroom ) ;
extern  PHP_RINIT_FUNCTION	( vroom ) ;
extern  PHP_RSHUTDOWN_FUNCTION	( vroom ) ;

/*-----------------------------------------------------------------------------------------------------------

	External functions.
	
 *-----------------------------------------------------------------------------------------------------------*/
extern void 		vroom_initialize	( THRAK_INIT_FUNC_ARGS ) ;
extern void 		vroom_shutdown 		( THRAK_SHUTDOWN_FUNC_ARGS ) ;

# endif		/* __PHP_VROOM_H__ */
