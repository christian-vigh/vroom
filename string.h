/************************************************************************************************************

    NAME
	string.h
	
    DESCRIPTION
	Definitions for string-related functions.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# ifndef 	__STRING_H__
# 	define	__STRING_H__

language_block ( C ) ;


/*-----------------------------------------------------------------------------------------------------------
	
	Macros & constants.
	
 *-----------------------------------------------------------------------------------------------------------*/

/* Implode / explode constants */
# define 	UNESCAPE_FLAG						0x80000000

# define 	EXPLODE_NONE 						0x00000000
# define 	EXPLODE_TRIM_LEFT 					0x00000001
# define 	EXPLODE_TRIM_RIGHT 					0x00000002
# define 	EXPLODE_TRIM						( EXPLODE_TRIM_LEFT | EXPLODE_TRIM_RIGHT )
# define 	EXPLODE_ESCAPE_QUOTES 					0x00000004
# define 	EXPLODE_ESCAPE_DOUBLEQUOTES 				0x00000008
# define 	EXPLODE_NOCASE	 					0x00000010
# define 	EXPLODE_IGNORE_EMPTY 					0x00000020
# define 	EXPLODE_TOLOWER 					0x00000040
# define 	EXPLODE_TOUPPER 					0x000000C0
# define 	EXPLODE_UCFIRST 					0x00000100
# define 	EXPLODE_UCWORDS						0x00000200
# define 	EXPLODE_UNESCAPE_QUOTES 				( EXPLODE_ESCAPE_QUOTES | UNESCAPE_FLAG )
# define 	EXPLODE_UNESCAPE_DOUBLEQUOTES 				( EXPLODE_ESCAPE_DOUBLEQUOTES | UNESCAPE_FLAG )


# define 	IMPLODE_NONE 						EXPLODE_NONE
# define 	IMPLODE_TRIM_LEFT 					EXPLODE_TRIM_LEFT
# define 	IMPLODE_TRIM_RIGHT 					EXPLODE_TRIM_RIGHT
# define 	IMPLODE_TRIM						EXPLODE_TRIM
# define 	IMPLODE_ESCAPE_QUOTES 					EXPLODE_ESCAPE_QUOTES
# define 	IMPLODE_ESCAPE_DOUBLEQUOTES 				EXPLODE_ESCAPE_DOUBLEQUOTES
# define 	IMPLODE_NOCASE		 				EXPLODE_NOCASE
# define 	IMPLODE_IGNORE_EMPTY					EXPLODE_IGNORE_EMPTY
# define 	IMPLODE_TOLOWER 					EXPLODE_TOLOWER
# define 	IMPLODE_TOUPPER 					EXPLODE_TOUPPER
# define 	IMPLODE_UCFIRST 					EXPLODE_UCFIRST
# define 	IMPLODE_UCWORDS						EXPLODE_UCWORDS
# define 	IMPLODE_UNESCAPE_QUOTES 				( IMPLODE_ESCAPE_QUOTES | UNESCAPE_FLAG )
# define 	IMPLODE_UNESCAPE_DOUBLEQUOTES 				( IMPLODE_ESCAPE_DOUBLEQUOTES | UNESCAPE_FLAG )

 
/*-----------------------------------------------------------------------------------------------------------

	External functions.
	
 *-----------------------------------------------------------------------------------------------------------*/
extern 				PHP_FUNCTION ( string_atoi ) ;

extern 				PHP_FUNCTION ( string_append ) ;
extern THRAK_API zend_bool	internal_string_append 			( zval * 		z_string,
									  int  			argc, 
									  zval **		argv,
									  char ** 		result,
									  int * 		result_length ) ;

extern 				PHP_FUNCTION ( string_endswith ) ;
extern THRAK_API zend_bool	internal_string_endswith 		( const char *		value,
									  int  			value_length,
									  const char * 		suffix,
									  int 			suffix_length,
									  int 			case_insensitive ) ;

extern 				PHP_FUNCTION ( string_explode ) ;
extern THRAK_API zend_bool	internal_string_explode 		( zval *		separator,
									  char *		value,
									  int 			value_length,
									  zval *		left_string,
									  zval *		right_string,
									  int 			options,
									  zval *		return_value
									  TSRMLS_DC ) ;
									  
extern 				PHP_FUNCTION ( string_implode ) ;
extern THRAK_API zend_bool	internal_string_implode 		( zval *		separator,
									  char *		value,
									  int 			value_length,
									  zval *		left_string,
									  zval *		right_string,
									  int 			options,
									  zval *		return_value
									  TSRMLS_DC ) ;
									  
extern 				PHP_FUNCTION ( string_setbos ) ;
extern THRAK_API zend_bool 	internal_string_setbos			( char *  		value, 
									  int  			value_length, 
									  char *  		prefix, 
									  int  			prefix_length, 
									  zend_bool 		case_insensitive,
									  char **  		result, 
									  int *  		result_length,
									  zend_bool *		modified ) ;

extern 				PHP_FUNCTION ( string_startswith ) ;
extern THRAK_API zend_bool	internal_string_startswith 		( const char *		value,
									  int  			value_length,
									  const char * 		preffix,
									  int 			prefix_length,
									  int 			case_insensitive ) ;
									  
extern 				PHP_FUNCTION ( string_seteos ) ;
extern THRAK_API zend_bool 	internal_string_seteos			( char *  		value, 
									  int  			value_length, 
									  char *  		suffix, 
									  int  			suffix_length, 
									  zend_bool 		case_insensitive,
									  char **  		result, 
									  int *  		result_length,
									  zend_bool *		modified ) ;

extern void 		string_initialize				( THRAK_INIT_FUNC_ARGS ) ;
extern void 		string_shutdown					( THRAK_SHUTDOWN_FUNC_ARGS ) ;


end_language_block ( C ) ;

# endif		/* __STRING_H__  */