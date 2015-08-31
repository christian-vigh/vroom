/************************************************************************************************************

    NAME
	thrak.h
	
    DESCRIPTION
	General header file for the thrak API for developing PHP extensions.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/

# ifndef __THRAK_H__
# 	define 	__THRAK_H__

/*-----------------------------------------------------------------------------------------------------------

	Compiler/platform specific macros.
	
 *-----------------------------------------------------------------------------------------------------------*/

# if 	__cplusplus
#	define 		language(C) 		extern ##C
# 	define 		language_block(C)	language (C)  {
# 	define 		end_language_block(C) 	}
# else
# 	define 		language(C)
# 	define 		language_block(C)
#	define 		end_language_block(C)
# endif		

language_block(C) ;

 
/*-----------------------------------------------------------------------------------------------------------

	PHP includes.
	
 *-----------------------------------------------------------------------------------------------------------*/
# ifdef 	PHP_WIN32
# 	pragma 		warning ( disable : 4267 )
# endif

# include 	<php.h>
# include 	<php_ini.h>
# include 	<ext/standard/info.h>

# ifdef 	PHP_WIN32
# 	pragma 		warning ( default : 4267 )
# endif

# ifdef ZTS
#	include 	"TSRM.h"
# endif


/*-----------------------------------------------------------------------------------------------------------

	Machine-related constants & types.
	
 *-----------------------------------------------------------------------------------------------------------*/
# define 	BIG_ENDIAN 					1
# define 	LITTLE_ENDIAN 					0
# define 	MACHINE_ENDIAN					2

# if  ( ULONG_MAX == 0xFFFFFFFF )
# 	if  ( ULLONG_MAX  >  ULONG_MAX )
#		define 	LONG_SIZE 		4
		typedef short int 		int16 ;
		typedef int 			int32 ;
		typedef long long int 		int64 ;
		typedef unsigned short int 	uint16 ;
		typedef unsigned int 		uint32 ;
		typedef unsigned long long int 	uint64 ;
# 	else
# 		error Cannot compile on platforms that do not support 64-bit integers.
#	endif
# else
#		define 	LONG_SIZE 		8
		typedef short int 		int16 ;
		typedef int 			int32 ;
		typedef long int 		int64 ;
		typedef unsigned short int 	uint16 ;
		typedef unsigned int 		uint32 ;
		typedef unsigned long int 	uint64 ;
# endif

/* May need some adjustments */ 
typedef 	unsigned short int 				MACHINE_WORD ;
typedef 	unsigned int 					MACHINE_DWORD ;
typedef 	unsigned long long int				MACHINE_QWORD ;


/*-----------------------------------------------------------------------------------------------------------

	Platform-dependent defines.
	
 *-----------------------------------------------------------------------------------------------------------*/
# define 	THRAK_API					ZEND_API

# define 	PLATFORM_WINDOWS 				0 
# define 	PLATFORM_APPLE 					0 
# define 	PLATFORM_POSIX 					0 


# if 	defined ( _WIN32 ) ||  defined (  _WIN64 ) 

#  	undef  	PLATFORM_WINDOWS 
#  	define 	PLATFORM_WINDOWS 				1 
#  	define 	WIN32_LEAN_AND_MEAN 

# 	include 	<windows.h> 

# elif 	defined ( __APPLE__ ) 

#  	undef  	PLATFORM_APPLE 
#  	define 	PLATFORM_APPLE 					1 

# 	include 	<string.h> 
#	include 	<stdint.h> 

# else 

#  	undef  	PLATFORM_POSIX 
#  	define 	PLATFORM_POSIX 					1 

#  	include 	<unistd.h> 
#  	include 	<string.h> 
#	include 	<stdint.h> 

# endif 

/*-----------------------------------------------------------------------------------------------------------

	Misc macros & constants for Zend.
	
 *-----------------------------------------------------------------------------------------------------------*/
 
/* Shortcuts for returning a string in a PHP_FUNCTION, either by duplicating it or using the existing pointer */
# define 	RETURN_STRING_CONSTANT(str)			RETURN_STRING ( str, 1 )	
# define 	RETURN_STRING_VARIABLE(str)			RETURN_STRING ( str, 0 )

/* Substitute in argument list with a string and its size */
# define 	STRLC(strconst)					( strconst ), ( sizeof ( strconst ) )	
# define 	STRLC0(strconst)				( strconst ), ( sizeof ( strconst ) -  1 )	
# define 	STRLZ(strp)					( strp ), strlen ( strp )
# define 	STRLZ0(strp)					( strp ), ( strlen ( strp ) - 1 )


/*-----------------------------------------------------------------------------------------------------------

	Shortcuts for shared module definition.
	
 *-----------------------------------------------------------------------------------------------------------*/
# define 	THRAK_MODULE_ENTRY(name)			name##_module_entry
# define	THRAK_GET_MODULE(name) 				ZEND_GET_MODULE ( name )
# define 	THRAK_GET_MODULE_FUNCTION(name)			ZEND_DLEXPORT zend_module_entry *	get_module ( void )
# define 	THRAK_RETURN_MODULE_ENTRY(name)			return ( & THRAK_MODULE_ENTRY ( name ) )


/*-----------------------------------------------------------------------------------------------------------

	Shortcuts for function table definition.
	
 *-----------------------------------------------------------------------------------------------------------*/
# define 	THRAK_FUNCTION_TABLE(name)			const zend_function_entry 	name##_functions []
# define 	THRAK_BEGIN_FUNCTION_TABLE(name)		THRAK_FUNCTION_TABLE ( name ) = {
# define 	THRAK_FE(name)					PHP_FE ( name, name##_arginfo )
# define 	THRAK_FEA(name,arginfo)				PHP_FE ( name, arginfo )
# define        THRAK_FE_ALIAS(alias,name)			ZEND_FENTRY ( alias, ZEND_FN(name), name##arg_info, 0)
# define 	THRAK_FE_END					PHP_FE_END
# define 	THRAK_END_FUNCTION_TABLE			THRAK_FE_END } ;


/*-----------------------------------------------------------------------------------------------------------

	Shortcuts for module definition.
	
 *-----------------------------------------------------------------------------------------------------------*/
# define 	THRAK_MODULE(name)				zend_module_entry 	name##_module_entry
# define 	THRAK_BEGIN_MODULE(name)			THRAK_MODULE ( name ) = {
# define 	THRAK_END_MODULE				} ;

 
/*-----------------------------------------------------------------------------------------------------------

	Shortcuts for function argument definition.
	
 *-----------------------------------------------------------------------------------------------------------*/
# define 	THRAK_ARG_DECLARE(name)				const zend_arg_info 	name##_arginfo []			
# define 	THRAK_BEGIN_ARG_INFO_EX(name, _unused, return_reference, required_num_args)	\
								static THRAK_ARG_DECLARE ( name ) = {					\
									{ NULL, 0, NULL, required_num_args, 0, return_reference, 0, 0 },

# define 	THRAK_BEGIN_ARG_INFO(name,_unused,retval,required)				\
								THRAK_BEGIN_ARG_INFO_EX ( name, _unused, retval, required )
# define 	THRAK_ARG_INFO(byref,name)			ZEND_ARG_INFO ( byref, name )
# define 	THRAK_BYREF_ARG_INFO(name)			THRAK_ARG_INFO ( 1, name )
# define 	THRAK_BYVAL_ARG_INFO(name)			THRAK_ARG_INFO ( 0, name )

# define 	THRAK_ARG_VARIADIC_INFO(byref,name)		ZEND_ARG_VARIADIC_INFO ( byref, name )
# define 	THRAK_BYREF_ARG_VARIADIC_INFO(name)		THRAK_ARG_VARIADIC_INFO ( 1, name )
# define 	THRAK_BYVAL_ARG_VARIADIC_INFO(name)		THRAK_ARG_VARIADIC_INFO ( 0, name )

# define 	THRAK_END_ARG_INFO				ZEND_END_ARG_INFO() ;


/*-----------------------------------------------------------------------------------------------------------

	Macros to declare/access module globals. The same macros should work well in both thread-safe and non thread-safe
	environments.
	
 *-----------------------------------------------------------------------------------------------------------*/
# ifdef 	ZTS
#	define  THRAK_GLOBALS_NAME(name)			name##_globals_id
# 	define  THRAK_GLOBALS_TYPE(name)			zend_##name##_globals	
#	define 	THRAK_GLOBAL(name,v) 				TSRMG ( THRAK_GLOBALS_NAME ( name ), THRAK_GLOBALS_TYPE ( name ) *, v )
# 	define 	THRAK_INIT_GLOBALS(name,callback)		ZEND_INIT_MODULE_GLOBALS ( name, callback, NULL )
# 	define  THRAK_FETCH_GLOBALS()				TSRMLS_FETCH ( )
# else
#	define 	THRAK_GLOBALS_NAME(name)			name
# 	define  THRAK_GLOBALS_TYPE(name)			zend_##name##_globals	
#	define 	THRAK_GLOBAL(name,v) 				( name. ( v ) )
# 	define 	THRAK_INIT_GLOBALS(name,callback)		callback ( & name )
# 	define  THRAK_FETCH_GLOBALS()
# endif


/*-----------------------------------------------------------------------------------------------------------

	Definitions for submodules.
	
 *-----------------------------------------------------------------------------------------------------------*/
# define 	THRAK_INIT_FUNC_ARGS 				INIT_FUNC_ARGS
# define 	THRAK_SHUWTDOWN_FUNC_ARGS 			SHUTDOWN_FUNC_ARGS
# define 	THRAK_INFO_FUNC_ARGS 				ZEND_MODULE_INFO_FUNC_ARGS

# define 	THRAK_INIT_FUNC_ARGS_PASSTHRU 			INIT_FUNC_ARGS_PASSTHRU
# define 	THRAK_SHUTDOWN_FUNC_ARGS_PASSTHRU 		SHUTDOWN_FUNC_ARGS_PASSTHRU
# define 	THRAK_INFO_FUNC_ARGS_PASSTHRU 			ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU
 

/*-----------------------------------------------------------------------------------------------------------

	Ini settings helpers.
	
 *-----------------------------------------------------------------------------------------------------------*/
/* Defines a byte quantity, such as "1Mb", "256Kb", etc. */
typedef struct thrak_byte_quantity
   {
	long 		bytes ;					/* Actual byte count 		*/
	long		quantity ;				/* Specified quantity 		*/
	long 		multiplier ;				/* Multiplier quantity 		*/
	char *		multiplier_string ;			/* Specified multiplier 	*/
    }  thrak_byte_quantity ;

/* Defines a byte quantity, such as "1Mb", "256Kb", etc. or a percentage */
typedef struct thrak_memory_quantity
   {
	char 		type ; 					/* 0 = quantity, 1 = percentage */
	union							
	   {
		double 			percentage ;
		thrak_byte_quantity	quantity ;
	    } ;						/* Quantity or percentage       */
    }  thrak_memory_quantity ;

    
/*-----------------------------------------------------------------------------------------------------------

	External functions.
	
 *-----------------------------------------------------------------------------------------------------------*/

/* thrak.c */
extern void 			thrak_initialize			( THRAK_INIT_FUNC_ARGS ) ;
extern void 			thrak_shutdown				( THRAK_SHUTDOWN_FUNC_ARGS ) ;

extern int 			thrak_get_machine_endianness 		( ) ;
 
/* string_helpers.c */
extern  THRAK_API zend_bool	thrak_atoi 				( char *  			input_value, 
									  char **  			output_position,
									  int64 * 			output_value,
									  int 				is_escape ) ;
extern  THRAK_API char *	thrak_strichr 				( const char *			p,
									  int 				ch ) ;

extern  THRAK_API char *	thrak_trim  				( char *  			p, 
									  char *  			output ) ;
extern  THRAK_API char * 	thrak_ltrim  				( char *  			p, 
									  char *  			output ) ;
extern  THRAK_API char * 	thrak_rtrim  				( char *  			p, 
									  char *  			output ) ;
									  
/* array_helpers.c */
extern  THRAK_API char * 	thrak_array_to_char 			( zval * 			array,
									  char *** 			argv,
									  int *				argc,
									  int ** 			argl ) ;
extern  THRAK_API  zend_bool	thrak_free_array_to_char 		( char * 			data ) ;

/* ini_helpers.c */
extern  THRAK_API char * 	thrak_ini_get_string 			( char *  			setting, 
									  char *  			default_value ) ;
extern  THRAK_API int 		thrak_ini_parse_byte_quantity 		( char *  			value, 
									  thrak_byte_quantity *  	quantity ) ;
extern  THRAK_API int 		thrak_ini_parse_percentage 		( char *  			value, 
									  double *  			percentage ) ;
extern  THRAK_API int 		thrak_ini_parse_memory_quantity 	( char *  			value, 
									  thrak_memory_quantity *  	quantity ) ;

/* memory_helpers.c */
extern  THRAK_API int  		thrak_memory_check_constraint 		( long  			memory_requested, 
									  thrak_memory_quantity *  	constraint
									  TSRMLS_DC ) ;
								  
/*-----------------------------------------------------------------------------------------------------------

	External variables.
	
 *-----------------------------------------------------------------------------------------------------------*/
extern int 		thrak_machine_endianness ;

end_language_block ( C ) ;

# endif 	/* __THRAK_H__ */
