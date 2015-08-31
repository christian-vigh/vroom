/************************************************************************************************************

    NAME
	array.c
	
    DESCRIPTION
	Extra array functions.
	
    AUTHOR
	Christian Vigh, 10/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# include 	"php_vroom.h"
# include 	"array.h"


/*===========================================================================================================

    NAME
	array_initialize - initializes the array_buffer module.

 *===========================================================================================================*/
void 	array_initialize	( THRAK_INIT_FUNC_ARGS )
   {
    }

    
/*===========================================================================================================

    NAME
	array_shutdown
	
    DESCRIPTION
	Sub-extension shutdown function.
	
 *===========================================================================================================*/
void 	array_shutdown ( THRAK_SHUTDOWN_FUNC_ARGS )
   {
    }    

    
/*===========================================================================================================

    NAME
	array_flatten - Flattens an array.
	
    PROTOTYPE 
	$array 	=  array_flatten ( $array, $copy_keys = false ) ;
	
    DESCRIPTION
	Flattens an array so that it has only one nesting level.
	
    PARAMETERS 
	$array (array) -
		Array to be flattened.
		
	$copy_keys (boolean) -
		When true, array keys are copied into the flattened array, whenever possible.
		
    RETURNS
	The flattened array.
	
    NOTES
	. When both an outer array and a nested array contain the same associative key, the corresponding value
	  in the outer array will be overridden by the one of the nested array.
	  Example :
	  
		print_r ( array_flatten ( [ 'a' => 1, [ 'a' => 'nested value', 'b' => 1 ] ] ) ) ;
		
	  will output :
	  
		[ 'a' => 'nested value', 'b' => 1 ]
		
	. Keys will be lost during the flattening process if their value is a nested array :
	
		print_r ( array_flatten ( [ 1, 'nested_array' => [ 2, 3 ] ] ) ) ;
		
	  will output :
	  
		[ 1, 2, 3 ] 
	
 *===========================================================================================================*/
static zend_always_inline void  __array_flatten ( zval *  output_array, zval *  input_array, zend_bool  copy_keys )
   {
	HashTable *		array_hash 	=  Z_ARRVAL_P ( input_array ) ;
	HashPosition 		array_position ;
	char * 			key ;
	ulong 			key_length ;
	ulong 			index ;
	zval ** 		item ;
	int  			key_type ;


	/* Loop through array items */
	for  ( zend_hash_internal_pointer_reset_ex ( array_hash, & array_position ) ; 
			zend_hash_get_current_data_ex ( array_hash, ( void ** ) & item, & array_position )  ==  SUCCESS ; 
			zend_hash_move_forward_ex ( array_hash, & array_position ) ) 
	   {
		/* Get array key for current position */	
		key_type 	=  zend_hash_get_current_key_ex ( array_hash, & key, & key_length, & index, 0, & array_position ) ;

		/* Add a reference to the item */
		Z_ADDREF_PP ( item ) ;

		// If array given, recursively process its contents
		if  ( Z_TYPE_PP ( item )  ==  IS_ARRAY )
		   {
			__array_flatten ( output_array, * item, copy_keys ) ;
		    }
		else
		   {
# ifdef 	PHP_WIN32
# 	pragma 		warning ( disable : 4267 )
# endif
			// If the key type is string, then add the current item with an associative index
			if  ( key_type  ==  HASH_KEY_IS_STRING  &&  copy_keys )
			   {
				add_assoc_zval_ex ( output_array, key, key_length, * item ) ;
			    }
			// Otherwise, simply add the current item
			else 
				add_next_index_zval ( output_array, * item ) ;
# ifdef 	PHP_WIN32
# 	pragma 		warning ( default : 4267 )
# endif
		    }
	    }
    }
    
PHP_FUNCTION ( array_flatten )
   {
	zval *		input_array ;
	zend_bool	copy_keys 	=  0 ;
	     
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "a|b", & input_array, & copy_keys )  ==  FAILURE )
		return ;
		
	/* Initialize output array */
	array_init ( return_value ) ;
	
	/* Run the recursive input array processing */
	__array_flatten ( return_value, input_array, copy_keys ) ;
    }
 
 
 /*===========================================================================================================

    NAME
	iin_array - Case-insensitive search in array.
	
    PROTOTYPE 
	$bool 	=  iin_array ( $needle, $haystack ) ;
	
    DESCRIPTION
	Performs a case-insensitive search of the value $needle in the array $haystack.
	
    PARAMETERS 
	$needle (mixed) -
		Value to be searched.
		
	$haystack (array) -
		Array to be searched.
		
    RETURNS
	True if the array contains the specified value, false otherwise.
	
 *===========================================================================================================*/
 
 /***
	Grrr ! in_array uses compare_function(), which has no kcase-insensitive equivalent...
	This is simply an adaptation of the original.
 ***/
#define TYPE_PAIR(t1,t2) (((t1) << 4) | (t2))

static zend_always_inline int 	__case_compare_function ( zval *  result, zval *  op1, zval *  op2, int  identical TSRMLS_DC  )  
   {
	switch ( TYPE_PAIR ( Z_TYPE_P ( op1 ), Z_TYPE_P ( op2 ) ) ) 
	   {
		case TYPE_PAIR ( IS_STRING, IS_STRING ) :
			Z_LVAL_P ( result ) 	=  zend_binary_zval_strcasecmp ( op1, op2 ) ;
			ZVAL_LONG ( result, ZEND_NORMALIZE_BOOL ( Z_LVAL_P ( result ) ) ) ;				
			return ( SUCCESS ) ;
			
		default :
			return ( ( identical ) ? 
					is_equal_function 	( result, op1, op2 TSRMLS_CC ) :
					is_identical_function   ( result, op1, op2 TSRMLS_CC )
				) ;
	    }
    }
   
static zend_always_inline int __is_case_equal_function ( zval *  result, zval *  op1, zval *  op2 TSRMLS_DC )
   {
	if  ( __case_compare_function (result, op1, op2, 0 TSRMLS_CC )  ==  FAILURE)
		return ( FAILURE ) ;
		
	ZVAL_BOOL ( result, ( Z_LVAL_P ( result )  ==  0 ) ) ;
	return ( SUCCESS ) ;
    }
    
    
static zend_always_inline int __is_case_identical_function ( zval *  result, zval *  op1, zval *  op2  TSRMLS_DC )
   {
	if  ( __case_compare_function (result, op1, op2, 1 TSRMLS_CC )  ==  FAILURE)
		return ( FAILURE ) ;
		
	ZVAL_BOOL ( result, ( Z_LVAL_P ( result )  ==  0 ) ) ;
	return ( SUCCESS ) ;
    }
    
    
PHP_FUNCTION ( iin_array )
   {
	zval *		value,				/* value to check for 		*/
	     *		array,				/* array to check in 		*/
	     **		entry,				/* pointer to array entry 	*/
			res ;				/* comparison result 		*/
	HashTable *	hash ;				/* Array hash 			*/
	HashPosition 	pos ;				/* hash iterator 		*/
	zend_bool 	strict =  0 ;			/* strict comparison or not 	*/
	int 		( * is_equal_func ) ( zval *, zval *, zval * TSRMLS_DC) 
				=  __is_case_equal_function;

	
	/*  Parse parameters */
	if ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "za|b", & value, & array, & strict )  ==  FAILURE )
		return ;

	/* Use strict comparison function if needed */
	if  ( strict )
		is_equal_func = __is_case_identical_function ;

	/* Loop through array entries */
	hash 	=  Z_ARRVAL_P ( array ) ;
	zend_hash_internal_pointer_reset_ex ( hash, & pos ) ;
	
	while  ( zend_hash_get_current_data_ex ( hash, ( void ** ) & entry, & pos )  ==  SUCCESS ) 
	   {
		is_equal_func ( & res, value, * entry TSRMLS_CC ) ; 
		
		if  ( Z_LVAL ( res ) ) 
			RETURN_TRUE ;
		
		zend_hash_move_forward_ex ( hash, & pos ) ;
	    }

	RETURN_FALSE ;
    }
 
 
  /*===========================================================================================================

    NAME
	array_ikey_exists - Case-insensitive search in array keys.
	
    PROTOTYPE 
	$bool 	=  array_ikey_exists ( $needle, $haystack ) ;
	
    DESCRIPTION
	Performs a case-insensitive search of the value $needle in the keys of the array $haystack.
	
    PARAMETERS 
	$needle (mixed) -
		Key to be searched.
		
	$haystack (array) -
		Array to be searched.
		
    RETURNS
	True if the array contains the specified key, false otherwise.
	
 *===========================================================================================================*/
 PHP_FUNCTION ( array_ikey_exists )
   {
	zval *		key ;					/* key to check for 		*/
	HashTable *	array_hash ;				/* array to check in 		*/
	HashPosition 	array_position ;			/* current array position 	*/
	char *		array_key ;
	int 		array_key_length ;
	int  		array_key_index ;
	int  		array_key_type ;
	int  		length ;

	
	/* Check parameters */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "zH", & key, & array_hash )  ==  FAILURE )
		return ;

	/* Search key depending on its type */
	switch  ( Z_TYPE_P ( key ) ) 
	   {
		case 	IS_STRING :
			zend_hash_internal_pointer_reset_ex ( array_hash, & array_position ) ;
			length 	=  Z_STRLEN_P ( key ) ;
		
			while  ( ( array_key_type = zend_hash_get_current_key_ex ( 
								array_hash, & array_key, & array_key_length, & array_key_index, 0, & array_position ) )  !=  HASH_KEY_NON_EXISTENT ) 
			   {
				if  ( length  ==  array_key_length - 1  && 
						! strncasecmp ( Z_STRVAL_P ( key ), array_key, length ) )
					RETURN_TRUE ;
		
				zend_hash_move_forward_ex ( array_hash, & array_position ) ;
			    }
			    
			RETURN_FALSE ;
			
		case IS_LONG :
			if  ( zend_hash_index_exists ( array_hash, Z_LVAL_P ( key ) ) )
				RETURN_TRUE ;
			RETURN_FALSE ;
			
		case IS_NULL :
			if  ( zend_hash_exists ( array_hash, "", 1 ) )
				RETURN_TRUE;
			RETURN_FALSE ;

		default :
			php_error_docref ( NULL TSRMLS_CC, E_WARNING, "The first argument should be either a string or an integer" ) ;
			RETURN_FALSE ;
	    }
    }