/************************************************************************************************************

    NAME
	array_buffer.h
	
    DESCRIPTION
	Implementation of buffers for scalar arrays.
	
    AUTHOR
	Christian Vigh, 10/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# include 	"php_vroom.h"
# include 	"array.h"

/*-----------------------------------------------------------------------------------------------------------

	Global variables.
	
 *-----------------------------------------------------------------------------------------------------------*/
 
/* Module globals */
ZEND_DECLARE_MODULE_GLOBALS ( array_buffer ) ;

/* Id of the resource associated with an array_buffer */
int 					array_buffer_resource_type_id ;		

/* Max array buffer size */
static thrak_byte_quantity		ini_setting_max_size ; 
/* Array buffers will not be allocated if available memory falls below this limit */
static thrak_memory_quantity		ini_setting_min_free_memory ;


/*===========================================================================================================

    NAME
	array_buffer_resource_destructor - Destroys a resource of array_buffer type.

 *===========================================================================================================*/
static void  array_buffer_resource_destructor ( zend_rsrc_list_entry *  resource TSRMLS_DC )  
   {
        array_buffer *	abp 	=  ( array_buffer * ) resource -> ptr ;

	if  ( abp  !=  NULL )		// Paranoid checks...
	   {
		if (  abp -> data  !=  NULL )		// Free array data
			efree ( abp -> data ) ;
			
		efree ( abp ) ;
	    }
    }

    
    
/*===========================================================================================================

    NAME
	array_buffer_initialize - initializes the array_buffer module.

 *===========================================================================================================*/
static void  array_buffer_init_globals ( THRAK_GLOBALS_TYPE ( array_buffer ) *  globals )
   { }

void 	array_buffer_initialize	( THRAK_INIT_FUNC_ARGS )
   {
	char * 		ini_value ;
	int 		status ;
	
	/* Initialize module globals */
	THRAK_INIT_GLOBALS ( array_buffer, array_buffer_init_globals ) ;
	
	/* Register a resource of type "array_buffer" */
	array_buffer_resource_type_id	=  zend_register_list_destructors_ex ( array_buffer_resource_destructor, NULL, 
								ARRAY_BUFFER_RESOURCE_NAME, module_number ) ;
								
	/* INI setting : array_buffer.max_size. Allows for Kilobytes quantities (suffixed by "K" or "KB") and megabytes ("M" or "MB") */
	ini_value 	=  thrak_ini_get_string ( ARRAY_BUFFER_MAX_SIZE_SETTING, DEFAULT_ARRAY_BUFFER_MAX_SIZE ) ;
	status 		=  thrak_ini_parse_byte_quantity ( ini_value, & ini_setting_max_size ) ;

	if (  ! status )
		php_error_docref ( NULL TSRMLS_CC, E_NOTICE, "Incorrect value \"%s\" for the \"%s\" ini setting.\n", ini_value, ARRAY_BUFFER_MAX_SIZE_SETTING ) ;

	efree ( ini_value ) ;
	
	/* INI setting : array_buffer.min_free_memory. */
	ini_value 	=  thrak_ini_get_string ( ARRAY_BUFFER_MIN_FREE_MEMORY_SETTING, DEFAULT_ARRAY_BUFFER_MIN_FREE_MEMORY ) ;
	status 		=  thrak_ini_parse_memory_quantity ( ini_value, & ini_setting_min_free_memory ) ;

	if (  ! status )
		php_error_docref ( NULL TSRMLS_CC, E_NOTICE, "Incorrect value \"%s\" for the \"%s\" ini setting.\n", ini_value, ARRAY_BUFFER_MIN_FREE_MEMORY_SETTING ) ;

	efree ( ini_value ) ;
	
    }

    
/*===========================================================================================================

    NAME
	array_buffer_shutdown
	
    DESCRIPTION
	Sub-extension shutdown function.
	
 *===========================================================================================================*/
void 	array_buffer_shutdown ( THRAK_SHUTDOWN_FUNC_ARGS )
   {
    }    
    

/*===========================================================================================================

    NAME
	array_buffer_clone - Clones a buffer.
	
    PROTOTYPE 
	$new_resource = array_buffer_clone ( $resource ) ;
	
    DESCRIPTION
	Clones an array buffer.
	
    PARAMETERS
	$resource (resource) -
		Array buffer resource to be cloned.
		
    RETURNS
	A resource that points to the clone of the specified input array buffer, or false if something went
	wrong.
	
 *===========================================================================================================*/
PHP_FUNCTION ( array_buffer_clone )
   {
	zval * 			z_resource ;
	array_buffer *		input_buffer ;
	array_buffer *		cloned_buffer ;

	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "r", & z_resource )  ==  FAILURE )
		return ;

	/* Error handling (bad resource id) is performed by ZEND_FETCH_RESOURCE */
	ZEND_FETCH_RESOURCE ( input_buffer, array_buffer *, & z_resource, -1, ARRAY_BUFFER_RESOURCE_NAME, array_buffer_resource_type_id ) ;

	/* Check memory constraints */
	if  ( ! thrak_memory_check_constraint ( sizeof ( array_buffer ) + input_buffer -> size, & ini_setting_min_free_memory TSRMLS_CC ) )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Not enough memory to allocate %d bytes", sizeof ( array_buffer ) + input_buffer -> size ) ;
		RETURN_FALSE ;
	    }
	
	/* Allocate a new array buffer */
	cloned_buffer 	=  emalloc ( sizeof ( array_buffer ) ) ;
	
	if  ( cloned_buffer  ==  NULL )
		RETURN_FALSE ;
		
	cloned_buffer -> data 	=  emalloc ( input_buffer -> size ) ;
	
	if  ( cloned_buffer -> data  ==  NULL )
	   {
		efree ( cloned_buffer ) ;
		RETURN_FALSE ;
	    }
	    
	/* Perform the copy */
	memcpy ( cloned_buffer -> data, input_buffer -> data, input_buffer -> size ) ;
	cloned_buffer -> size 	=  input_buffer -> size ;
	
	/* Then register this new "array_buffer" resource */
	ZEND_REGISTER_RESOURCE ( return_value, cloned_buffer, array_buffer_resource_type_id ) ;
    }
    
    
/*===========================================================================================================

    NAME
	array_buffer_copy - Copies an array buffer over an existing one.
	
    PROTOTYPE 
	$bytes_copied = array_buffer_copy ( $dst_resource, $src_resource, $num_bytes = 0, 
						$dst_offset = 0, $src_offset = 0 ) ;
	
    DESCRIPTION
	Copies the whole or a part of an array buffer over an existing one. Only the bytes that can fit into
	the destination buffer array will be copied.
	
    PARAMETERS
	$dst_resource (resource) -
		Destination array buffer.
		
	$src_resource (resource) -
		Source array buffer.
		
	$num_bytes (integer) -
		Number of bytes to copy. If unspecified or zero, the whole contents of the source array buffer
		will be copied (up to destination buffer size).
		
	$dst_offset (integer) -
		Offset where to start the copy in the destination buffer.
		Defaults to zero.
		
	$src_offset (integer) -
		Offset where to start the copy from the source buffer.
		Defaults to zero.
		
    RETURNS
	The number of bytes copied, or false is some invalid parameters were specified.
	
 *===========================================================================================================*/
PHP_FUNCTION ( array_buffer_copy )
   {
	zval *			z_source ;					/* Parameter values */
	zval *			z_destination ;
	array_buffer *		source_buffer ;
	array_buffer *		destination_buffer ;
	long 			num_bytes 		=  0 ;
	long 			source_offset		=  0 ;
	long 			destination_offset	=  0 ;			/* End of parameter values */
	long 			destination_byte_count,				/* Number of bytes that can be copied into the destination */
				source_byte_count ;				/* Number of bytes that can be copied from the source */

	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "rr|lll", 
			& z_source, & z_destination, & num_bytes, & destination_offset, & source_offset )  ==  FAILURE )
		return ;

	/* Check byte count + source & destination offsets */
	if  ( num_bytes  <  0 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "number of bytes to be copied must be a positive integer" ) ;
		RETURN_FALSE ;
	    }

	if  ( destination_offset  <  0 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "destination offset must be a positive integer" ) ;
		RETURN_FALSE ;
	    }
	    
	if  ( source_offset  <  0 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "source offset must be a positive integer" ) ;
		RETURN_FALSE ;
	    }
	    
	/* Get destination & source buffers */
	ZEND_FETCH_RESOURCE ( destination_buffer, array_buffer *, & z_destination, -1, ARRAY_BUFFER_RESOURCE_NAME, array_buffer_resource_type_id ) ;
	ZEND_FETCH_RESOURCE ( source_buffer     , array_buffer *, & z_source     , -1, ARRAY_BUFFER_RESOURCE_NAME, array_buffer_resource_type_id ) ;
	
	/* Compute byte count limits */
	if  ( destination_offset  >=  destination_buffer -> size )
		RETURN_FALSE ;
		
	if  ( source_offset  >=  source_buffer -> size )
		RETURN_FALSE ;
		
	destination_byte_count 	=  ( long ) min ( destination_buffer -> size - destination_offset, num_bytes ) ;
	source_byte_count 	=  ( long ) min ( source_buffer -> size - source_offset, num_bytes ) ;
	num_bytes 		=  min ( source_byte_count, destination_byte_count ) ;
	
	/* Perform the copy */
	memcpy ( ( ( char * ) destination_buffer -> data ) + destination_offset, ( ( char * ) source_buffer -> data ) + source_offset, num_bytes ) ;
	
	RETURN_LONG ( num_bytes ) ;
    }
    
    
/*===========================================================================================================

    NAME
	array_buffer_create - Creates an array buffer.
	
    PROTOTYPE 
	$resource 	=  array_buffer_create ( $size ) ;
	
    DESCRIPTION
	Creates an array buffer of the specified size.
	
    PARAMETERS
	$size (int) -
		Array buffer size, in bytes.
	
     RETURN VALUE
	A resource identifying the allocated array buffer, or false if something went wrong.
	
 *===========================================================================================================*/
PHP_FUNCTION ( array_buffer_create )
  {
	long 			size ;
	array_buffer *		buffer ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "l", & size )  ==  FAILURE )
		return ;
  
	/* Check that the specified size is positive and less than or equal to max array size */
	if  ( size  <=  0 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Array buffer size must be a positive integer" ) ;
		RETURN_FALSE ;
	    }
	else if  ( size  >  ini_setting_max_size. bytes )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_NOTICE, "Array buffer size exceeds maximum size (%d bytes) ; array size adjusted to this limit",
			ini_setting_max_size ) ;
		size	=  ini_setting_max_size. bytes ;
	    }
	    
	/* Check memory constraints */
	if  ( ! thrak_memory_check_constraint ( sizeof ( array_buffer ) + size, & ini_setting_min_free_memory TSRMLS_CC ) )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Not enough memory to allocate %d bytes", sizeof ( array_buffer ) + size ) ;
		RETURN_FALSE ;
	    }
	
	/* Allocate the buffer */
	buffer 		=  ( array_buffer * ) emalloc ( sizeof ( array_buffer ) ) ;
	
	if  ( buffer  ==  NULL )
		RETURN_FALSE ;
		
	buffer -> data 	=  emalloc ( size ) ;
	
	if  ( buffer -> data  ==  NULL )
	   {
		efree ( buffer ) ;
		RETURN_FALSE ;
	    }
	    
	buffer -> size 		=  size ;
	memset ( buffer -> data, 0, size ) ;
	
	/* Then register this new "array_buffer" resource */
	ZEND_REGISTER_RESOURCE ( return_value, buffer, array_buffer_resource_type_id ) ;
   }

   
/*===========================================================================================================

    NAME
	array_buffer_free - Frees a resource associated to an array buffer.
	
    PROTOTYPE 
	array_buffer_free ( $resource ) ;
	
    DESCRIPTION
	Frees the resources associated with the specified array buffer resource.
	
    PARAMETERS
	$resource (resource) -
		Array buffer resource.
	
 *===========================================================================================================*/
PHP_FUNCTION ( array_buffer_destroy )
   {
	zval * 		z_resource ;

	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "r", & z_resource )  ==  FAILURE )
		return ;

	zend_list_delete ( Z_LVAL_P ( z_resource ) ) ;
    }
    

/*===========================================================================================================

    NAME
	array_buffer_get - Gets a byte.
	
    PROTOTYPE 
	array_buffer_get ( $resource, $offset, $num_bytes = 1 ) ;
	
    DESCRIPTION
	Retrieves a byte at the specified offset.
	
    PARAMETERS
	$resource (resource) -
		Array buffer resource.
		
	$offset (int) -
		Offset of the byte to be retrieved.
		
	$num_bytes (integer) -
		Number of bytes to retrieve.
	
    RETURNS
	When $num_bytes is 1 (the default), array_buffer_get() returns the byte value at the specified offset 
	of the array buffer.
	When $num_bytes is greater than 1, consecutive bytes are returned as a binary string.
	The function returns false if the offset is invalid.
	
 *===========================================================================================================*/
PHP_FUNCTION ( array_buffer_get )
   {
	zval * 			z_resource ;
	long 			offset ;
	long 			num_bytes 		=  1 ;
	array_buffer *		buffer ;
	char 			c ;

	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "rl|l", & z_resource, & offset, & num_bytes )  ==  FAILURE )
		return ;

	/* Get the array buffer */
	ZEND_FETCH_RESOURCE ( buffer, array_buffer *, & z_resource, -1, ARRAY_BUFFER_RESOURCE_NAME, array_buffer_resource_type_id ) ;

	/* Check offset */
	if  ( offset  <  0 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Array buffer offset must be a positive integer" ) ;
		RETURN_FALSE ;
	    }
	else if  ( offset  >=  buffer -> size )
		RETURN_FALSE ;
		
	// Check byte count
	if  ( num_bytes  <  1 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "byte count must be a strictly positive integer" ) ;
		RETURN_FALSE ;
	    }
		
	// Get the character at the specified offset if only one byte is to be retrieved
	if  ( num_bytes  ==  1 )
	   {
		c 	=  * ( ( ( ( char * ) buffer -> data ) ) + offset ) ;
	
		RETURN_LONG ( c ) ;
	    }
	// Otherwise, return at most $num_bytes as a binary string
	else
	   {
		num_bytes 	=  ( long ) min ( buffer -> size - offset, num_bytes ) ;
		
		ZVAL_STRINGL ( return_value, ( ( char * ) buffer -> data ) + offset, num_bytes, 1 ) ;
	    }
    }
  
  
  /*===========================================================================================================

    NAME
	array_buffer_resize - Resizes an array buffer.
	
    PROTOTYPE 
	array_buffer_resize ( $resource, $new_size ) ;
	
    DESCRIPTION
	Resizes an array buffer.
	
    PARAMETERS
	$resource (resource) -
		Array buffer resource.
		
	$size (int) -
		New size for the array buffer. If less than the current size, existing values above will be
		truncated.
	
    RETURNS
	True if the array buffer has been resized or remains unchanged because the new size is equal to the
	actual size, false otherwise.
	
 *===========================================================================================================*/
PHP_FUNCTION ( array_buffer_resize )
   {
	zval * 			z_resource ;
	long 			new_size ;
	array_buffer *		buffer ;
	void *			new_data ;

	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "rl", & z_resource, & new_size )  ==  FAILURE )
		return ;

	/* Error handling (bad resource id) is performed by ZEND_FETCH_RESOURCE */
	ZEND_FETCH_RESOURCE ( buffer, array_buffer *, & z_resource, -1, ARRAY_BUFFER_RESOURCE_NAME, array_buffer_resource_type_id ) ;

	/* Check new size */
	if  ( new_size  <=  0 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Array buffer size must be a positive integer" ) ;
		RETURN_FALSE ;
	    }
	else if  ( new_size  >  ini_setting_max_size. bytes )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_NOTICE, "Array buffer size exceeds maximum size (%d bytes) ; array size adjusted to this limit",
			ini_setting_max_size ) ;
		new_size	=  ini_setting_max_size. bytes ;
	    }
	    
	/* Do nothing if new size is equal to old size */
	if  ( new_size  ==  buffer -> size )
		RETURN_TRUE ;

	/* Check memory constraint */
	if  ( new_size  >  buffer -> size )
	   {
		if  ( ! thrak_memory_check_constraint ( new_size - buffer -> size, & ini_setting_min_free_memory TSRMLS_CC ) )
		   {
			php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Not enough memory to allocate %d bytes", new_size - buffer -> size ) ;
			RETURN_FALSE ;
		    }
	    }
		
	/* Resize the array buffer */
	new_data 	=  erealloc ( buffer -> data, new_size ) ;
	
	if (  new_data  ==  NULL )	/* Allocation error */
		RETURN_FALSE ;

	/* If size increased, initialize the extra bytes */
	if  ( new_size  >  buffer -> size )
		memset ( ( ( char * ) new_data ) + buffer -> size, 0, new_size - buffer -> size ) ;
	
	/* All done, update the array buffer structure */
	buffer -> data		=  new_data ;
	buffer -> size 		=  new_size ;
	
	RETURN_TRUE ;
    }
    
    
/*===========================================================================================================

    NAME
	array_buffer_set - Initializes an array buffer.
	
    PROTOTYPE 
	$bytes_set = array_buffer_set ( $resource, $offset = 0, $init_value = 0, $num_bytes = 0  ) ;
	
    DESCRIPTION
	Initializes a whole or a range of an array buffer to the specified value.
	
    PARAMETERS
	$resource (resource) -
		Array buffer to be initialized.
		
	$init_value (integer) -
		Initialization value.
		
	$num_bytes (integer) -
		Number of bytes to initialize. If unspecified or zero, the whole contents of the array buffer
		are initialized.
		
	$offset (integer) -
		Offset where to start initialization from.
		Defaults to zero.
		
    RETURNS
	The number of bytes initialized, or false is some invalid parameters were specified.
	
 *===========================================================================================================*/
PHP_FUNCTION ( array_buffer_set )
   {
	zval *			z_buffer ;					/* Parameter values */
	zval *			z_char_or_string ;
	array_buffer *		buffer ;
	long 			init_value 		=  0 ;
	long 			num_bytes 		=  1 ;
	long 			offset			=  0 ;

	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "r|lzl", 
			& z_buffer, & offset, & z_char_or_string, & num_bytes )  ==  FAILURE )
		return ;

	/* Check byte count + source & destination offsets */
	if  ( num_bytes  <  0 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "number of bytes to be initialized must be a positive integer" ) ;
		RETURN_FALSE ;
	    }

	if  ( offset  <  0 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "offset must be a positive integer" ) ;
		RETURN_FALSE ;
	    }
	    
	/* Get array buffer */
	ZEND_FETCH_RESOURCE ( buffer, array_buffer *, & z_buffer, -1, ARRAY_BUFFER_RESOURCE_NAME, array_buffer_resource_type_id ) ;
	
	/* Compute byte count limits */
	if  ( offset  >=  buffer -> size )
		RETURN_FALSE ;
		
	num_bytes 	=  ( long ) min ( ( buffer -> size - offset ), num_bytes ) ;
	
	// Initialize
	memset ( ( ( char * ) buffer -> data ) + offset, init_value, num_bytes ) ;
	
	RETURN_LONG ( num_bytes ) ;
    }
    
    
/*===========================================================================================================

    NAME
	array_buffer_size - Returns the size in bytes of an array buffer.
	
    PROTOTYPE 
	$size 	=  array_buffer_length ( $resource ) ;
	
    DESCRIPTION
	Returns the size in bytes of the array buffer associated to the specified resource.
	
    PARAMETERS
	$resource (resource) -
		Memtext resource.
		
    RETURN VALUE
	Size in bytes of the array buffer.
	
 *===========================================================================================================*/
PHP_FUNCTION ( array_buffer_size )
   {
	zval * 		z_resource ;
	array_buffer *	buffer  	=  NULL ;

	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "r", & z_resource )  ==  FAILURE )
		return ;
	
	/* Error handling (bad resource id) is performed by ZEND_FETCH_RESOURCE */
	ZEND_FETCH_RESOURCE ( buffer, array_buffer *, & z_resource, -1, ARRAY_BUFFER_RESOURCE_NAME, array_buffer_resource_type_id ) ;
	    
	RETURN_LONG ( ( long ) buffer -> size ) ;
    }

    
    