 /************************************************************************************************************

    NAME
	vroom.c
	
    DESCRIPTION
	Vroom-specific functions.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# include 	"php_vroom.h"


/*-----------------------------------------------------------------------------------------------------------

	Global variables.
	
 *-----------------------------------------------------------------------------------------------------------*/
int 			vroom_endianness ;


/*===========================================================================================================

    NAME
	vroom_initialize 
	
    DESCRIPTION
	Register the constants published by the vroom extension.
	
 *===========================================================================================================*/
static zend_bool auto_create_environment ( const char *  name, uint  name_len TSRMLS_DC )
   {
	char ** 		p ;
	char *			var,
	     *			equal_sign ;
	int 			position, length ;
	zval *			env_array ;
	

	// Initialize a PHP array
	MAKE_STD_ZVAL ( env_array ) ;
	array_init ( env_array ) ;
	
	// Loop through environment variables
	for  ( p = environ ; * p  !=  NULL ; p ++ )
	   {
		// Search the length of variable name and value, from left & right side of the equal sign
		var 		=  * p ++ ;
		equal_sign 	=  strchr ( var, '=' ) ;
		length 		=  ( int ) strlen ( var ) ;
		
		// Paranoia : there should always be an equal sign
		if  ( equal_sign  ==  NULL )
			equal_sign 	=  var + length ;
		
		// Position of the variable value
		position 	=  ( int ) ( equal_sign - var ) + 1  ;
		
		// Add this entry to the environment array
		add_assoc_string_ex ( env_array, var, position, var + position, 1 ) ;
	    }
	    
	// All done, this variable is now a superglobal
	ZEND_SET_GLOBAL_VAR_WITH_LENGTH ( "ENVIRONMENT", sizeof ( "ENVIRONMENT" ), env_array, 2, 1 ) ;
	return ( 0 ) ;
    }

    
void 	vroom_initialize ( THRAK_INIT_FUNC_ARGS )
   {
	/* Some information about internal (compiled) data sizes */
	REGISTER_LONG_CONSTANT ( "SIZEOF_CHAR"		, sizeof ( char      )	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "SIZEOF_DOUBLE"	, sizeof ( double    )	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "SIZEOF_FLOAT"		, sizeof ( float     )	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "SIZEOF_INT"		, sizeof ( int       )	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "SIZEOF_LONG"		, sizeof ( long      )	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "SIZEOF_LONG_LONG"	, sizeof ( long long )	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "SIZEOF_PTR"		, sizeof ( void *    )	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "SIZEOF_SHORT"		, sizeof ( short     )	, CONST_CS | CONST_PERSISTENT ) ;
	
	/* Machine endianness */
	REGISTER_LONG_CONSTANT ( "LITTLE_ENDIAN"	, LITTLE_ENDIAN			, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "BIG_ENDIAN"		, BIG_ENDIAN			, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "ENDIANNESS"		, thrak_machine_endianness	, CONST_CS | CONST_PERSISTENT ) ;
	
	/*** 
		Provide an associative array for environment variables.
		Note that the variable cannot be declared in the symbol table during module initialization.
		It must be done after ; this is why an auto_create callback is supplied. This callback will be called at
		appropriate time (after module initialization), when all the symbols are available.
	 ***/
	zend_register_auto_global ( "ENVIRONMENT", sizeof ( "ENVIRONMENT" ) - 1, 0, auto_create_environment TSRMLS_CC ) ;
    }
    
    
/*===========================================================================================================

    NAME
	vroom_shutdown
	
    DESCRIPTION
	Sub-extension shutdown function.
	
 *===========================================================================================================*/
void 	vroom_shutdown ( THRAK_SHUTDOWN_FUNC_ARGS )
   {
    }