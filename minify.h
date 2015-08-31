/************************************************************************************************************

    NAME
	array.h
	
    DESCRIPTION
	Definitions for buffered arrays.
	
    AUTHOR
	Christian Vigh, 10/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# ifndef 	__MINIFY_H__
# 	define	__MINIFY_H__

language_block ( C ) ;

/*-----------------------------------------------------------------------------------------------------------

	External array functions.
	
 *-----------------------------------------------------------------------------------------------------------*/
extern 				PHP_FUNCTION ( minify_php ) ; 
extern 				PHP_FUNCTION ( minify_php_stream ) ; 
extern 				PHP_FUNCTION ( minify_php_string ) ; 

extern THRAK_API zend_bool 	internal_minify_php 			( char * 		filename,
									  char *		input_content,
									  int 			input_content_length,
									  zend_bool 		preserve_newlines,
									  char **		output_content,
									  int * 		output_content_length 
									  TSRMLS_DC ) ;
 
extern void 			minify_initialize			( THRAK_INIT_FUNC_ARGS ) ;
extern void 			minify_shutdown				( THRAK_SHUTDOWN_FUNC_ARGS ) ;

end_language_block ( C ) ;

# endif		/* __MINIFY_H__  */