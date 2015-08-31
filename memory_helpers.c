/************************************************************************************************************

    NAME
	memory_helpers.c
	
    DESCRIPTION
	Memory-related functions.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# include 	"thrak.h"
# include 	<zend_alloc.h>


/*===========================================================================================================

    NAME
	thrak_memory_check_constraint - Checks if new allocation will fit into memory.
	
    PROTOTYPE 
	int 	thrak_memory_check_constraint ( long  memory_requested, 
						thrak_memory_quantity *  constraint TSRMLS_DC ) ;
	
    DESCRIPTION
	Checks if the requested amount of memory specified by the memory_requested parameter will satisfy the
	constraints specified by a thrak_memory_quantity structure.
	A constraint can be :
	- A minimum quantity of memory to be left after allocation
	- A minimum percentage of memory to be left
	
    PARAMETERS
	memory_requested (long) -
		Number of bytes to allocate.
		
	constraint (thrak_memory_quantity *) -
		Constraint on the available quantity of memory that must remain after allocation.
		
    RETURNS
	0 if constraint not satisfied, 1 otherwise.
	
 *===========================================================================================================*/
THRAK_API int  	thrak_memory_check_constraint ( long  memory_requested, thrak_memory_quantity *  constraint TSRMLS_DC )
   {
	thrak_byte_quantity 		memory_limit ;
	size_t 				current_memory 		=  zend_memory_usage ( 1 TSRMLS_CC ) ;
	
	
	thrak_ini_parse_byte_quantity ( INI_STR ( "memory_limit" ), & memory_limit ) ;
	
	if  ( constraint -> type  ==  0 )
	   {
		if  ( memory_limit. bytes - ( current_memory + memory_requested )  >  constraint -> quantity. bytes )
			return ( 1 ) ;
		else
			return ( 0 ) ;
	    }
	else
	   {
		long 		min_memory_pc 	=  ( long ) ( memory_limit. bytes - ( ( memory_limit. bytes * constraint -> percentage ) / 100 ) ) ;
		
		if  ( current_memory + memory_requested  <  min_memory_pc )
			return ( 1 ) ;
		else
			return ( 0 ) ;
	    }
    }