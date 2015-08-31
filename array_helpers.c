/************************************************************************************************************

    NAME
	ini_helpers.c
	
    DESCRIPTION
	Helper functions for parsing .INI file values.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# include 	"thrak.h"
# include 	<ctype.h>
# include 	<math.h>


/*===========================================================================================================

    NAME
	thrak_array_to_char - Converts an array to an array of C-strings.
	
    PROTOTYPE 
	char *  thrak_array_to_char ( zval *  array, char ***  argv, int *  argc, int **  argl ) ;
	
    DESCRIPTION
	Converts an array (of strings) to an array of string
		
    PARAMETERS
	array (zval *) -
		A pointer to a zval containing either an array or a scalar value.
		If a scalar value is supplied, it will be first converted to string and an array of 1 element
		will be created.
		
	argv (char **) -
		Address of a char * array that will receive the array of pointers to individual zero-terminated
		strings.
		Note that this array contains (argc + 1) elements ; the last one being a NULL pointer to signal
		the end of the array.
		
	argc (int *) -
		Address of an integer that will receive the number of elements in the input array.
		
	argl (int **) -
		If not NULL, will receive the respective lengths of each string in argv. Last item (which 
		corresponds to the last NULL item in argv) will be set to -1.
		
    RETURNS
	A pointer to the memory block allocated to store all the individual array strings. This memory must
	then be freed using the thrak_free_array_to_char() function.
	
 *===========================================================================================================*/
THRAK_API char * 	thrak_array_to_char ( zval *  array, char ***  argv, int *  argc, int **  argl )
   {
	HashTable *		array_hash ;
	HashPosition 		array_position ;
	zval ** 		item ;
	char *			buffer ;
	int 			total_size 	=  1 ;
	char **			arg_values ;
	int  			arg_count ;
	int *			arg_lengths ;
	char * 			p ;
	int 			current_argc,
				length ;
	int 			argv_length,
				argl_length ;
	

	/* Ban forbidden zval types, retain only ARRAY and STRING types, and convert all other scalar types to string */
	switch  ( Z_TYPE_P ( array ) )
	   {
		case 	IS_OBJECT :
		case 	IS_RESOURCE :
		case	IS_NULL :
			return ( NULL ) ;
			
		case 	IS_ARRAY :
		case 	IS_STRING :
			break ;
			
		default :
			convert_to_string_ex ( & array ) ;
	    }
	    
	/* Compute the size needed to hold all the strings supplied in the input array */
	if  ( Z_TYPE_P ( array )  ==  IS_ARRAY )
	   {
		/* Get hash from array */
		array_hash 	=  Z_ARRVAL_P ( array ) ;
		
		/* Get item count and allocate this count + one element (the last element being a NULL pointer) */
		arg_count 	=  zend_hash_num_elements ( array_hash ) ;
		
		/* A first pass through the input array to compute the total size needed */
		for  ( zend_hash_internal_pointer_reset_ex ( array_hash, & array_position ) ; 
				zend_hash_get_current_data_ex ( array_hash, ( void ** ) & item, & array_position )  ==  SUCCESS ; 
				zend_hash_move_forward_ex ( array_hash, & array_position ) ) 
		   {
			/* Ignore non-scalar types and convert non-strings to strings */
			switch ( Z_TYPE_PP ( item ) )
			   {
				case 	IS_OBJECT :
				case 	IS_RESOURCE :
				case 	IS_ARRAY :
				case 	IS_NULL :
					continue ;
					
				case 	IS_STRING :
					break ;
					
				default :
					convert_to_string_ex ( item ) ;
			    }
			    
			/* Update total needed size */
			total_size 	+=  Z_STRLEN_PP ( item ) + 1 ;
		    }
		    
		/*** 
			The buffer that will be allocated has the following layout :
			. offset 0 					: argv 
			. offset sizeof ( char * ) * ( argc + 1 ) 	: argl (optional)
			. offset argl + ( sizeof ( int ) * ( argc + 1 )	: zero-terminated strings
			. last byte 					: 0, to indicate the end of zero-terminated strings
			
		 ***/
		argv_length 	=  ( ( arg_count + 1 ) * sizeof ( char * ) ) ;
		argl_length 	=  ( argl  ==  NULL ) ?  0 : ( ( arg_count + 1 ) * sizeof ( int ) ) ;
		
		buffer 	=  emalloc ( argv_length + argl_length + total_size ) ;
		
		if  ( buffer  ==  NULL )
			return ( NULL ) ;
			
		/* Set the pointers to the correct offset into buffer */
		arg_values 	=  ( char ** ) buffer ;
		
		if  ( argl  !=  NULL )
		   {
			arg_lengths 	=  ( int * ) ( buffer + argv_length ) ;
			p 		=  buffer + argv_length + argl_length ;
		    }
		else 
			p 		=  buffer + argv_length ;
		    
		/* Second pass : copy input array values to buffer and update argv accordingly */
		current_argc 	=  0 ;
		
		for  ( zend_hash_internal_pointer_reset_ex ( array_hash, & array_position ) ; 
				zend_hash_get_current_data_ex ( array_hash, ( void ** ) & item, & array_position )  ==  SUCCESS ; 
				zend_hash_move_forward_ex ( array_hash, & array_position ) ) 
		   {
			/* Ignore non-scalar types and convert non-strings to strings */
			switch ( Z_TYPE_PP ( item ) )
			   {
				case 	IS_OBJECT :
				case 	IS_RESOURCE :
				case 	IS_ARRAY :
				case 	IS_NULL :
					continue ;
					
				case 	IS_STRING :
					break ;
					
				default :
					convert_to_string_ex ( item ) ;
			    }
			    
			/* Add current item and make it nul-terminated */
			length 		=  Z_STRLEN_PP ( item ) ;
			memcpy ( p, Z_STRVAL_PP ( item ), length ) ;
			
			* ( p + length ) 	=  0 ;
			
			/* Add it to the argv array */
			arg_values [ current_argc ] 		=  p ;
			
			if  ( argl  !=  NULL )
				arg_lengths [ current_argc ]	=  length ;
				
			p 	+= length + 1 ;
			current_argc ++ ;
		    }
	    }
	/* Input value is not an array ; if scalar type, we will convert it to a string and put it in a 1-element argv array */
	else
	   {
		/* Ignore non-scalar types and convert non-strings to strings */
		switch ( Z_TYPE_P ( array ) )
		   {
			case 	IS_OBJECT :
			case 	IS_RESOURCE :
			case 	IS_NULL :
				return ( NULL ) ;
				
			case 	IS_STRING :
				break ;
				
			default :
				convert_to_string_ex ( & array ) ;
		    }

		length 		 =  Z_STRLEN_P ( array ) ;
		total_size 	+=  length + 1 ;
		
		arg_count 	=  1 ;
		argv_length 	=  ( ( arg_count + 1 ) * sizeof ( char * ) ) ;
		argl_length 	=  ( argl  ==  NULL ) ?  0 : ( ( arg_count + 1 ) * sizeof ( int ) ) ;
		
		buffer 	=  emalloc ( argv_length + argl_length + total_size ) ;
		
		if  ( buffer  ==  NULL )
			return ( NULL ) ;
			
		/* Set the pointers to the correct offset into buffer */
		arg_values 	=  ( char ** ) buffer ;
		
		if  ( argl  !=  NULL )
		   {
			arg_lengths 	=  ( int * ) ( buffer + argv_length ) ;
			p 		=  buffer + argv_length + argl_length ;
		    }
		else 
			p 		=  buffer + argv_length ;
		
		current_argc 	=  0 ;
		
		/* Copy the string value in it */
		strncpy ( p, Z_STRVAL_P ( array ), length ) ;
		
		/* Terminate by a double nul character (one for the string, one for signalling the end of the string list) */
		p [ length ] = p [ length + 1 ] = 0 ;
		
		/* Add it to the argv array */
		arg_values [0] 	=  p ;
		
		if  ( argl  !=  NULL )
			arg_lengths [0] 	=  length ;
			
		current_argc ++ ;
	    }
	    
	/* Make sure the last array item is a NULL pointer */
	arg_values [ current_argc ] 	=  NULL ;
	
	if  ( argl  !=  NULL )
	   {
		arg_lengths [ current_argc ] 	=  -1 ;
		* argl 				=  arg_lengths ;
	    }
	
	/* Update caller supplied values */
	* argv 	=  arg_values ;
	* argc 	=  arg_count ;
	
	/* All done, return */
	return ( buffer ) ;
    }
    
    
    
/*===========================================================================================================

    NAME
	thrak_free_array_to_char - Frees memory previously allocated by thrak_array_to_char.
	
    PROTOTYPE 
	zend_bool  thrak_free_array_to_char ( char *  data ) ;
	
    DESCRIPTION
	Frees memory previously allocated by thrak_free_array_to_char().
		
    PARAMETERS
	data (char *) -
		A pointer returned by thrak_array_to_char().
		
    RETURNS
	1 if memory has been successfully freed, 0 otherwise.
	
 *===========================================================================================================*/
 THRAK_API zend_bool 	thrak_free_array_to_char ( char *  data )
    {
	if  ( data  !=  NULL )
	   {
		efree ( data ) ;
		return ( 1 ) ;
	    }
	else
		return ( 0 ) ;
     }