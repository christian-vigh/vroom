 /************************************************************************************************************

    NAME
	zval.c
	
    DESCRIPTION
	Functions dealing with zvals.
	
    AUTHOR
	Christian Vigh, 10/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# include 	"php_vroom.h"
# include	"zval.h"


/*===========================================================================================================

    NAME
	zval_initialize - Initializes the zval functions.
		
 *===========================================================================================================*/
void 	zval_initialize ( THRAK_INIT_FUNC_ARGS )
   {
	/* Register constants for ZVAL types */
	REGISTER_LONG_CONSTANT ( "ZVAL_UNKNOWN"		, -1			, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "ZVAL_NULL"		, IS_NULL		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "ZVAL_BOOL"		, IS_BOOL		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "ZVAL_LONG"		, IS_LONG		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "ZVAL_DOUBLE"		, IS_DOUBLE		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "ZVAL_STRING"		, IS_STRING		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "ZVAL_RESOURCE"	, IS_RESOURCE		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "ZVAL_ARRAY"		, IS_ARRAY		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "ZVAL_OBJECT"		, IS_OBJECT		, CONST_CS | CONST_PERSISTENT ) ;
    }


/*===========================================================================================================

    NAME
	zval_shutdown
	
    DESCRIPTION
	Sub-extension shutdown function.
	
 *===========================================================================================================*/
void 	zval_shutdown ( THRAK_SHUTDOWN_FUNC_ARGS )
   {
    }

    
/*===========================================================================================================

    NAME
	zval_refcount - gets a zval reference count
	
    PROTOTYPE 
	int  	zval_refcount ( &$variable ) ;
	
    DESCRIPTION
	Returns the number of references to the specified zval.
	
    PARAMETERS
	$variable (mixed) -
		Variable whose reference count is to be returned.

    RETURN VALUE
	The reference count of the underlying zval.
	
 *===========================================================================================================*/
PHP_FUNCTION ( zval_refcount )
   {
	zval *		value ;
	
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z", &value )  ==  FAILURE )
		return ;
		
	RETURN_LONG ( Z_REFCOUNT_P ( value ) - 1 ) ;
    }


/*===========================================================================================================

    NAME
	zval_dump - get zval contents as an array 
	
    PROTOTYPE 
	$array 	= zval_dump ( &$variable ) ;
	
    DESCRIPTION
	Returns an associative array containing the fields of a zval structure.
	
    PARAMETERS
	$variable (mixed) -
		Variable to be dumped.

    RETURN VALUE
	An associative array containing the following keys :
	- 'type' :
		zval value type.
	- 'type-string' :
		zval value type, as a string.
	- 'refcount' :
		Reference count. See notes below.
	- 'isref' :
		A boolean indicating whether this zval is a reference or not. See notes below.
	- 'value' :
		The underlying zval value.
	- 'value-length' :
		For string values, contain the string length.
	
    NOTES
	Since this function accepts a reference to a zval, its reference count field will be automatically
	incremented and the zval will be set to be a reference.
	We try to simulate how the original value was before calling this function :
	- Reference count is decremented by one
	- The "isref" field of the zval is set to zero if the decremented reference count is one.
	This may not reflect the actual situation but it tends to be more accurate than debug_zval_dump().
	
 *===========================================================================================================*/
PHP_FUNCTION ( zval_dump )
   {
	zval *		value ;				/* Input value 								*/
	zval *		value_dump_array ;		/* Returned array 							*/
	zval *		dumped_value ;			/* Holds a copy of the value - returned in the 'value' array key 	*/
	int 		type,				/* Value type constant 							*/
			refcount, 			/* Reference count 							*/
			isref ;				/* 1 if value is a reference 						*/
	char *		type_string ;			/* Value type, as a string 						*/
	
	
	/* Parse parameters */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z", &value )  ==  FAILURE )
		return ;
	
	/* Initialize the returned array */
	MAKE_STD_ZVAL ( value_dump_array ) ;
	array_init ( value_dump_array ) ;

	// Get type value and type string
	switch  ( Z_TYPE_P ( value ) )
	   {
		case	IS_NULL 	:  type = IS_NULL     ; type_string 	= "ZVAL_NULL"     ; break ;
		case 	IS_BOOL 	:  type = IS_BOOL     ; type_string 	= "ZVAL_BOOL"     ; break ;
		case 	IS_LONG 	:  type = IS_LONG     ; type_string 	= "ZVAL_LONG"     ; break ;
		case 	IS_DOUBLE 	:  type = IS_DOUBLE   ; type_string 	= "ZVAL_DOUBLE"   ; break ;
		case 	IS_STRING 	:  type = IS_STRING   ; type_string 	= "ZVAL_STRING"   ; break ;
		case 	IS_RESOURCE 	:  type = IS_RESOURCE ; type_string 	= "ZVAL_RESOURCE" ; break ;
		case 	IS_ARRAY 	:  type = IS_ARRAY    ; type_string 	= "ZVAL_ARRAY"    ; break ;
		case 	IS_OBJECT	:  type = IS_OBJECT   ; type_string 	= "ZVAL_OBJECT"   ; break ;
		default 		:  type = -1          ; type_string 	= "ZVAL_UNKNOWN"  ; break ;
	    }
	    
# ifdef 	PHP_WIN32
# 	pragma 		warning ( disable : 4267 )
# endif
	refcount 	=  Z_REFCOUNT_P ( value ) - 1 ;
	isref 		=  ( refcount  >  1 ) ;
	
	/* Set standard fields */
	add_assoc_string ( value_dump_array, "type-string"	, type_string, 1 ) ;
	add_assoc_long   ( value_dump_array, "type"		, type ) ;
	add_assoc_long   ( value_dump_array, "refcount" 	, refcount ) ;
	add_assoc_bool   ( value_dump_array, "isref" 		, isref ) ;

	/* Make a copy of the underlying value */
	MAKE_STD_ZVAL ( dumped_value ) ;
	INIT_PZVAL_COPY ( dumped_value, value ) ;
	add_assoc_zval ( value_dump_array, "value", dumped_value ) ;
	
	/* Add value length if type is string */
	if  ( type  ==  IS_STRING )
		add_assoc_long ( value_dump_array, "value-length", Z_STRLEN_P ( value ) ) ;
	
# ifdef 	PHP_WIN32
# 	pragma 		warning ( default : 4267 )
# endif
	
	RETURN_ZVAL ( value_dump_array, 0, 0 ) ;
    }   

    
/*===========================================================================================================

    NAME
	zval_decref, zval_incref, zval_setref - Manipulate a zval reference count.
	
    PROTOTYPE 
	int  	zval_decref ( &$variable ) ;
	int  	zval_incref ( &$variable ) ;
	int  	zval_setref ( &$variable, $refcount ) ;
	
    DESCRIPTION
	Manipulates a zval reference count :
	- zval_decref decrements the reference count
	- zval_incref increments it
	- and zval_setref assigns a new reference count
	
    PARAMETERS
	$variable (mixed) -
		Variable whose reference count is to be returned.
		
	$refcount (int) -
		New reference count.

    RETURN VALUE
	All functions return the previous reference count.
	
 *===========================================================================================================*/
PHP_FUNCTION ( zval_decref )
   {
	zval *		value ;
	int 		old_refcount ;
	
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z", &value )  ==  FAILURE )
		return ;
	
	// old_count is current_count - 1 because calling this function with a reference to the original variable increased by 1 its refcount
	old_refcount 	=  Z_REFCOUNT_P ( value ) - 1 ;		
	
	if  ( old_refcount  >  1 )
		Z_DELREF_P ( value ) ;
	
	RETURN_LONG ( old_refcount ) ;
    }    
 
 
PHP_FUNCTION ( zval_incref )
   {
	zval *		value ;
	int 		old_refcount ;
	
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z", &value )  ==  FAILURE )
		return ;
		
	// old_count is current_count - 1 because calling this function with a reference to the original variable increased by 1 its refcount
	old_refcount 	=  Z_REFCOUNT_P ( value ) - 1 ;
	Z_ADDREF_P ( value ) ;
	
	RETURN_LONG ( old_refcount ) ;
    }    

    
 PHP_FUNCTION ( zval_setref )
   {
	zval *		value ;
	int 		new_refcount ;
	int 		old_refcount ;
	
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "zl", &value, &new_refcount )  ==  FAILURE )
		return ;
		
	// old_count is current_count - 1 because calling this function with a reference to the original variable increased by 1 its refcount
	old_refcount 	=  Z_REFCOUNT_P ( value ) - 1 ;
	
	// new_refcount + 1 because the reference count of the supplied variable will be decreased upon function exit
	Z_SET_REFCOUNT_P ( value, new_refcount + 1 ) ;
	
	RETURN_LONG ( old_refcount ) ;
    }    
 

/*===========================================================================================================

    NAME
	zval_isref - checks if zval is a reference
	
    PROTOTYPE 
	bool  	zval_isref ( $variable ) ;
	
    DESCRIPTION
	Returns a boolean indicating whether the specified variable is a reference or not.
	
    PARAMETERS
	$variable (mixed) -
		Variable whose type is to be retrieved.

     RETURN VALUE
	True if the specified variable is a reference, false otherwise.
	
 *===========================================================================================================*/
PHP_FUNCTION ( zval_isref )
   {
	zval *		value ;
	
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z", &value )  ==  FAILURE )
		return ;

	// Since the argument of this function is a reference, the variable's reference count will be incremented by 1
	// Thus, a variable which is not a reference will have its reference count set to 2 ; we need to adjust by -1 
	// to take into account the extra reference implied by calling this function.
	RETURN_BOOL ( Z_REFCOUNT_P ( value )  >  2 ) ;
    }     

    
/*===========================================================================================================

    NAME
	zval_swap - Swaps two zvals.
	
    PROTOTYPE 
	zval_swap ( $value1, $value2 ) ;
	
    DESCRIPTION
	Swaps the contents of two zvals.
	
    PARAMETERS
	$value1, $value2 (mixed) -
		Variables to be swapped.
	
 *===========================================================================================================*/
PHP_FUNCTION ( zval_swap )
   {
	zval *		value1,  
	     *		value2 ;
	zval 		temp ;
	
	// Parse parameters
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "zz", &value1, &value2 )  ==  FAILURE )
		return ;

	// Swap values
	memcpy ( & temp, value1, sizeof ( zval ) ) ;
	memcpy ( value1, value2, sizeof ( zval ) ) ;
	memcpy ( value2, & temp, sizeof ( zval ) ) ;
    }     
    
    
/*===========================================================================================================

    NAME
	zval_type - gets a zval type
	
    PROTOTYPE 
	int  	zval_type ( $variable ) ;
	
    DESCRIPTION
	Returns the type of a zval.
	
    PARAMETERS
	$variable (mixed) -
		Variable whose type is to be retrieved.

     RETURN VALUE
	One of the constants ZVAL_NULL, ZVAL_BOOL, ZVAL_LONG, ZVAL_STRING, ZVAL_RESOURCE, ZVAL_ARRAY, 
	ZVAL_OBJECT, or ZVAL_UNKNOWN if the zval type could not be determined.
	
 *===========================================================================================================*/
PHP_FUNCTION ( zval_type )
   {
	zval *		value ;
	
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z", &value )  ==  FAILURE )
		return ;
		
	RETURN_LONG ( Z_TYPE_P ( value ) ) ;
    }    
 
 
/*===========================================================================================================

    NAME
	zval_type_constant - gets a zval type as a string
	
    PROTOTYPE 
	int  	zval_type ( $variable ) ;
	
    DESCRIPTION
	Returns the type of a zval as a string constant.
	
    PARAMETERS
	$variable (mixed) -
		Variable whose type is to be retrieved.

     RETURN VALUE
	One of the constants "ZVAL_NULL", "ZVAL_BOOL", "ZVAL_LONG", "ZVAL_STRING", "ZVAL_RESOURCE", "ZVAL_ARRAY", 
	"ZVAL_OBJECT", or "ZVAL_UNKNOWN" if the zval type could not be determined.
	
 *===========================================================================================================*/
PHP_FUNCTION ( zval_type_constant )
   {
	zval *		value ;
	long 		type ;
	
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z", &value )  ==  FAILURE )
		return ;
		
	type 	=  Z_TYPE_P ( value ) ;
	
# ifdef 	PHP_WIN32
# 	pragma 		warning ( disable : 4267 )
# endif
	switch (  type )
	   {
		case	IS_NULL 	:  RETURN_STRING_CONSTANT ( "ZVAL_NULL"     ) ;
		case 	IS_BOOL 	:  RETURN_STRING_CONSTANT ( "ZVAL_BOOL"     ) ;
		case 	IS_LONG 	:  RETURN_STRING_CONSTANT ( "ZVAL_LONG"     ) ;
		case 	IS_DOUBLE 	:  RETURN_STRING_CONSTANT ( "ZVAL_DOUBLE"   ) ;
		case 	IS_STRING 	:  RETURN_STRING_CONSTANT ( "ZVAL_STRING"   ) ;
		case 	IS_RESOURCE 	:  RETURN_STRING_CONSTANT ( "ZVAL_RESOURCE" ) ;
		case 	IS_ARRAY 	:  RETURN_STRING_CONSTANT ( "ZVAL_ARRAY"    ) ;
		case 	IS_OBJECT	:  RETURN_STRING_CONSTANT ( "ZVAL_OBJECT"   ) ;
		default 		:  RETURN_STRING_CONSTANT ( "ZVAL_UNKNOWN"  ) ;
	    }
# ifdef 	PHP_WIN32
# 	pragma 		warning ( default : 4267 )
# endif
    }    
 