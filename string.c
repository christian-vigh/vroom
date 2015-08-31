/************************************************************************************************************

    NAME
	string.c
	
    DESCRIPTION
	String-related operations.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# include 	"php_vroom.h"
# include 	"string.h"


/*===========================================================================================================

    NAME
	string_initialize - Initializes the string functions.
		
 *===========================================================================================================*/
void 	string_initialize ( THRAK_INIT_FUNC_ARGS )
   {
	/* string_explode() and string_implode() options */
	REGISTER_LONG_CONSTANT ( "EXPLODE_NONE"			, EXPLODE_NONE			, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_TRIM_LEFT"		, EXPLODE_TRIM_LEFT		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_TRIM_RIGHT"		, EXPLODE_TRIM_RIGHT		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_TRIM"			, EXPLODE_TRIM			, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_ESCAPE_QUOTES"	, EXPLODE_ESCAPE_QUOTES		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_ESCAPE_DOUBLEQUOTES"	, EXPLODE_ESCAPE_DOUBLEQUOTES	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_NOCASE"		, EXPLODE_NOCASE		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_IGNORE_EMPTY"		, EXPLODE_IGNORE_EMPTY		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_TOUPPER"		, EXPLODE_TOUPPER		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_TOLOWER"		, EXPLODE_TOLOWER		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_UCFIRST"		, EXPLODE_UCFIRST		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_UCWORDS"		, EXPLODE_UCWORDS		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_UNESCAPE_QUOTES"	, EXPLODE_UNESCAPE_QUOTES	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "EXPLODE_UNESCAPE_DOUBLEQUOTES", EXPLODE_UNESCAPE_DOUBLEQUOTES	, CONST_CS | CONST_PERSISTENT ) ;

	REGISTER_LONG_CONSTANT ( "IMPLODE_NONE"			, IMPLODE_NONE			, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_TRIM_LEFT"		, IMPLODE_TRIM_LEFT		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_TRIM_RIGHT"		, IMPLODE_TRIM_RIGHT		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_TRIM"			, IMPLODE_TRIM			, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_ESCAPE_QUOTES"	, IMPLODE_ESCAPE_QUOTES		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_ESCAPE_DOUBLEQUOTES"  , IMPLODE_ESCAPE_DOUBLEQUOTES	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_NOCASE"	  	, IMPLODE_NOCASE 		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_IGNORE_EMPTY"		, IMPLODE_IGNORE_EMPTY		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_TOUPPER"		, IMPLODE_TOUPPER		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_TOLOWER"		, IMPLODE_TOLOWER		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_UCFIRST"		, IMPLODE_UCFIRST		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_UCWORDS"		, IMPLODE_UCWORDS		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_UNESCAPE_QUOTES"	, IMPLODE_UNESCAPE_QUOTES	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "IMPLODE_UNESCAPE_DOUBLEQUOTES", IMPLODE_UNESCAPE_DOUBLEQUOTES	, CONST_CS | CONST_PERSISTENT ) ;
    }


/*===========================================================================================================

    NAME
	string_shutdown
	
    DESCRIPTION
	Sub-extension shutdown function.
	
 *===========================================================================================================*/
void 	string_shutdown ( THRAK_SHUTDOWN_FUNC_ARGS )
   {
    }


    
/*===========================================================================================================

    NAME
	string_append - Fast string concatenation.
	
    PROTOTYPE
	string_append ( &$value, ... ) ;
	
    DESCRIPTION
	Appends series of values to the specified string.
	
    PARAMETERS
	$value (string) -
		String where to append values.
		
	$... (mixed) -
		Any argument that can be converted to a string and appended to $value.
	
    NOTE 
	This code is theorically faster than the inline string-catenation operator, except that calling a
	PHP_FUNCTION() has a cost.
	When catenating 100 000 times a set of value takes 170ms using the catenation operator, it takes 615ms
	for string_append(), 535 of them being consumed just by calling the PHP_FUNCTION().
	
 *===========================================================================================================*/
THRAK_API zend_bool 	internal_string_append ( zval *  z_string, int  argc, zval **  argv, char **  result, int *  result_length )
    {
	char *		data,
	     *		p ;
	int  		size 		=  Z_STRLEN_P ( z_string ) ;
	int  		i ;
	
	// Compute the size needed to hold all the appended strings
	for (  i = 0 ; i  <  argc ; i ++ )
	   {
		if  ( Z_ISREF_P ( argv [i] ) )
			SEPARATE_ZVAL ( & argv [i] ) ;
			
		convert_to_string ( argv [i] ) ;
		size 	+=  Z_STRLEN_P ( argv [i] ) ;
	    }
	    
	// Allocate enough memory for it (just reallocate existing data, this will avoid to copy the appended variable
	// initial value)
	data = p = erealloc ( Z_STRVAL_P ( z_string ), size + 1 ) ;
	
	if  ( data  ==  NULL )
		return ( 0 ) ;
		
	// Point at the end of initial value
	p += Z_STRLEN_P ( z_string ) ;
	
	for  ( i = 0 ; i  <  argc ; i ++, argv ++ )
	   {
		memcpy ( p, Z_STRVAL_PP ( argv ), Z_STRLEN_PP ( argv ) ) ;
		p += Z_STRLEN_PP ( argv ) ;
	    }
	 
	/* Useless but makes me happy */
	* p = 0 ;
	
	// Give the resulting string to the caller
	* result 		=  data ;
	* result_length 	=  size ;
	
	return ( 1 ) ;
     }
     
     
 PHP_FUNCTION ( string_append )
    {
	zval *		z_string ;
	zval ***	argv 		=  NULL ;
	int  		argc 		=  0 ;
	char *		result ;
	int 		result_length ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z+", & z_string, & argv, & argc )  ==  FAILURE )
		return;

	// First arg must be a string
	if  ( Z_TYPE_P ( z_string )  !=  IS_STRING )
		convert_to_string ( z_string ) ;
	
	// Perform the append operation
	if  ( internal_string_append ( z_string, argc, * argv, & result, & result_length ) )
	   {
		RETVAL_TRUE ;
		ZVAL_STRINGL ( z_string, result, result_length, 0 ) ;
	    }
	else
		RETVAL_FALSE ;
		
	efree ( argv ) ;
    }

    
/*===========================================================================================================

    NAME
	string_atoi - C atoi() equivalent.
	
    PROTOTYPE
	$value 	=  string_atoi ( $str [, &$end = null ) ;
	
    DESCRIPTION
	Provides an equivalent of the C atoi() function, interpreting the following codes :
	- Decimal numbers 	: [1-9][0-9]*
	- Octal numbers 	: 0[1-7]* or 0?[oO][0-7]+
	- Hexadecimals numbers  : 0?[xX][0-9a-f]+
	- Binary numbers 	: 0?[bB][01]+
	
    PARAMETERS
	$str (string) -
		String where to append values.
		
	&$end (integer) -
		If specified, will receive the position in $str after the scanned value.
	
    RETURN VALUE
	The decoded integer value, or false if no integer value was present in the input string.
    
    NOTES
	Leading spaces are ignored.
	
 *===========================================================================================================*/
PHP_FUNCTION ( string_atoi ) 
   {
	char *			value ;
	int  			value_length ;
	zval * 			output_index 		=  NULL ;
	char * 			output_position ;
	int64			decoded_value ;
	zend_bool		status ;
	
	
	/* Get parameters */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "s|z", & value, & value_length, & output_index )  ==  FAILURE )
		return ;
	
	/* Decode */
	status 	=  thrak_atoi ( value, & output_position, & decoded_value, 0 ) ;
	
	/* Return false if no integer is represented in the value */
	if  ( ! status )
		RETURN_FALSE ;
		
	/* If the $end parameter is supplied, set it to the numeric index of the first position after the decoded value */
	if  ( output_index  !=  NULL )
	   {
		Z_TYPE_P ( output_index )	=  IS_LONG ;
		Z_LVAL_P ( output_index ) 	=  ( int ) ( output_position - value ) ;
	    }
	    
	/* All done, return the decode value (take into account the size of long on Windows) */
# if  	( LONG_SIZE  ==  4 )
	RETVAL_DOUBLE ( ( double ) decoded_value ) ;
# else
	RETVAL_LONG ( decoded_value ) ;
# endif
    }
    
    
/*===========================================================================================================

    NAME
	string_endswith - Checks if a string ends with another string.
	
    PROTOTYPE
	$bool 	=  string_endswith ( $value, $suffix, $case_insensitive = false ) ;
	
    DESCRIPTION
	Checks if the specified string ends with the specified suffix.
	The case_insensitive parameter determines whether comparison should be case-sensitive or not.
	
    PARAMETERS
	$value (string) -
		String to be checked.
		
	$suffix (string) -
		String to be compared with the end of $value.
		
	$case_insensitive (boolean) -
		When true, comparison is case-insensitive.
		
 *===========================================================================================================*/
THRAK_API zend_bool 	internal_string_endswith  ( const char *  value, int  value_length, const char *  suffix, int  suffix_length, int  case_insensitive )
   {
	zend_bool 	status 		=  0 ;
	int 		( *cmpfunc ) ( const char *  a, const char *  b, size_t  count )	=  ( case_insensitive ) ? strnicmp : strncmp ;
	
	
	if  ( suffix_length  <=  value_length )
	   {
		value	+=  ( value_length - suffix_length ) ;
		status 	 =  ( * cmpfunc ) ( value, suffix, suffix_length ) ?  0 : 1 ;
	    }
	    
	return ( status ) ;
    }

    
PHP_FUNCTION(string_endswith)
   {
	char *		value ;
	int 		value_length ;
	char *		suffix ;
	int 		suffix_length ;
	int 		case_insensitive 	=  0 ;
	
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &value, &value_length, &suffix, &suffix_length, &case_insensitive )  ==  FAILURE )
		return ;
	
	RETURN_BOOL ( internal_string_endswith ( value, value_length, suffix, suffix_length, case_insensitive ) ) ;
   }

   
   
/*===========================================================================================================

    NAME
	string_explode, string_implode - Similar to the explode()/implode functions.
	
    PROTOTYPE
	$array 	=  string_explode ( $separator, $value [ [ [ [ $left [, $right ] ], $options ], $callback ] ) ;
	$string	=  string_implode ( $separator, $array [ [ [ [ $left [, $right ] ], $options ], $callback ] ) ;
	
    DESCRIPTION
	Behaves similarly to the standard PHP function explode(), with a few enhancements :
	- Multiple separators can be given (in this case, $separator must be an array, not a string)
	- Values must be prepended or appended to the exploded string ($left / $right)
	- Several operations can be applied to each expanded value ($options).
	
	Note that the $left, $right, $options and $callback parameters can be specified in any order, the
	only restriction being that $left must always be specified before $right.
	
    PARAMETERS
	$separator (string or array of strings) -
		Separator(s) to be used for string separation.
		
	$value (string) -
		String to be exploded or array to be imploded.

	$left (string) -
		When specified, string to be prepended to each exploded/imploded value.
		
	$right (string) -
		When specified, string to be appended to each exploded/imploded value.
		
	$options (integer) -
		For string_explode(), one of the following bit options :
		- EXPLODE_TRIM_LEFT :
			Left trims each exploded string.
		- EXPLODE_TRIM_RIGHT :
			Right trims each exploded string.
		- EXPLODE_TRIM :
			Trims each exploded string left and right.
		- EXPLODE_ESCAPE_QUOTES :
			Escapes each single quote withing the exploded string.
		- EXPLODE_ESCAPE_DOUBLEQUOTES :
			Escapes each double quote withing the exploded string.
		- EXPLODE_NOCASE :
			When specified, separator(s) is(are) considered to be case-insensitive.
		- EXPLODE_IGNORE_EMPTY :
			When specified, empty strings, after processing, are ignored.
		- EXPLODE_TOLOWER :
			Converts the string to lowercase.
		- EXPLODE_TOUPPER :
			Converts the string to uppercase.
		- EXPLODE_UCFIRST :
			Uppercases the first character of the string, lowercases every other one.
		- EXPLODE_UCWORDS :
			Uppercases the first letter of each word.
		- EXPLODE_ESCAPE_QUOTES :
			Escapes quote characters.
		- EXPLODE_ESCAPE_DOUBLEQUOTES :
			Escapes doublequote characters.

		For string_implode(), constants are the same except that EXPLODE_ is replaced by IMPLODE_.
		
	$callback (callback) -
		A callback function that have the following signature :
		
			mixed  callback ( $index, $value ) ;
			
		The callback function is called before any processing guided by the $options parameter is
		performed.

		The function can return one of this values :
		- null or boolean false (or no value returned at all) :
			The input value is kept as is.
		- boolean true :
			The input value is discarded.
		- other (string or scalar type) :
			The input value is replaced by the returned one.
		
    RETURN VALUE
	For string_explode(), an array containing the exploded strings.
	Note that the function behaves a little bit differently from explode() : if the input value is empty,
	then an empty array will be returned.
	
	For string_implode(), a string containing the concatenated values.
	
	Both functions return false if something went wrong (eg, invalid parameter type or memory allocation
	error).
	
 *===========================================================================================================*/
 
/* Extra arguments that can be specified in any order are stored in a zval array of EXTRA_ARG_COUNT elements.		*/
/* Each slot in this array corresponds to a specified arg in the following order : left, right, options and callback.	*/
# define 	EXTRA_ARG_COUNT 		4
# define 	EXTRA_ARG_LEFT 			0
# define 	EXTRA_ARG_RIGHT 		1
# define 	EXTRA_ARG_OPTIONS		2
# define 	EXTRA_ARG_CALLBACK		3 

/* Generic value processing - called by __process_explode_options() and __process_implode_options() */ 
static zend_always_inline void	__process_options 	  ( int  		index,
							    char *  		value, 
							    int  		value_length, 
							    zval *  		options_zval, 
							    zval *  		left_string, 
							    zval *  		right_string, 
							    zval *  		callback,
							    char **		output_value,
							    int *		output_value_length,
							    int * 		duplicate
							    TSRMLS_DC )
   {
	char * 		value_end 	=  value + value_length ;
	char * 		original_value 	=  value ;
	int 		options 	=  ( options_zval  ==  NULL ) ?  0 : Z_LVAL_P ( options_zval ) ;
	char *		p,
	     *		q ;
	char * 		new_value ;
	int 		realloc_extra	=  0 ;
	zval 		callback_result ;			
	int 		free_value 	=  0 ;
	int 		length ;


	/* EXPLODE_TRIM_LEFT option : trim left space characters */
	if  ( options  &  EXPLODE_TRIM_LEFT )
	   {
		while  ( value  <  value_end  &&  isspace ( * value ) )
			value ++, value_length -- ;
	    }
	    
	/* EXPLODE_TRIM_RIGHT : trim right space characters */
	if  ( options  &  EXPLODE_TRIM_RIGHT )
	   {
		while  ( value_end  >  value  &&  isspace ( * ( value_end - 1 ) ) )
			value_end --, value_length -- ;
	    }

	/* EXPLODE_IGNORE_EMPTY : ignore empty strings */
	if  ( options  &  EXPLODE_IGNORE_EMPTY )
	   {
		if  ( ! value_length )
		   {
			if  ( free_value ) 
				zval_dtor ( & callback_result ) ;
				
			return ;
		    }
	    }
	    
	/* Process callback if any */
	if  ( callback  !=  NULL )
	   {
		zval 		retval ;				/* Callback return value 			        */
		zval *		callback_params [3] ;			/* Callback parameters : index and string value 	*/
		zval  		index_zval ;				/* A zval for the index parameter			*/
		zval 	  	value_zval ;				/* Another one for the value parameter 			*/
		
		/* Build the $index and $value callback parameters */
		INIT_ZVAL ( index_zval ) ;
		ZVAL_LONG ( & index_zval, index ) ;

		/***
			The zend_call_function() function in zend_execute_API.c always calls zend_vm_stack_clear_multiple() which 
			in turn calls zval_dtor() for each of the function arguments. The call is systematic, so every supplied 
			string zval will get undonditionnally freed.
			For that reason, we must allocate a copy of the supplied value in value_zval before calling the function.
		 ***/
		INIT_ZVAL ( value_zval ) ;
		ZVAL_STRINGL ( & value_zval, value, value_length, 1 ) ;
		
		/* Initialize the callback parameters array */
		callback_params [0] 	=  & index_zval ;
		callback_params [1] 	=  & value_zval ;
		callback_params [2]	=  NULL ;
		
		/* Call it */
		if  ( call_user_function ( CG ( function_table ), NULL, callback, & retval, 2, callback_params TSRMLS_CC )  ==  FAILURE )
			return ;
			
		efree ( Z_STRVAL ( value_zval ) ) ;

		/* Process return value */
		switch  ( Z_TYPE ( retval ) )
		   {
			/* Forbidden return value types */
			case 	IS_RESOURCE :
			case 	IS_OBJECT :
			case 	IS_ARRAY :
				php_error_docref ( NULL TSRMLS_CC, E_ERROR, "invalid return value type from user callback" ) ;
				return ;
				
			/* NULL return value : callback leaves the current value as is */
			case 	IS_NULL :
				callback_result 	=  retval ;		/* Keep original return value, we will need to free it */
				break ;
				
			/* Boolean false : same as NULL ; boolean true : the value is discarded */
			case 	IS_BOOL :
				if  ( Z_BVAL ( retval ) )
				   {
					zval_dtor ( & retval ) ;
					return ;
				    }
				    
				callback_result 	=  retval ;		/* Keep original return value, we will need to free it */
				break ;
				
			/* Other types : make sure to have a string here */
			default :
				callback_result 	=  retval ;		/* Keep original return value, we will need to free it */
				convert_to_string ( & retval ) ;
				value 			=  Z_STRVAL ( retval ) ;
				value_length 		=  Z_STRLEN ( retval ) ;
				value_end 		=  value + value_length ;
				free_value 		=  1 ;
		    }
	     }
	
	/* EXPLODE_TOLOWER : Converts letters to lower case */
	if  ( ( options  &  EXPLODE_TOLOWER )  ==  EXPLODE_TOLOWER )
	   {
		for  ( p = value ; p < value_end ; p ++ )
			* p 	=  tolower ( * p ) ;
	    }
	    
	/* EXPLODE_TOUPPER : Converts letters to upper case */
	if  ( ( options  &  EXPLODE_TOUPPER )  ==  EXPLODE_TOUPPER )
	   {
		for  ( p = value ; p < value_end ; p ++ )
			* p 	=  toupper ( * p ) ;
	    }
	
	/* EXPLODE_UCFIRST : Converts the first letter of the string to uppercase, lowercases the rest */
	if  ( options  &  EXPLODE_UCFIRST )
	   {
		* value 	=  toupper ( * value ) ;
		
		for  ( p = value + 1 ; p < value_end ; p ++ )
			* p 	=  tolower ( * p ) ;
	    }
	
	/* EXPLODE_UCWORDS : Converts the first letter of every word in the string to uppercase, lowercases the rest */
	if  ( options  &  EXPLODE_UCWORDS )
	   {
		char 		prev_ch 	=  * value ;
		
		* value 	=  toupper ( * value ) ;
		
		
		for  ( p = value + 1 ; p < value_end ; p ++ )
		   {
			if  ( isalpha ( prev_ch ) )
				* p 	=  tolower ( * p ) ;
			else
				* p 	=  toupper ( * p ) ;
				
			prev_ch 	=  * p ;
		    }
	    }
	    
	/* EXPLODE_ESCAPE_QUOTES/DOUBLEQUOTES : escapes quotes/double quotes during processing */
	if  ( options  &  ( EXPLODE_ESCAPE_QUOTES | EXPLODE_ESCAPE_DOUBLEQUOTES )  &&  ! ( options  &  UNESCAPE_FLAG ) )
	   {
		char 	quote 	=  ( options & EXPLODE_ESCAPE_QUOTES ) ?  '\'' : '"' ;
		
		p 	=  strchr ( value, quote ) ;
		
		while  ( p  !=  NULL )
		   {
			realloc_extra ++ ;
			
			if  ( ++ p  ==  value_end )
				break ;
				
			p 	=  strchr ( p, quote ) ;
		    }
	    }
	    
   
	/* Count potential space used for left- and right-strings */
	if  ( left_string  !=  NULL )
		realloc_extra 	+=  Z_STRLEN_P ( left_string ) ;
		
	if  ( right_string  !=  NULL )
		realloc_extra   +=  Z_STRLEN_P ( right_string ) ;
	
	/* If some space is to be added, due to left/right strings or character escaping, allocate it */
	if  ( realloc_extra )
	   {
		/* realloc_extra contains additional bytes to reserve, such as space for escaping quotes or also strings to prepend/append */
		new_value 	=  emalloc ( value_length + realloc_extra ) ;
		
		if  ( new_value  ==  NULL )
			return ;
			
		q		=  new_value ;
		
		/* Prepend "left" string, if any */
		if  ( left_string  !=  NULL )
		   {
			length		=  Z_STRLEN_P ( left_string ) ;
			
			if  ( length )
			   {
				memcpy ( q, Z_STRVAL_P ( left_string ), length ) ;
				q += length ;
			    }
		    }
		    
		/* Copy the initial value, but take into account the case where quotes are to be escaped */
		if  ( options  &  ( EXPLODE_ESCAPE_QUOTES | EXPLODE_ESCAPE_DOUBLEQUOTES )  &&  ! ( options  &  UNESCAPE_FLAG ) )
		   {
			char 	quote 	=  ( options & EXPLODE_ESCAPE_QUOTES ) ?  '\'' : '"' ;
			
			length 		=  value_length ;
			p 		=  value ;
			
			while  ( length -- )
			   {
				if  ( * p  ==  quote )
					* q ++ 	=  '\\' ;
					
				* q ++ 	=  * p ++ ;
			    }
		    }
		/* Otherwise, perform a raw copy */
		else
		   {
			memcpy ( q, value, value_length ) ;
			q += value_length ;
		    }
		    
		/* Append right string, if any */
		if  ( right_string  !=  NULL )
		   {
			length 		=  Z_STRLEN_P ( right_string ) ;    
			
			if  ( length )
				memcpy ( q, Z_STRVAL_P ( right_string ), length ) ;
		    }
			
		/* Ok, here we have a new value and a new length */
		value 		=  new_value ;
		value_length   +=  realloc_extra ;
	    }
	
	/* Free callback result if any, but only if the buffer has been reallocated */
	if  ( free_value  &&  realloc_extra )
		zval_dtor ( & callback_result ) ;
		
	/* Set the output result */
	* output_value 		=  value ;
	* output_value_length 	=  value_length ;
	* duplicate 		=  ! realloc_extra ;
    }


/* Process current value for string_explode() */
static zend_always_inline void	__process_explode_options ( zval *  		array, 
							    int 		index,
							    char *  		value, 
							    int  		value_length, 
							    zval *  		options_zval, 
							    zval *  		left_string, 
							    zval *  		right_string, 
							    zval *  		callback
							    TSRMLS_DC )
   {
	char *		output_value ;
	int 		output_value_length ;
	int 		duplicate ;
	
	
	/* Process the options to apply on the current value */
	__process_options ( index, value, value_length, options_zval, left_string, right_string, callback,
				& output_value, & output_value_length, & duplicate TSRMLS_CC ) ;
				
	/* Add the value to the returned array ; do not duplicate it if we already did it */
	add_next_index_stringl ( array, output_value, output_value_length, duplicate ) ;
    }    
    

PHP_FUNCTION ( string_explode )
   {
	zval *			separator_zval ;				/* Variables used for parameter retrieval 					*/
	char * 			value ;						
	int 			value_length ;
	zval ***		argv ;
	int 			argc 			=  0 ;
	zval *			extra_args [ EXTRA_ARG_COUNT ] ;		/* Extra args, in the following order : left, right, options and callback 	*/
	char **			separator_values ;				/* Separators used for exploding 						*/
	int *			separator_lengths ;				/* their respectives lengths 							*/
	int 			separator_count ;				/* and their count								*/
	char *			separator_buffer ;				/* All of that stored in that buffer 						*/
	char * 			callback_name ;
	char *			p,
	     *			p_end,
	     * 			q ;
	int 			i ;
	int  			index 			=  0 ;
	int 			found ;
	int 			( * cmpfunc ) 	  ( const char *  a, const char *  b, size_t  length ) ;
	char * 			( * searchfunc )  ( const char *  a, int  ch ) ;
	
	  
	     

	/* Parse parameters */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "zs*", & separator_zval, & value, & value_length, & argv, & argc )  ==  FAILURE )
		RETURN_FALSE ;
	
	if  ( argc  >  4 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_ERROR, "string_explode() expects at most 6 arguments, %d provided", argc + 2 ) ;
		
		if  ( argc )
			efree ( argv ) ;
			
		RETURN_FALSE ;
	    }
	    
	/* Check parameter type for $separator */
	switch  ( Z_TYPE_P ( separator_zval ) )
	   {
		case 	IS_OBJECT :
		case 	IS_RESOURCE :
			php_error_docref ( NULL TSRMLS_CC, E_ERROR, "the type of the $separator argument cannot be an object nor a resource" ) ;
			
			if  ( argc )
				efree ( argv ) ;
				
			RETURN_FALSE ;
	    }
	    
	/* Collect extra arguments from argv[] and reorder them into extra_args */
	memset ( extra_args, 0, sizeof ( extra_args ) ) ;

	for  ( i = 0 ; i  <  argc ; i ++ )
	   {
		switch ( Z_TYPE_P ( * argv [i] ) )
		   {
			/* Forbidden value types */
			case 	IS_RESOURCE :
				php_error_docref ( NULL TSRMLS_CC, E_ERROR, "incorrect resource data type for parameter #%d", i + 3 ) ;
				efree ( argv ) ;
					
				RETURN_FALSE ;

			case 	IS_ARRAY :
				if  ( extra_args [ EXTRA_ARG_CALLBACK ]  !=  NULL  ||  zend_hash_num_elements ( Z_ARRVAL_P ( * argv [i] ) )  !=  2 )
				   {
					php_error_docref ( NULL TSRMLS_CC, E_ERROR, "incorrect array data type for parameter #%d", i + 3 ) ;
					efree ( argv ) ;
						
					RETURN_FALSE ;
				    }
				    
				extra_args [ EXTRA_ARG_CALLBACK ] 	=  * argv [i] ;
				break ;
			
			/* If an object is specified, it must be a closure */
			case  	IS_OBJECT :
				callback_name 	=  NULL ;
				
				if  ( ! zend_is_callable ( * argv [i], 0, & callback_name TSRMLS_CC )  ||  extra_args [ EXTRA_ARG_CALLBACK ]  !=  NULL )
				   {
					php_error_docref ( NULL TSRMLS_CC, E_ERROR, "incorrect object data type for parameter #%d", i + 3 ) ;
					
					if  ( callback_name  !=  NULL )
						efree ( callback_name ) ;
						
					efree ( argv ) ;
						
					RETURN_FALSE ;
				    }

				if  ( callback_name  !=  NULL )
					efree ( callback_name ) ;
				   
				extra_args [ EXTRA_ARG_CALLBACK ] 	=  * argv [i] ;
				break ;
			
			/* A long is reserved for the $options parameter */
			case 	IS_LONG :
				if  ( extra_args [ EXTRA_ARG_OPTIONS ]  ==  NULL )
				   {
					extra_args [ EXTRA_ARG_OPTIONS ]	=  * argv [i] ;
					break ;
				    }
				/* Break intentionnally missing to fall through the next case */
				
			/* All other types are for either $left, $right or $callback (when specified as a string) */
			case 	IS_BOOL :
			case 	IS_DOUBLE :
			case 	IS_STRING :
				convert_to_string_ex ( argv [i] ) ;
				
				if  ( extra_args [ EXTRA_ARG_LEFT ]  !=  NULL )
				   {
					if ( extra_args [ EXTRA_ARG_RIGHT ]  !=  NULL )
					   {
						if  ( extra_args [ EXTRA_ARG_CALLBACK ]  !=  NULL )
						   {
							php_error_docref ( NULL TSRMLS_CC, E_ERROR, "incorrect data type for parameter #%d", i + 3 ) ;
							efree ( argv ) ;
								
							RETURN_FALSE ;
						    }
						else
							extra_args [ EXTRA_ARG_CALLBACK ] 	=  * argv [i] ;
					    }
					else
						extra_args [ EXTRA_ARG_RIGHT ] 	=  * argv [i] ;
				    }
				else
					extra_args [ EXTRA_ARG_LEFT ] 	=  * argv [i] ;
				break ;
		    }
	    }
	    
	/* Explode the input string */
	separator_buffer 	=  thrak_array_to_char ( separator_zval, & separator_values, & separator_count, & separator_lengths ) ;
	p 			=  value ;
	p_end 			=  value + value_length ;

	/* Make sure we could convert input separator array to an array of strings */
	if  ( separator_buffer  ==  NULL )
	   {
		if  ( argc )
			efree ( argv ) ;
			
		RETURN_FALSE ;
	    }
		
	/* Compare function */
	if  ( extra_args [ EXTRA_ARG_OPTIONS ]  !=  NULL  &&  ( Z_LVAL_P ( extra_args [ EXTRA_ARG_OPTIONS ] )  &  EXPLODE_NOCASE ) )
	   {
		cmpfunc 	=  strnicmp ;
		searchfunc 	=  thrak_strichr ;
	    }
	else
	   {
		cmpfunc 	=  strncmp ;
		searchfunc 	=  strchr ;
	    }
	
	/* Initialize return value */
	array_init ( return_value ) ;
	index 	=  0 ;

	/* Loop through string to explode */
	while  ( p  <  p_end )
	   {
		/* Search for the next occurrence of a separator */
		found 	=  0 ;
		
		for  ( i = 0 ; i  <  separator_count ; i ++ )
		   {
			q 	=  searchfunc ( p, separator_values [i] [0] ) ;
			
			if  ( q  !=  NULL )
			   {
				if  ( ! cmpfunc ( q, separator_values [i], separator_lengths [i] ) )
				   {
					found 	=  1 ;
					break ;
				    }
			    }
		    }

		/* Separator found */
		if  ( found )
		   {
			__process_explode_options ( return_value, index ++, p, ( int ) ( q - p ), 
					extra_args [ EXTRA_ARG_OPTIONS ], extra_args [ EXTRA_ARG_LEFT ], extra_args [ EXTRA_ARG_RIGHT ], 
					extra_args [ EXTRA_ARG_CALLBACK ] TSRMLS_CC ) ;
			p 	= q + separator_lengths [i] ;
		    }
		/* This string contains none of the supplied separator_values : no more search to be done */
		else
			break ;
	    }

	/* Maybe a last item ? */
	if  ( p  <  p_end )
	   {
		__process_explode_options ( return_value, index, p, ( int ) ( p_end - p ), 
					extra_args [ EXTRA_ARG_OPTIONS ], extra_args [ EXTRA_ARG_LEFT ], extra_args [ EXTRA_ARG_RIGHT ], 
					extra_args [ EXTRA_ARG_CALLBACK ] TSRMLS_CC ) ;
	    }
	    
	/* Free allocated resources */
	thrak_free_array_to_char ( separator_buffer ) ;
	
	if  ( argc )
		efree ( argv ) ;
    }

    
/* Process current value for string_implode() */
static zend_always_inline void	__process_implode_options ( int 		index,
							    char *  		value, 
							    int  		value_length, 
							    zval *  		options_zval, 
							    zval *  		left_string, 
							    zval *  		right_string, 
							    zval *  		callback,
							    char **		output_value,
							    int *		output_value_length,
							    int * 		duplicate 
							    TSRMLS_DC )
   {
	/* Process the options to apply on the current value */
	__process_options ( index, value, value_length, options_zval, left_string, right_string, callback,
				output_value, output_value_length, duplicate TSRMLS_CC ) ;
    }    
    

/* Holds information about a processed item */
typedef struct  processed_item
   {
	char *		value ;			/* Value 									*/
	int 		value_length ;		/* Value length 								*/
	int 		needs_duplicate ;	/* = 1 when value points to an original buffer and has not been strdup'ed 	*/
    }  processed_item ;
    
PHP_FUNCTION ( string_implode )
   {
	char * 			separator_value ;				/* Parameter values 								*/
	int 			separator_length ;
	zval * 			array ;
	zval ***		argv ;
	int 			argc 			=  0 ;
	zval *			extra_args [ EXTRA_ARG_COUNT ] ;		/* Extra args, in the following order : left, right, options and callback 	*/
	char * 			callback_name ;
	char * 			return_buffer 		=  NULL ;		/* Return value ; reallocated upon temp buffer overflows 			*/
	char * 			p ;
	processed_item *	processed_items ;				/* Processed input items							*/
	int  			processed_item_count ;
	int  			total_size 		=  0 ;			/* Total processed items size 							*/
	int 			i ;
	int  			index 			=  0 ;
	HashTable *		array_hash ;					/* Used for looping through the array of strings to be imploded 		*/
	zval ** 		item ;

	     

	/* Parse parameters */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "sa*", & separator_value, & separator_length, & array, & argv, & argc )  ==  FAILURE )
		RETURN_FALSE ;
	
	if  ( argc  >  4 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_ERROR, "string_implode() expects at most 6 arguments, %d provided", argc + 2 ) ;
		efree ( argv ) ;
			
		RETURN_FALSE ;
	    }
	    
	/* Collect extra arguments from argv[] and reorder them into extra_args */
	memset ( extra_args, 0, sizeof ( extra_args ) ) ;

	for  ( i = 0 ; i  <  argc ; i ++ )
	   {
		switch ( Z_TYPE_P ( * argv [i] ) )
		   {
			/* Forbidden value types */
			case 	IS_RESOURCE :
				php_error_docref ( NULL TSRMLS_CC, E_ERROR, "incorrect resource data type for parameter #%d", i + 3 ) ;
				efree ( argv ) ;
					
				RETURN_FALSE ;

			case 	IS_ARRAY :
				if  ( extra_args [ EXTRA_ARG_CALLBACK ]  !=  NULL  ||  zend_hash_num_elements ( Z_ARRVAL_P ( * argv [i] ) )  !=  2 )
				   {
					php_error_docref ( NULL TSRMLS_CC, E_ERROR, "incorrect array data type for parameter #%d", i + 3 ) ;
					efree ( argv ) ;
						
					RETURN_FALSE ;
				    }
				    
				extra_args [ EXTRA_ARG_CALLBACK ] 	=  * argv [i] ;
				break ;
			
			/* If an object is specified, it must be a closure */
			case  	IS_OBJECT :
				callback_name 	=  NULL ;
				
				if  ( ! zend_is_callable ( * argv [i], 0, & callback_name TSRMLS_CC )  ||  extra_args [ EXTRA_ARG_CALLBACK ]  !=  NULL )
				   {
					php_error_docref ( NULL TSRMLS_CC, E_ERROR, "incorrect object data type for parameter #%d", i + 3 ) ;
					
					if  ( callback_name  !=  NULL )
						efree ( callback_name ) ;
						
					efree ( argv ) ;
						
					RETURN_FALSE ;
				    }

				if  ( callback_name  !=  NULL )
					efree ( callback_name ) ;
				   
				extra_args [ EXTRA_ARG_CALLBACK ] 	=  * argv [i] ;
				break ;
			
			/* A long is reserved for the $options parameter */
			case 	IS_LONG :
				if  ( extra_args [ EXTRA_ARG_OPTIONS ]  ==  NULL )
				   {
					extra_args [ EXTRA_ARG_OPTIONS ]	=  * argv [i] ;
					break ;
				    }
				/* Break intentionnally missing to fall through the next case */
				
			/* All other types are for either $left, $right or $callback (when specified as a string) */
			case 	IS_BOOL :
			case 	IS_DOUBLE :
			case 	IS_STRING :
				convert_to_string_ex ( argv [i] ) ;
				
				if  ( extra_args [ EXTRA_ARG_LEFT ]  !=  NULL )
				   {
					if ( extra_args [ EXTRA_ARG_RIGHT ]  !=  NULL )
					   {
						if  ( extra_args [ EXTRA_ARG_CALLBACK ]  !=  NULL )
						   {
							php_error_docref ( NULL TSRMLS_CC, E_ERROR, "incorrect data type for parameter #%d", i + 3 ) ;
							efree ( argv ) ;
								
							RETURN_FALSE ;
						    }
						else
							extra_args [ EXTRA_ARG_CALLBACK ] 	=  * argv [i] ;
					    }
					else
						extra_args [ EXTRA_ARG_RIGHT ] 	=  * argv [i] ;
				    }
				else
					extra_args [ EXTRA_ARG_LEFT ] 	=  * argv [i] ;
				break ;
		    }
	    }

	/* Allocate an array for the input array items after they have been processed */
	array_hash 		=  Z_ARRVAL_P ( array ) ;
	processed_item_count	=  zend_hash_num_elements ( array_hash ) ;
	
	if  ( ! processed_item_count )
	   {
		if  ( argc )
			efree ( argv ) ;
			
		RETURN_BOOL ( 0 ) ;
	    }
	    
	processed_items 	=  emalloc ( sizeof ( processed_item ) * processed_item_count ) ;
	
	if  ( processed_items  ==  NULL )
	   {
		if  ( argc )
			efree ( argv ) ;
			
		RETURN_BOOL ( 0 ) ;
	    }
	
	/* Loop through input array to be imploded */
	for  ( zend_hash_internal_pointer_reset ( array_hash ) ; 
			zend_hash_get_current_data ( array_hash, ( void ** ) & item )  ==  SUCCESS ; 
			zend_hash_move_forward ( array_hash ) ) 
	   {
		/* Systematically convert value to string */
		convert_to_string_ex ( item ) ;
		
		/* Process implode options for this value */
		__process_implode_options ( index, Z_STRVAL_PP ( item ), Z_STRLEN_PP ( item ),
					extra_args [ EXTRA_ARG_OPTIONS ], extra_args [ EXTRA_ARG_LEFT ], extra_args [ EXTRA_ARG_RIGHT ], 
					extra_args [ EXTRA_ARG_CALLBACK ],
					& processed_items [ index ]. value, & processed_items [ index ]. value_length, & processed_items [ index ]. needs_duplicate
					TSRMLS_CC ) ;

		total_size 	+=  processed_items [ index ]. value_length ;
		index ++ ;
	    }
	    
	/* Allocate buffer for the return value (concatenation of all the processed items) */
	return_buffer 	=  emalloc ( total_size ) ;
	
	if  ( return_buffer  ==  NULL )
	   {
		RETVAL_FALSE ;
		goto  FreeResources ;
	    }
	    
	/* Catenate the processed items */
	p	=  return_buffer ;
	
	for  ( i = 0 ; i  <  processed_item_count ; i ++ )
	   {
		memcpy ( p, processed_items [i]. value, processed_items [i]. value_length ) ;
		p += processed_items [i]. value_length ;
	    }
	    
	RETVAL_STRINGL ( return_buffer, total_size, 0 ) ;
	    
	/* Free allocated resources */
FreeResources :
	for  ( i = 0 ;  i  <  processed_item_count ; i ++ )
	   {
		if  ( ! processed_items [i]. needs_duplicate )
			efree ( processed_items [i]. value ) ;
	    }
	    
	efree ( processed_items ) ;
	
	if  ( argc )
		efree ( argv ) ;
    }
    
 
/*===========================================================================================================

    NAME
	string_startswith - Checks if a string starts with another string.
	
    PROTOTYPE
	$bool 	=  string_startswith ( $value, $prefix, $case_insensitive = false ) ;
	
    DESCRIPTION
	Checks if the specified string starts with the specified prefix.
	The case_insensitive parameter determines whether comparison should be case-sensitive or not.
	
    PARAMETERS
	$value (string) -
		String to be checked.
		
	$prefix (string) -
		String to be compared with the start of $value.
		
	$case_insensitive (boolean) -
		When true, comparison is case-insensitive.
		
 *===========================================================================================================*/
THRAK_API zend_bool 	internal_string_startswith  ( const char *  value, int  value_length, const char *  prefix, int  prefix_length, int  case_insensitive )
   {
	zend_bool 	status 		=  0 ;
	int 		( *cmpfunc ) ( const char *  a, const char *  b, size_t  count )	=  ( case_insensitive ) ? strnicmp : strncmp ;
	
	
	if  ( prefix_length  <=  value_length )
		status 	 =  ( * cmpfunc ) ( value, prefix, prefix_length ) ?  0 : 1 ;
	    
	return ( status ) ;
    }

    
PHP_FUNCTION(string_startswith)
   {
	char *		value ;
	int 		value_length ;
	char *		suffix ;
	int 		suffix_length ;
	int 		case_insensitive 	=  0 ;
	
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &value, &value_length, &suffix, &suffix_length, &case_insensitive )  ==  FAILURE )
		return ;
	
	RETURN_BOOL ( internal_string_startswith ( value, value_length, suffix, suffix_length, case_insensitive ) ) ;
   }

    

/*===========================================================================================================

    NAME
	string_setbos - Prepends a prefix to a string if it does not exist.
	
    PROTOTYPE
	$bool 	=  string_setbos ( &$value, $prefix, [ $case_insensitive = false, [ $return_value = false ] ] ) ;
	
    DESCRIPTION
	Prepends the $prefix string to $value if not already present.
	
    PARAMETERS
	$value (string) -
		String to be checked.
		
	$prefix (string) -
		String to be prepended to $value if $value does not start with that string.
		
	$case_insensitive (boolean) -
		When true, comparison is case-insensitive.
		
	$return_value (boolean) -
		When true, the new value is returned and $value is left as is.
		When false, $value is modified in-place.
		
    RETURN VALUE
	When the $return_value parameter is false, returns true if operation succeeded or false otherwise.
	When $return_value is true, returns either the new string or false if operation failed or no update
	was performed.
		
 *===========================================================================================================*/
THRAK_API zend_bool 	internal_string_setbos  (  char *  		value, 
						  int  			value_length, 
						  char *  		prefix, 
						  int  			prefix_length, 
						  zend_bool  		case_insensitive, 
						  char **  		result, 
						  int *  		result_length, 
						  zend_bool *  		modified )
   {
	int 		( * cmpfunc ) ( const char *  a, const char *  b, size_t  count ) ;
	char * 		p ;
	
	
	/* If input string or prefix is empty, or prefix length is greater than input string length, then we have nothing to do */
	if  ( ! value_length  ||  ! prefix_length )
	   {
		* result 		=  value ;
		* result_length 	=  value_length ;
		* modified 		=  0 ;
		
		return ( 1 ) ;
	    }
	    
	/* Compare function and comparison start in input string */
	cmpfunc 	=  ( case_insensitive ) ?  strnicmp : strncmp ;
	
	/* If input string ends with the supplied prefix, then we have nothing to do */
	if  ( prefix_length  <=  value_length  &&  ! cmpfunc ( value, prefix, prefix_length ) )
	   {
		* result 		=  value ;
		* result_length 	=  value_length ;
		* modified 		=  0 ;
		
		return ( 1 ) ;
	    }
	
	/* Input string does not end with the specified prefix : allocate enough room to append it */
	p		=  emalloc ( value_length + prefix_length ) ;
	
	if  ( p  ==  NULL )
		return ( 0 ) ;
		
	strncpy ( p, prefix, prefix_length ) ;
	strncpy ( p + prefix_length, value, value_length ) ;
	
	/* Tell the caller about the new string and the fact that we modified the initial string and an malloc() took place */
	* result 		=  p ;
	* result_length 	=  value_length + prefix_length ;
	* modified 		=  1 ;
	
	/* All done, report everything is ok */
	return ( 1 ) ;
    }

    
PHP_FUNCTION(string_setbos)
   {
	zval * 		z_value ;
	char *		value ;
	int 		value_length ;
	char *		prefix ;
	int 		prefix_length ;
	char *		new_value ;
	int 		new_value_length ;
	zend_bool	case_insensitive 	=  0 ;
	zend_bool 	retval	 		=  0 ;
	zend_bool 	modified ;
	
	
	/* Get parameters */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "zs|bb", & z_value, & prefix, & prefix_length, & case_insensitive, & retval )  ==  FAILURE )
		return ;

	// First arg must be a string
	if  ( Z_TYPE_P ( z_value )  !=  IS_STRING )
		convert_to_string ( z_value ) ;
		
	value 		=  Z_STRVAL_P ( z_value ) ;
	value_length	=  Z_STRLEN_P ( z_value ) ;
		
	// Check input string and prefix lengths */
	if  ( ! value_length )
	   {
		/* If $return_value = true, return a copy of the input string */
		if  ( retval )
		   {
			RETURN_STRINGL ( value, value_length, 1 ) ;
		    }
		/* Otherwise, leave the input string as is */
		else
			RETURN_FALSE ;
	    }
	    
	/* The same case happens when the prefix string is empty */
	if  ( ! prefix_length )
	   {
		if  ( retval )
		   {
			RETURN_STRINGL ( value, value_length, 1 ) ;
		    }
		else
			RETURN_FALSE ;
	    }
	    
	    
	/* Perform the prefix appending */
	if  ( internal_string_setbos ( value, value_length, prefix, prefix_length, case_insensitive, & new_value, & new_value_length, & modified ) )
	   {
		/* If we need to return a value instead of modifying the supplied input value in-place... */
		if (  retval )
		   {
			/* ... if the string was modified, then internal_string_seteos() returned a newly malloc'ed strings, so no need to duplicate it */
			if  ( modified )
			   {
				RETURN_STRINGL ( new_value, new_value_length, 0 ) ;
			    }
			/* Otherwise, the original value was returned so we do need to duplicate it */
			else
			   {
				RETURN_STRINGL ( new_value, new_value_length, 1 ) ;
			    }
		    }
		/* Otherwise, we need to perform an in-place modification of the supplied input value */
		else
		   {
			/* ... but only if the string was modified (ie, the prefix was appended to the original value) */
			if (  modified )
			   {
				zval_dtor ( z_value ) ;
				ZVAL_STRINGL ( z_value, new_value, new_value_length, 0 ) ;
			    }
			    
			RETURN_TRUE ;
		    }
	    }
	/* Something bad happened in internal_string_seteos(), such as memory allocation error */
	else
		RETURN_FALSE ;
   }

   
   
/*===========================================================================================================

    NAME
	string_seteos - Appends a suffix to a string if it does not exist.
	
    PROTOTYPE
	$bool 	=  string_seteos ( &$value, $suffix, [ $case_insensitive = false, [ $return_value = false ] ] ) ;
	
    DESCRIPTION
	Adds the $suffix string to $value if not already present.
	
    PARAMETERS
	$value (string) -
		String to be checked.
		
	$suffix (string) -
		String to be appended to $value if $value does not end with that string.
		
	$case_insensitive (boolean) -
		When true, comparison is case-insensitive.
		
	$return_value (boolean) -
		When true, the new value is returned and $value is left as is.
		When false, $value is modified in-place.
		
    RETURN VALUE
	When the $return_value parameter is false, returns true if operation succeeded or false otherwise.
	When $return_value is true, returns either the new string or false if operation failed or no update
	was performed.
		
 *===========================================================================================================*/
THRAK_API zend_bool 	internal_string_seteos  (  char *  		value, 
						  int  			value_length, 
						  char *  		suffix, 
						  int  			suffix_length, 
						  zend_bool  		case_insensitive, 
						  char **  		result, 
						  int *  		result_length, 
						  zend_bool *  		modified )
   {
	int 		( * cmpfunc ) ( const char *  a, const char *  b, size_t  count ) ;
	char * 		p ;
	
	
	/* If input string or suffix is empty */
	if  ( ! value_length  ||  ! suffix_length  )
	   {
		* result 		=  value ;
		* result_length 	=  value_length ;
		* modified 		=  0 ;
		
		return ( 1 ) ;
	    }
	    
	/* Compare function and comparison start in input string */
	cmpfunc 	=  ( case_insensitive ) ?  strnicmp : strncmp ;
	p 		=  value + max ( value_length - suffix_length, 0 ) ;
	
	/* If input string ends with the supplied suffix, then we have nothing to do */
	if  ( suffix_length  <=  value_length  &&  ! cmpfunc ( p, suffix, suffix_length ) )
	   {
		* result 		=  value ;
		* result_length 	=  value_length ;
		* modified 		=  0 ;
		
		return ( 1 ) ;
	    }
	
	/* Input string does not end with the specified suffix : allocate enough room to append it */
	p		=  emalloc ( value_length + suffix_length ) ;
	
	if  ( p  ==  NULL )
		return ( 0 ) ;
		
	strncpy ( p, value, value_length ) ;
	strncpy ( p + value_length, suffix, suffix_length ) ;
	
	/* Tell the caller about the new string and the fact that we modified the initial string and an malloc() took place */
	* result 		=  p ;
	* result_length 	=  value_length + suffix_length ;
	* modified 		=  1 ;
	
	/* All done, report everything is ok */
	return ( 1 ) ;
    }

    
PHP_FUNCTION(string_seteos)
   {
	zval * 		z_value ;
	char *		value ;
	int 		value_length ;
	char *		suffix ;
	int 		suffix_length ;
	char *		new_value ;
	int 		new_value_length ;
	zend_bool	case_insensitive 	=  0 ;
	zend_bool 	retval	 		=  0 ;
	zend_bool 	modified ;
	
	
	/* Get parameters */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "zs|bb", & z_value, & suffix, & suffix_length, & case_insensitive, & retval )  ==  FAILURE )
		return ;

	// First arg must be a string
	if  ( Z_TYPE_P ( z_value )  !=  IS_STRING )
		convert_to_string ( z_value ) ;
		
	value 		=  Z_STRVAL_P ( z_value ) ;
	value_length	=  Z_STRLEN_P ( z_value ) ;
		
	// Check input string and suffix lengths */
	if  ( ! value_length )
	   {
		/* If $return_value = true, return a copy of the input string */
		if  ( retval )
		   {
			RETURN_STRINGL ( value, value_length, 1 ) ;
		    }
		/* Otherwise, leave the input string as is */
		else
			RETURN_FALSE ;
	    }
	    
	/* the same case happens when the suffix string is empty */
	if  ( ! suffix_length )
	   {
		if  ( retval )
		   {
			RETURN_STRINGL ( value, value_length, 1 ) ;
		    }
		else
			RETURN_FALSE ;
	    }
	    
	/* Perform the suffix appending */
	if  ( internal_string_seteos ( value, value_length, suffix, suffix_length, case_insensitive, & new_value, & new_value_length, & modified ) )
	   {
		/* If we need to return a value instead of modifying the supplied input value in-place... */
		if (  retval )
		   {
			/* ... if the string was modified, then internal_string_seteos() returned a newly malloc'ed strings, so no need to duplicate it */
			if  ( modified )
			   {
				RETURN_STRINGL ( new_value, new_value_length, 0 ) ;
			    }
			/* Otherwise, the original value was returned so we do need to duplicate it */
			else
			   {
				RETURN_STRINGL ( new_value, new_value_length, 1 ) ;
			    }
		    }
		/* Otherwise, we need to perform an in-place modification of the supplied input value */
		else
		   {
			/* ... but only if the string was modified (ie, the suffix was appended to the original value) */
			if (  modified )
			   {
				zval_dtor ( z_value ) ;
				ZVAL_STRINGL ( z_value, new_value, new_value_length, 0 ) ;
			    }
			    
			RETURN_TRUE ;
		    }
	    }
	/* Something bad happened in internal_string_seteos(), such as memory allocation error */
	else
		RETURN_FALSE ;
   }

    
   