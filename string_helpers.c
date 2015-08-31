/************************************************************************************************************

    NAME
	string_helpers.c
	
    DESCRIPTION
	Helper functions for string manipulations.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# include 	"thrak.h"
# include 	<ctype.h>
# include 	<math.h>



/*===========================================================================================================

    thrak_atoi - 
	Extended atoi implementation. The output_position parameter can be null.
	When escape_only is non-zero, a leading sign is forbidden and the value cannot exceed 255.
	
 *===========================================================================================================*/
THRAK_API zend_bool 	thrak_atoi ( char *  input_value, char **  output_position, int64 *  output_value, int  escape_only )
   {
	char *		p 		=  input_value ;		/* Pointer to current char of input value 		*/
	int 		sign 		=  0 ;				/* Sign (0 = unspecified) 				*/
	int 		base 		=  10 ;				/* Conversion base 					*/
	int64		value 		=  0 ;				/* Current conversion value 				*/
	int 		digit_count 	=  0 ;				/* Number of digits processed (0 = conversion error)	*/
	
	
	/* Allow leading sign only for real integers, not escape sequences */
	if  ( ! escape_only )
	   {
		/* Facility : ignore leading spaces */
		while  ( * p  &&  isspace ( * p ) )
			p ++ ;
	
		/* If a sign is present, remember it */
		if  ( * p  ==  '-' )
			sign = -1, p ++ ;
		else if  ( * p  ==  '+' ) 
			p ++, sign = 1 ;
			
		/* Facility II : ignore spaces between sign and first digits */
		while  ( * p  &&  isspace ( * p ) )
			p ++ ;
	    }
	
	/* If a number starts with zero, it may be specified in octal... */
	if  ( * p  ==  '0' )
	   {
		base 		=  8 ;
		p ++ ;
	    }
	    
	/* ... unless the character following is a base specifier */
	switch  ( tolower ( * p ) )
	   {
		case 	'b' :
			base 		 =  2 ;
			p ++ ;
			break ;
			
		case 	'o' :
			base 		 =  8 ;
			p ++ ;
			break ;
			
		case 	'd' :
			base 		 =  10 ;
			p ++ ;
			break ;
			
		case 	'x' :
			base 		 =  16 ;
			p ++ ;
			break ;
	    }
	    
	/* Loop through digits */
	while  ( isxdigit ( * p ) )
	   {
		char  		ch	=  ( * p  >  '9' ) ?  toupper ( * p ) - 'A' + 10 : * p - '0' ;
		int64  		tmpval ;
		
		/* Stop on first digit not belonging to the current base */
		if  ( ch  >=  base )
			break ;
		
		/* If we interpret an escaped character sequence, make sure we don't overflow */
		tmpval 	=  ( value * base ) + ch ;
		
		if  ( escape_only  &&  tmpval  >  255 )
			break ;
			
		/* Proceed with next character */
		value 	=  tmpval ;
		digit_count ++ ;
		p ++ ;
	    }

	/* No digit found : signal a conversion error */
	if  ( ! digit_count )
	   {
		if  ( output_position  !=  NULL )
			* output_position 	=  input_value ;
	
		return ( 0 ) ;
	    }
	    
	/* If an output position pointer has been supplied, set it to the first character after the last digit */
	if  ( output_position  !=  NULL )
		* output_position	 =  p ;
	
	/* Don't forget the leading sign, if any */
	if  ( sign )
		value 	*=  sign ;
		
	/* All done, return */
	* output_value 	=  value ;
	return ( 1 ) ;
    }
    

/*===========================================================================================================

    thrak_strichr - 
	strichr() implementation for systems that don't support it/
	
 *===========================================================================================================*/
THRAK_API char *  thrak_strichr ( const char *  p, int  ch )
   {
	ch 	=  toupper ( ch ) ;
	
	while  ( * p )
	   {
		if  ( toupper ( * p )  ==  ch )
			return ( ( char * ) p ) ;
			
		p ++ ;
	    }
	    
	return ( NULL ) ;
    }
    
    
/*===========================================================================================================

    thrak_trim/ltrim/rtrim - 
	Trims a value. Puts the result in the "output" parameter if specified, or returns a newly
	allocated string.
	
 *===========================================================================================================*/
static char *  __thrak_trim ( char *  p, int  mode, char *  output )
   {
	char * 		start 	=  p,
	     *		end 	=  p + strlen ( p ) - 1 ;
	long 		size ;
	     
	if  ( mode  &  1 )
	   {
		while  ( * start  &&  isspace ( * start ) )
			start ++ ;
	    }
	    
	    
	if  ( mode  &  2 )
	   {
		while  ( end  >  start  &&  isspace ( * end ) )
			end -- ;
	    }
	
	size 	=  ( long ) ( end - start + 1 ) ;
	
	if  ( output  !=  NULL )
	   {
		strncpy ( output, start, size ) ;
		* ( end + 1 )	=  '\0' ;
		
		return  ( output ) ;
	    }
	else
		return  ( estrndup ( start, size ) ) ;
    }

THRAK_API char *  thrak_trim ( char *  p, char *  output )
   { return  ( __thrak_trim ( p, 0x03, output ) ) ; }
   
   
THRAK_API char *  thrak_ltrim ( char *  p, char *  output )
   { return  ( __thrak_trim ( p, 0x01, output ) ) ; }    
   
THRAK_API char *  thrak_rtrim ( char *  p, char *  output )
   { return  ( __thrak_trim ( p, 0x02, output ) ) ; }
   
