/************************************************************************************************************

    NAME
	thrak.c
	
    DESCRIPTION
	Helper functions for writing PHP extensions.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# include 	"thrak.h"
# include 	<zend_alloc.h>


/*-----------------------------------------------------------------------------------------------------------

	Global variables.
	
 *-----------------------------------------------------------------------------------------------------------*/
int 		thrak_machine_endianness ;


/*===========================================================================================================

    NAME
	thrak_initialize - initializes the thrak utility package.
		
 *===========================================================================================================*/
void   thrak_initialize ( THRAK_INIT_FUNC_ARGS )
   {
	thrak_machine_endianness 	=  thrak_get_machine_endianness ( ) ;
    }

    
/*===========================================================================================================

    NAME
	thrak_shutdown - Shutdows the thrak utility package.
		
 *===========================================================================================================*/
void 	thrak_shutdown ( THRAK_SHUTDOWN_FUNC_ARGS )
   {
    }
    

/*===========================================================================================================

    NAME
	thrak_get_machine_endianness - Returns the machine endianness.
	
    PROTOTYPE 
	endianness 	=  thrak_machine_endianness ( ) ;
	
    DESCRIPTION
	Returns the endianness of the current machine.
	
    RETURNS
	One of the constants LITTLE_ENDIAN or BIG_ENDIAN.
	
 *===========================================================================================================*/
int 	thrak_get_machine_endianness ( )
   {
	unsigned char 		bytes [ sizeof ( unsigned int ) ] ;
	unsigned int		test_value 		=  0 ;
	unsigned int 		current_value 		=  0x00 ;
	int 			i ;
	

	for  ( i = 0 ; i  <  sizeof ( unsigned int ) ; i ++ )
	   {
		current_value ++ ;
		test_value 	=  ( test_value  <<  8 )  |  current_value ;
	    }
	    
	* ( ( unsigned int * ) bytes ) 	=  test_value ;
	
	if  ( bytes [0]  ==  current_value )
		return ( LITTLE_ENDIAN ) ;
	else
		return ( BIG_ENDIAN ) ;
    }