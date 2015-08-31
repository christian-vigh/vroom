/************************************************************************************************************

    NAME
	data.h
	
    DESCRIPTION
	Definitions for data-related functions.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# ifndef 	__DATA_H__
# 	define	__DATA_H__

language_block ( C ) ;

/*-----------------------------------------------------------------------------------------------------------

	Packing/unpacking structure information.
	
 *-----------------------------------------------------------------------------------------------------------*/

/* Format specifiers characters */
# define 	PACK_FORMAT_NULL_PADDED_STRING 		'a' 

/* Format specifier structure */
typedef struct  packing_data_format
   {
	char 	format ;				/* Format specifier 									*/
	int 	flags ;					/* Extra flags 										*/
	int 	repeat ;				/* Length specified after the format specifier 						*/
	int 	size ;					/* When input arguments are available, indicates how much bytes can be retrieved 	*/
    } packing_data_format ;
 
/* Boxing structure */
typedef struct  packing_boxing_value
   {
	union
	   {
		char *		p ;
		short int 	si ;
		int 		i ;
		float 		f ;
		double 		d ;
		long 		l ;
		long long 	ll ;
	    } ;
    }   packing_boxing_value ;
 
 
/*-----------------------------------------------------------------------------------------------------------

	External functions.
	
 *-----------------------------------------------------------------------------------------------------------*/
 
/* Packing functions */
extern				PHP_FUNCTION ( data_dwpack ) ;
extern 				PHP_FUNCTION ( data_dwlpack ) ;
extern 				PHP_FUNCTION ( data_dwbpack ) ;
extern 				PHP_FUNCTION ( data_dwmpack ) ;

extern 				PHP_FUNCTION ( data_ddpack ) ;
extern 				PHP_FUNCTION ( data_ddlpack ) ;
extern 				PHP_FUNCTION ( data_ddbpack ) ;
extern 				PHP_FUNCTION ( data_ddmpack ) ;

extern 				PHP_FUNCTION ( data_dqpack ) ;
extern 				PHP_FUNCTION ( data_dqlpack ) ;
extern 				PHP_FUNCTION ( data_dqbpack ) ;
extern 				PHP_FUNCTION ( data_dqmpack ) ;

/* Internal packing functions - Note that the pack_dxm functions are not used by the PHP_FUNCTIONs but provided for internal convenience */
extern THRAK_API zend_bool	internal_data_pack_dwl 			( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC ) ;
extern THRAK_API zend_bool	internal_data_pack_dwb 			( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC ) ;
extern THRAK_API zend_bool	internal_data_pack_dwm 			( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC ) ;

extern THRAK_API zend_bool	internal_data_pack_ddl 			( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC ) ;
extern THRAK_API zend_bool	internal_data_pack_ddb 			( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC ) ;
extern THRAK_API zend_bool	internal_data_pack_ddm 			( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC ) ;

extern THRAK_API zend_bool	internal_data_pack_dql 			( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC ) ;
extern THRAK_API zend_bool	internal_data_pack_dqb 			( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC ) ;
extern THRAK_API zend_bool	internal_data_pack_dqm 			( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC ) ;

			
/* Unpacking functions */			
extern 				PHP_FUNCTION ( data_dwunpack ) ;
extern 				PHP_FUNCTION ( data_dwlunpack ) ;
extern 				PHP_FUNCTION ( data_dwbunpack ) ;
extern 				PHP_FUNCTION ( data_dwmunpack ) ;

extern 				PHP_FUNCTION ( data_ddunpack ) ;
extern 				PHP_FUNCTION ( data_ddlunpack ) ;
extern 				PHP_FUNCTION ( data_ddbunpack ) ;
extern 				PHP_FUNCTION ( data_ddmunpack ) ;

/* Internal unpacking functions - still for the convenience of internal needs */
extern THRAK_API zend_bool 	internal_data_unpack_dwl 		( zval *  binary_data, int  count, zval **  result TSRMLS_DC ) ;
extern THRAK_API zend_bool 	internal_data_unpack_dwb 		( zval *  binary_data, int  count, zval **  result TSRMLS_DC ) ;
extern THRAK_API zend_bool 	internal_data_unpack_dwm 		( zval *  binary_data, int  count, zval **  result TSRMLS_DC ) ;
									  
extern THRAK_API zend_bool 	internal_data_unpack_ddl 		( zval *  binary_data, int  count, zval **  result TSRMLS_DC ) ;
extern THRAK_API zend_bool 	internal_data_unpack_ddb 		( zval *  binary_data, int  count, zval **  result TSRMLS_DC ) ;
extern THRAK_API zend_bool 	internal_data_unpack_ddm 		( zval *  binary_data, int  count, zval **  result TSRMLS_DC ) ;
									  
extern THRAK_API zend_bool 	internal_data_unpack_dql 		( zval *  binary_data, int  count, zval **  result TSRMLS_DC ) ;
extern THRAK_API zend_bool 	internal_data_unpack_dqb 		( zval *  binary_data, int  count, zval **  result TSRMLS_DC ) ;
extern THRAK_API zend_bool 	internal_data_unpack_dqm 		( zval *  binary_data, int  count, zval **  result TSRMLS_DC ) ;
									  
 
/* General packing/unpacking functions */
extern THRAK_API int 		internal_data_decode_format 		( const char * 			format,
									  int 				format_length,
									  int 				argc,
									  zval **			argv,
									  packing_data_format **	output_format_specifiers,
									  int *				output_format_count 
									  TSRMLS_DC ) ;
									  
extern  			PHP_FUNCTION( data_pack ) ;
extern  THRAK_API int 		internal_data_pack 			( const char * 			format,
									  int  				format_length,
									  int 				argc,
									  zval **			argv,
									  char ** 			result,
									  size_t *			result_length
									  TSRMLS_DC ) ;

extern  			PHP_FUNCTION ( data_unpack ) ;
extern  THRAK_API int 		internal_data_unpack 			( const char * 			format,
									  int  				format_length,
									  int 				argc,
									  char **			argv,
									  char ** 			result,
									  size_t *			result_length ) ;

extern void 			data_initialize				( THRAK_INIT_FUNC_ARGS ) ;
extern void 			data_shutdown				( THRAK_SHUTDOWN_FUNC_ARGS ) ;

end_language_block ( C ) ;

# endif		/* __DATA_H__  */