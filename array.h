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
# ifndef 	__ARRAY_H__
# 	define	__ARRAY_H__

language_block ( C ) ;

/*-----------------------------------------------------------------------------------------------------------

	Macros & constants.
	
 *-----------------------------------------------------------------------------------------------------------*/
# define 	ARRAY_BUFFER_CLASS_NAME			"ArrayBuffer"
# define 	ARRAY_BUFFER_RESOURCE_NAME		"array_buffer"

# define 	ARRAY_BUFFER_MAX_SIZE_SETTING		"array_buffer.max_size"
# define 	DEFAULT_ARRAY_BUFFER_MAX_SIZE		"1Mb"
# define 	ARRAY_BUFFER_MIN_FREE_MEMORY_SETTING	"array_buffer.min_free_memory"
# define 	DEFAULT_ARRAY_BUFFER_MIN_FREE_MEMORY	"30%"


/*-----------------------------------------------------------------------------------------------------------

	Array buffer structures.
	
 *-----------------------------------------------------------------------------------------------------------*/
typedef struct array_buffer
   {
	void *		data ;
	long 		size ;
    }  array_buffer ;
    
 
/*-----------------------------------------------------------------------------------------------------------

	External variables.
	
 *-----------------------------------------------------------------------------------------------------------*/
ZEND_BEGIN_MODULE_GLOBALS ( array_buffer )					/* Module globals					*/
	int  	junk ;
ZEND_END_MODULE_GLOBALS ( array_buffer ) ;

extern int 			array_buffer_resource_id ;			/* Id of the resource associated with an array_buffer 	*/
extern thrak_byte_quantity 	array_buffer_max_size ;				/* Max array buffer size 				*/
 
 
/*-----------------------------------------------------------------------------------------------------------

	External array_buffer functions.
	
 *-----------------------------------------------------------------------------------------------------------*/
extern PHP_FUNCTION ( array_buffer_clone ) ;
extern PHP_FUNCTION ( array_buffer_copy ) ;
extern PHP_FUNCTION ( array_buffer_create ) ;
extern PHP_FUNCTION ( array_buffer_destroy ) ;
extern PHP_FUNCTION ( array_buffer_get ) ;
extern PHP_FUNCTION ( array_buffer_resize ) ;
extern PHP_FUNCTION ( array_buffer_set ) ;
extern PHP_FUNCTION ( array_buffer_size ) ;

extern void 		array_buffer_initialize		( THRAK_INIT_FUNC_ARGS ) ;
extern void 		array_buffer_shutdown		( THRAK_SHUTDOWN_FUNC_ARGS ) ;

/*-----------------------------------------------------------------------------------------------------------

	External array functions.
	
 *-----------------------------------------------------------------------------------------------------------*/
extern PHP_FUNCTION ( array_flatten ) ; 
extern PHP_FUNCTION ( iin_array ) ;
extern PHP_FUNCTION ( array_ikey_exists ) ;

 
extern void 		array_initialize		( THRAK_INIT_FUNC_ARGS ) ;
extern void 		array_shutdown			( THRAK_SHUTDOWN_FUNC_ARGS ) ;

end_language_block ( C ) ;

# endif		/* __ARRAY_H__  */