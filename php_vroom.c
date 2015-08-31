/************************************************************************************************************

    NAME
	vroom.c
	
    DESCRIPTION
	Vroom extension.
	
    AUTHOR
	Christian Vigh, 10/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# ifdef 	HAVE_CONFIG_H
# 	include 	"config.h"
# endif

# include 	"php_vroom.h"
# include	"array.h"
# include 	"data.h"
# include 	"minify.h"
# include 	"string.h"
# include 	"timer.h"
# include 	"zval.h"

# include 	"array_statics.h"
# include 	"data_statics.h"
# include 	"minify_statics.h"
# include 	"string_statics.h"
# include 	"timer_statics.h"
# include	"zval_statics.h"


/*** 
	Module function table.
 
	{{{ vroom_functions[]
 ***/
THRAK_BEGIN_FUNCTION_TABLE(vroom)
	/* array.c */
	THRAK_FE 	( array_flatten )
	THRAK_FE 	( array_ikey_exists )
	THRAK_FE	( iin_array )
	
   	/* array_buffer.c */
	THRAK_FE  	( array_buffer_clone )
	THRAK_FE  	( array_buffer_copy )
	THRAK_FE  	( array_buffer_create )
	THRAK_FE  	( array_buffer_destroy )
	THRAK_FE  	( array_buffer_resize )
	THRAK_FE  	( array_buffer_set )
	THRAK_FE  	( array_buffer_size )
    
	/* data.c */
	THRAK_FE  	( data_pack )
	THRAK_FE  	( data_unpack )
	
	THRAK_FEA  	( data_dwpack		, data_dxpack_arginfo )
	THRAK_FEA  	( data_dwlpack		, data_dxxpack_arginfo )
	THRAK_FEA  	( data_dwbpack		, data_dxxpack_arginfo )
	THRAK_FEA 	( data_dwmpack		, data_dxxpack_arginfo )
	
	THRAK_FEA  	( data_ddpack		, data_dxpack_arginfo )
	THRAK_FEA  	( data_ddlpack		, data_dxxpack_arginfo )
	THRAK_FEA  	( data_ddbpack		, data_dxxpack_arginfo )
	THRAK_FEA 	( data_ddmpack		, data_dxxpack_arginfo )
	
	THRAK_FEA  	( data_dqpack		, data_dxpack_arginfo )
	THRAK_FEA  	( data_dqlpack		, data_dxxpack_arginfo )
	THRAK_FEA  	( data_dqbpack		, data_dxxpack_arginfo )
	THRAK_FEA 	( data_dqmpack		, data_dxxpack_arginfo )
	
	THRAK_FEA  	( data_dwunpack		, data_dxunpack_arginfo )
	THRAK_FEA  	( data_dwlunpack	, data_dxxunpack_arginfo )
	THRAK_FEA  	( data_dwbunpack	, data_dxxunpack_arginfo )
	THRAK_FEA 	( data_dwmunpack	, data_dxxunpack_arginfo )
		
	THRAK_FEA  	( data_ddunpack		, data_dxunpack_arginfo )
	THRAK_FEA  	( data_ddlunpack	, data_dxxunpack_arginfo )
	THRAK_FEA  	( data_ddbunpack	, data_dxxunpack_arginfo )
	THRAK_FEA 	( data_ddmunpack	, data_dxxunpack_arginfo )

	/* minify.c */
	THRAK_FE 	( minify_php )
	THRAK_FE 	( minify_php_stream )
	THRAK_FE 	( minify_php_string )
	
	/* string.c */
	THRAK_FE  	( string_append )
	THRAK_FE 	( string_atoi )
	THRAK_FE 	( string_explode )
	THRAK_FE 	( string_setbos )
	THRAK_FE  	( string_endswith )
	THRAK_FE 	( string_implode )
	THRAK_FE 	( string_seteos )
	THRAK_FE  	( string_startswith )
	
	/* timer.c */
	THRAK_FE 	( timer_clock )
	THRAK_FE 	( timer_ticks_to )
	THRAK_FE 	( timer_ticks_to_seconds )
	THRAK_FE 	( timer_ticks_to_milliseconds )
	THRAK_FE 	( timer_ticks_to_microseconds )
	THRAK_FE 	( timer_ticks_to_nanoseconds )
	THRAK_FE 	( timer_diff ) 
	THRAK_FE 	( timer_clock_delta )
	
	/* zval.c */
	THRAK_FE 	( zval_decref )
	THRAK_FE 	( zval_dump )
	THRAK_FE 	( zval_incref )
	THRAK_FE 	( zval_isref )
	THRAK_FE 	( zval_refcount )
	THRAK_FE 	( zval_setref )
	THRAK_FE 	( zval_swap )
	THRAK_FE 	( zval_type )
	THRAK_FE 	( zval_type_constant )
THRAK_END_FUNCTION_TABLE ;
/* }}} */


/***
	Module initialization structure.
	
 	{{{ vroom_module_entry
 ***/
THRAK_BEGIN_MODULE ( vroom )
	STANDARD_MODULE_HEADER,
	PHP_VROOM_EXTENSION_NAME,
	vroom_functions,
	PHP_MINIT			(vroom),	/* Module initialization 		*/
	PHP_MSHUTDOWN			(vroom),	/* Module termination			*/
	PHP_RINIT			(vroom),	/* Request initialization 		*/
	PHP_RSHUTDOWN			(vroom),	/* Request termination			*/
	PHP_MINFO			(vroom),
	PHP_VROOM_VERSION,
	STANDARD_MODULE_PROPERTIES
THRAK_END_MODULE ;
/* }}} */


/***
	INI settings.
 ***/
PHP_INI_BEGIN ( )
	PHP_INI_ENTRY ( ARRAY_BUFFER_MAX_SIZE_SETTING		, DEFAULT_ARRAY_BUFFER_MAX_SIZE		, PHP_INI_SYSTEM, NULL )
	PHP_INI_ENTRY ( ARRAY_BUFFER_MIN_FREE_MEMORY_SETTING	, DEFAULT_ARRAY_BUFFER_MIN_FREE_MEMORY	, PHP_INI_SYSTEM, NULL )
PHP_INI_END ( ) ;


/***
	Needed for shared libraries.
 ***/
# ifdef 	COMPILE_DL_VROOM
THRAK_GET_MODULE_FUNCTION ( vroom )
   {
	
	THRAK_RETURN_MODULE_ENTRY ( vroom ) ;
    }
# else
#	error  This extension must be compiled as a shared 
# endif


/***
	Module initialization function.
	
	{{{ PHP_MINIT_FUNCTION
 ***/
PHP_MINIT_FUNCTION ( vroom )
   {
	REGISTER_INI_ENTRIES ( ) ;
	
	thrak_initialize	( THRAK_INIT_FUNC_ARGS_PASSTHRU ) ;
	vroom_initialize 	( THRAK_INIT_FUNC_ARGS_PASSTHRU ) ;
	
	array_initialize	( THRAK_INIT_FUNC_ARGS_PASSTHRU ) ;
	array_buffer_initialize	( THRAK_INIT_FUNC_ARGS_PASSTHRU ) ;
	data_initialize 	( THRAK_INIT_FUNC_ARGS_PASSTHRU ) ;
	minify_initialize 	( THRAK_INIT_FUNC_ARGS_PASSTHRU ) ;
	string_initialize 	( THRAK_INIT_FUNC_ARGS_PASSTHRU ) ;
	timer_initialize 	( THRAK_INIT_FUNC_ARGS_PASSTHRU ) ;
	zval_initialize 	( THRAK_INIT_FUNC_ARGS_PASSTHRU ) ;
	
	return ( SUCCESS ) ;
    }
/* }}} */


/***
	Module termination function.
	
	{{{ PHP_MSHUTDOWN_FUNCTION
 ***/
PHP_MSHUTDOWN_FUNCTION ( vroom )
   {
	zval_shutdown 		( THRAK_SHUTDOWN_FUNC_ARGS_PASSTHRU ) ;
	timer_shutdown	 	( THRAK_SHUTDOWN_FUNC_ARGS_PASSTHRU ) ;
	string_shutdown 	( THRAK_SHUTDOWN_FUNC_ARGS_PASSTHRU ) ;
	minify_initialize 	( THRAK_SHUTDOWN_FUNC_ARGS_PASSTHRU ) ;
	data_shutdown 		( THRAK_SHUTDOWN_FUNC_ARGS_PASSTHRU ) ;
	array_buffer_shutdown 	( THRAK_SHUTDOWN_FUNC_ARGS_PASSTHRU ) ;
	array_shutdown 		( THRAK_SHUTDOWN_FUNC_ARGS_PASSTHRU ) ;
	vroom_shutdown 		( THRAK_SHUTDOWN_FUNC_ARGS_PASSTHRU ) ;
	thrak_shutdown 		( THRAK_SHUTDOWN_FUNC_ARGS_PASSTHRU ) ;
	
	UNREGISTER_INI_ENTRIES ( ) ;
	
	return ( SUCCESS ) ;
    }
/* }}} */


/***
	Module per-request initialization function.
	
	{{{ PHP_RINIT_FUNCTION
 ***/
PHP_RINIT_FUNCTION ( vroom )
   {
	return ( SUCCESS ) ;
    }
/* }}} */


/***
	Module per-request shutdown function.
	
	{{{ PHP_RSHUTDOWN_FUNCTION
 ***/
PHP_RSHUTDOWN_FUNCTION ( vroom )
   {
	return ( SUCCESS ) ;
    }
/* }}} */



/***
	Module information function.
	
	{{{ PHP_MINFO_FUNCTION
 ***/
PHP_MINFO_FUNCTION ( vroom )
   {
	php_info_print_table_start 	( ) ;
	php_info_print_table_header 	( 2, "vroom support"	, "enabled" ) ;
	php_info_print_table_header 	( 2, "version"		, PHP_VROOM_VERSION ) ;
	php_info_print_table_end  	( ) ;
    } 
/* }}} */
