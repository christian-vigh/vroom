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
	thrak_ini_get_string - retrieves a .ini setting as a string
	
    PROTOTYPE 
	char *  value 	=  thrak_ini_get_string ( char *  setting, char *  default_value ) ;
	
    DESCRIPTION
		Retrieves a .ini setting as a string.
		
    PARAMETERS
	- setting (char *) :
		.INI file setting whose value is to be retrieved.
		
	- default_value (char *) :
		Default value to be returned when the setting does not exist.
				
    RETURNS
	A string that contains the .ini setting value, NULL if the setting is not defined or empty if defined.
	The default value will be substituted only if the setting is not defined.
	The return value is dynamically allocated and must be freed.
	
 *===========================================================================================================*/
THRAK_API char * 	thrak_ini_get_string ( char *  setting, char *  default_value )
   {
	char * 		result 		=  zend_ini_string ( setting, ( long ) strlen ( setting ) + 1, 0 ) ;

	
	if  ( result  ==  NULL )
	   {
		if  ( default_value  ==  NULL )
			return ( NULL ) ;
		else
			result 	=  default_value ;
	    }
		
	result 	=  thrak_trim ( result, NULL ) ;

	return ( result ) ;
    }
 
 
/*===========================================================================================================

    NAME
	thrak_ini_parse_byte_quantity - retrieves a byte quantity from a .INI value.
	
    PROTOTYPE 
	status 	=  thrak_ini_parse_byte_quantity ( char *  value, thrak_byte_quantity *  quantity ) ;
	
    DESCRIPTION
	Gets a byte quantity from a .INI setting. Quantities can be :
	- A byte count, specified as an integer
	- An integer followed by a string representing a quantity :
		- "k" or "kb" for kilobytes
		- "m" or "mb" for megabytes
		- "g" or "gb" for gigabytes
	
    PARAMETERS
	- value (char *) :
		String value to be parsed.
		
	- quantity (thrak_byte_quantity *) :
		A structure that will receive the parsed quantity information. It has the following fields :
		- bytes (long) :
			Total number of bytes.
		- quantity (long) :
			The initial quantity (for example, "1024" when the initial value is "1024mb").
		- multiplier (long) :
			The quantity multiplier (1024 for kilobytes, etc.).
		- multiplier_string (char *) :
			Noprmalized multiplier string ("Kb" for kilobytes, "Mb" for megabytes, etc.)
		
    RETURNS
	1 if the function succeded, 0 otherwise.
	
 *===========================================================================================================*/
THRAK_API int  thrak_ini_parse_byte_quantity ( char *  value, thrak_byte_quantity *  qstruct )
   {
	char *		start	=  value,	
	     *		end 	=  NULL ;
	long 		quantity ;
	char *		multiplier_string 		=  "" ;
	int 		multiplier			=  1 ;
	
	
	/* Get a default value, if ini setting is empty */
	if  ( value  ==  NULL  ||  ! * value )
		return ( 0 ) ;
	
	/* Get first part, which should be a positive integer */
	quantity 	=  strtol ( start, & end, 0 ) ;
	
	/* Error checking : quantity must start with an integer value */
	if  ( end  ==  NULL )
		return ( 0 ) ;
		
	/* Error checking : size must be strictly positive */
	if  ( quantity  <  0 )
		return ( 0 ) ;
	    
	
	/* Skip spaces after the leading integer */
	value 	=  end ;

	while  ( * value  &&  isspace ( * value ) )
		value ++ ;
		
	/* If something after, it must be a multiplier (kb, mb, etc) */
	if  ( * value )
	   {
		if  ( ! strcasecmp ( value, "b" ) )
			;
		else if  ( ! strcasecmp ( value, "k" )  ||  ! strcasecmp ( value, "kb" ) )
		   {
			multiplier_string 	=  "Kb" ;
			multiplier 		=  1024 ;
		    }
		else if  ( ! strcasecmp ( value, "m" )  ||  ! strcasecmp ( value, "mb" ) )
		   {
			multiplier_string 	=  "Mb" ;
			multiplier 		=  1024 * 1024;
		    }
		else if  ( ! strcasecmp ( value, "g" )  ||  ! strcasecmp ( value, "gb" ) )
		   {
			multiplier_string 	=  "Gb" ;
			multiplier 		=  1024 * 1024 * 1024 ;
		    }
		else
			return ( 0 ) ;
	    }

	/* Initialize the byte_quantity structure */
	qstruct -> bytes 		=  quantity * multiplier ;
	qstruct -> quantity 		=  quantity ;
	qstruct -> multiplier_string	=  multiplier_string ;
	qstruct -> multiplier 		=  multiplier ;
	
	/* All done, report everything is ok */
	return ( 1 ) ;
    }
    
    
/*===========================================================================================================

    NAME
	thrak_ini_parse_percentage - retrieves a percentage from a .INI value.
	
    PROTOTYPE 
	status 	=  thrak_ini_parse_percentage ( char *  value, double *  percentage ) ;
	
    DESCRIPTION
	Gets a percentage value from a .ini setting.
	
    PARAMETERS
	- value (char *) :
		.INI file setting whose value is to be parsed.
		
	- percentage (double *) :
		Address of a doiuble that will receive the parsed value.
		
    RETURNS
	1 if the function succeded, 0 otherwise.
	
 *===========================================================================================================*/
THRAK_API int  thrak_ini_parse_percentage ( char *  value, double *  percentage )
   {
	char *		start	=  value,	
	     *		end 	=  NULL ;
	double 		percentage_value ;
	
	
	/* Note a quantity : maybe a percentage. Get first part, which should be a positive float */
	percentage_value 	=  strtod ( start, & end ) ;

	/* Error checking : quantity must start with a float value value */
	if  ( end  ==  NULL )
		return ( 0 ) ;

	/* Error checking : percentage must be in the range [0..100] */
	if  ( percentage_value  <  0   ||  percentage_value  >  100 )
		return ( 0 ) ;
		
	/* Skip spaces after the leading integer */
	value 	=  end ;

	while  ( * value  &&  isspace ( * value ) )
		value ++ ;
		
	/* If something is after the float value, it must be a percent sign */
	if  ( * value )
	   {
		if (  strcmp ( value, "%" ) )
			return ( 0 ) ;
	    }
	else
		return ( 0 ) ;
	    
	// This is a percentage
	* percentage 	=  percentage_value ;
	
	return ( 1 ) ;
    }
    
    
/*===========================================================================================================

    NAME
	thrak_ini_parse_memory_quantity - retrieves a byte quantity or percentage from a .INI value.
	
    PROTOTYPE 
	status 	=  thrak_ini_parse_memory_quantity ( char *  value, thrak_memory_quantity *  quantity ) ;
	
    DESCRIPTION
	Gets a free memory quantity from a .INI setting. This can be expressed as a quantity or percentage.
	
    PARAMETERS
	- value (char *) :
		.INI file setting whose value is to be retrieved.
		
	- quantity (thrak_memory_quantity *) :
		A structure that will receive the parsed information. It has the following fields :
		- bytes (long) :
			Total number of bytes.
		- quantity (long) :
			The initial quantity (for example, "1024" when the initial value is "1024mb").
		- multiplier (long) :
			The quantity multiplier (1024 for kilobytes, etc.).
		- multiplier_string (char *) :
			Noprmalized multiplier string ("Kb" for kilobytes, "Mb" for megabytes, etc.)
		
    RETURNS
	1 if the function succeded, 0 otherwise.
	
 *===========================================================================================================*/
THRAK_API int  thrak_ini_parse_memory_quantity ( char *  value, thrak_memory_quantity *  mstruct )
   {
	/* Try to recognize a byte quantity first */
	if  ( thrak_ini_parse_byte_quantity ( value, & mstruct -> quantity ) )
	   {
		mstruct -> type 	=  0 ;
		
		return ( 1 ) ;
	    }
	    
	/* Then a percentage */
	if  ( thrak_ini_parse_percentage ( value, & mstruct -> percentage ) )
	   {
		mstruct -> type 	=  1 ;

		return ( 1 ) ;
	    }
	    
	return ( 0 ) ;
    }
    