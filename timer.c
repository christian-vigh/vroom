/************************************************************************************************************

    NAME
	timer.c
	
    DESCRIPTION
	timer-related operations.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# include 	"php_vroom.h"
# include 	"timer.h"


/*-----------------------------------------------------------------------------------------------------------

	Platform-dependent variables.
	
 *-----------------------------------------------------------------------------------------------------------*/
# if 	PLATFORM_APPLE
static mach_timebase_info_data_t 		__timebase_info; 

static zend_always_inline void  absolutetime_to_nanoseconds ( uint64_t  mach_time, uint64_t *  clock ) 
   { 
	*clock 	= ( mach_time * _timebase_info. numer ) / _timebase_info. denom ; 
    }
# endif    


/*-----------------------------------------------------------------------------------------------------------

	Global variables.
	
 *-----------------------------------------------------------------------------------------------------------*/
static tick_t 			__timer_ticks_per_second    		=  0 ; 		/* Timer frequency 		*/
static double 			__timer_frequency  			=  0 ; 		/* 1 / timer frequency		*/
static double 			__last_timer_clock 			=  0 ;		/* Last clock value		*/


/*===========================================================================================================

    NAME
	timer_initialize - Initializes the time functions.
		
 *===========================================================================================================*/
void 	timer_initialize ( THRAK_INIT_FUNC_ARGS )
   {
	int  			status 		=  1 ;
# if 	PLATFORM_WINDOWS 
	tick_t 			unused ;
# elif  PLATFORM_POSIX 
	struct timespec 	ts 		=  { .tv_sec = 0, .tv_nsec = 0 } ; 	
# endif
	
	/* Register time resolution constants */
	REGISTER_LONG_CONSTANT ( "TIMER_RESOLUTION_SECOND"		, TIMER_RESOLUTION_SECOND	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "TIMER_RESOLUTION_MILLISECOND"		, TIMER_RESOLUTION_MILLISECOND	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "TIMER_RESOLUTION_MICROSECOND"		, TIMER_RESOLUTION_MICROSECOND	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "TIMER_RESOLUTION_NANOSECOND"		, TIMER_RESOLUTION_NANOSECOND	, CONST_CS | CONST_PERSISTENT ) ;

	/* Get the timer frequency */
# if 	PLATFORM_WINDOWS 
        if ( ! QueryPerformanceFrequency ( ( LARGE_INTEGER * ) & __timer_ticks_per_second ) || 
             ! QueryPerformanceCounter   ( ( LARGE_INTEGER * ) & unused ) ) 
		status 	=  0 ;
# elif 	PLATFORM_APPLE 
        if (  mach_timebase_info ( & __timebase_info ) ) 
		status 	=  0 ;
	else
		__timer_ticks_per_second 	=  1000000000ULL ; 
# elif 	PLATFORM_POSIX 
        if ( clock_gettime ( CLOCK_MONOTONIC, & ts ) ) 
		status 	=  0 ;
	else
		__timer_ticks_per_second 	=  1000000000ULL ; 
# endif 

	if  ( ! status )
		php_error_docref ( NULL TSRMLS_CC, E_ERROR, "Unable to determine high-resolution clock frequency." ) ;
		
	__timer_frequency 	=  1.0 / ( double ) __timer_ticks_per_second ;
	
	/* Register time frequency constants */
	REGISTER_DOUBLE_CONSTANT ( "TIMER_TICKS_PER_SECOND"	, ( double ) __timer_ticks_per_second	, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_DOUBLE_CONSTANT ( "TIMER_FREQUENCY"		, __timer_frequency			, CONST_CS | CONST_PERSISTENT ) ;
    }

    
/*===========================================================================================================

    NAME
	timer_shutdown - Shutdowns the time functions submodule.
		
 *===========================================================================================================*/
void 	timer_shutdown ( THRAK_SHUTDOWN_FUNC_ARGS )
   {
    }
    
    
/*===========================================================================================================

    NAME
	timer_clock - Returns the current value of the system high-resolution clock.
	
    PROTOTYPE
	$value 	=  timer_clock ( [ $resolution = TIMER_RESOLUTION_NATIVE ]) ;
	
    DESCRIPTION 
	Returns the current value of the high resolution clock.
	
    PARAMETERS
	$resolution (integer) -
		Indicates the resolution of the returned value :
		- TIMER_RESOLUTION_NATIVE :
			The natural number of ticks will be returned?
		- TIMER_RESOLUTION_SECOND, MILLISECOND, MICROSECOND, NANO_SECOND :
			The returned value will be converted to the specified unit.
			
    RETURNS
	The current value of the high resolution clock.
		
 *===========================================================================================================*/
THRAK_API double 	internal_timer_clock  ( void )
   {
	double 		ticks ;
	
	
# if 	PLATFORM_WINDOWS 
        tick_t 		current_clock ; 

        QueryPerformanceCounter ( ( LARGE_INTEGER * ) & current_clock ) ; 
	
        ticks 	=  ( double ) current_clock ; 
	
# elif  PLATFORM_APPLE 
        tick_t 		current_clock	=  0 ; 

        absolutetime_to_nanoseconds ( mach_absolute_time ( ), & current_clock ) ; 

        ticks 	=  ( double ) current_clock ;
	
# elif 	PLATFORM_POSIX 
        struct timespec 	ts 	= { .tv_sec = 0, .tv_nsec = 0 } ; 

        clock_gettime ( CLOCK_MONOTONIC, & ts ) ; 

        ticks	=  ( double ) ( ts.tv_sec * 1000000000ULL ) + ts. tv_nsec ) ; 
# endif 

	__last_timer_clock 	=  ticks ;
	
	return ( ticks ) ;
   }
    
    
PHP_FUNCTION ( timer_clock )
   {
	double 		ticks ;
	int 		resolution 		=  TIMER_RESOLUTION_NATIVE ;
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "|l", & resolution )  ==  FAILURE )
		return;
	
	// Get the number of elapsed ticks
	ticks 	=  internal_timer_clock ( ) ;
	
	// Convert to the requested resolution
	ticks 	=  internal_timer_ticks_to ( ticks, resolution ) ;
	
	RETURN_DOUBLE ( ticks ) ;
    }
    
    
/*===========================================================================================================

    NAME
	timer_ticks_to - Converts clock ticks to the specified resolution.
	
    PROTOTYPE
	$value 	=  timer_ticks_to ( $ticks [,  $resolution = TIMER_RESOLUTION_NATIVE ]) ;
	$value 	=  timer_ticks_to_seconds ( $ticks ) ;
	$value 	=  timer_ticks_to_milliseconds ( $ticks ) ;
	$value 	=  timer_ticks_to_microseconds ( $ticks ) ;
	$value 	=  timer_ticks_to_nanoseconds ( $ticks ) ;
	
    DESCRIPTION 
	Converts timer ticks to the specified resolution.
	
    PARAMETERS
	$ticks (double) -
		Tick count to be converted.
		
	$resolution (integer) -
		Indicates the resolution of the returned value :
		- TIMER_RESOLUTION_NATIVE :
			The natural number of ticks will be returned.
		- TIMER_RESOLUTION_SECOND, MILLISECOND, MICROSECOND, NANO_SECOND :
			The supplied tick count value will be converted to the specified unit.
			
    RETURNS
	The number of ticks supplied, converted to the specified resolution.
		
 *===========================================================================================================*/
THRAK_API double  internal_timer_ticks_to  ( double  ticks, int  resolution ) 
   {
	switch  ( resolution )
	   {
		case 	TIMER_RESOLUTION_SECOND :
			ticks 	=  ( ticks * __timer_frequency ) ;
			break ;
		
		case 	TIMER_RESOLUTION_MILLISECOND :
			ticks 	=  ( ticks * ( 1000.0 ) ) * __timer_frequency ;
			break ;
		
		case 	TIMER_RESOLUTION_MICROSECOND :
			ticks 	=  ( ticks * ( 1000.0 * 1000.0 ) ) * __timer_frequency ;
			break ;
		
		case 	TIMER_RESOLUTION_NANOSECOND :
			ticks 	=  ( ticks * ( 1000.0 * 1000.0 * 1000.0 ) ) * __timer_frequency ;
			break ;
		
		case 	TIMER_RESOLUTION_NATIVE :
		default :
			break ;
	    }
	    
	return ( ticks ) ;
    }
    
PHP_FUNCTION ( timer_ticks_to )
   {
	double 		ticks ;
	int 		resolution 		=  TIMER_RESOLUTION_NATIVE ;
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "d|l", & ticks, & resolution )  ==  FAILURE )
		return;
		
	ticks 	=  internal_timer_ticks_to ( ticks, resolution ) ;
	
	RETURN_DOUBLE ( ticks ) ;
    }     
     
PHP_FUNCTION ( timer_ticks_to_seconds )
   {
	double 		ticks ;
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "d", & ticks )  ==  FAILURE )
		return;
		
	ticks 	=  internal_timer_ticks_to ( ticks, TIMER_RESOLUTION_SECOND ) ;
	
	RETURN_DOUBLE ( ticks ) ;
    }     
 
      
PHP_FUNCTION ( timer_ticks_to_milliseconds )
   {
	double 		ticks ;
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "d", & ticks )  ==  FAILURE )
		return;
		
	ticks 	=  internal_timer_ticks_to ( ticks, TIMER_RESOLUTION_MILLISECOND ) ;
	
	RETURN_DOUBLE ( ticks ) ;
    }     
      
PHP_FUNCTION ( timer_ticks_to_microseconds )
   {
	double 		ticks ;
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "d", & ticks )  ==  FAILURE )
		return;
		
	ticks 	=  internal_timer_ticks_to ( ticks, TIMER_RESOLUTION_MICROSECOND ) ;
	
	RETURN_DOUBLE ( ticks ) ;
    }     
      
PHP_FUNCTION ( timer_ticks_to_nanoseconds )
   {
	double 		ticks ;
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "d", & ticks )  ==  FAILURE )
		return;
		
	ticks 	=  internal_timer_ticks_to ( ticks, TIMER_RESOLUTION_NANOSECOND ) ;
	
	RETURN_DOUBLE ( ticks ) ;
    }     
 
 
 
/*===========================================================================================================

    NAME
	timer_diff - Computes delta between two timer values.
	
    PROTOTYPE
	$value 	=  timer_diff ( $current, $previous [, $resolution = TIMER_RESOLUTION_NATIVE ]) ;
	
    DESCRIPTION 
	Computes the differences between two timer clock values and returns the result, converted to the
	specified resolution.
	
    PARAMETERS
	$current (double) -
		Current timer value.
		
	$previous (double) -
		Previous timer value.
		
	$resolution (integer) -
		Indicates the resolution of the returned value :
		- TIMER_RESOLUTION_NATIVE :
			The natural number of ticks will be returned?
		- TIMER_RESOLUTION_SECOND, MILLISECOND, MICROSECOND, NANO_SECOND :
			The returned value will be converted to the specified unit.
			
    RETURNS
	The delta between the current and previous timer values, converted to the specified resolution.
		
 *===========================================================================================================*/
PHP_FUNCTION ( timer_diff )
   {
	double 		ticks, current, previous ;
	int 		resolution 		=  TIMER_RESOLUTION_NATIVE ;
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "dd|l", & current, & previous, & resolution )  ==  FAILURE )
		return;
	
	// Get the number of elapsed ticks
	ticks 	=  current - previous ;
	
	// Convert to the requested resolution
	ticks 	=  internal_timer_ticks_to ( ticks, resolution ) ;
	
	RETURN_DOUBLE ( ticks ) ;
    }

    
/*===========================================================================================================

    NAME
	timer_clock_delta - Returns the number of ticks elapsed since the last call to timer_clock().
	
    PROTOTYPE
	$value 	=  timer_clock_delta ( [ $resolution = TIMER_RESOLUTION_NATIVE ]) ;
	
    DESCRIPTION 
	Returns the delta between the call of the last timer_clock() or timer_clock_delta() functions.
	If none of these functions was called, returns the current clock value.
	
    PARAMETERS
	$resolution (integer) -
		Indicates the resolution of the returned value :
		- TIMER_RESOLUTION_NATIVE :
			The natural number of ticks will be returned?
		- TIMER_RESOLUTION_SECOND, MILLISECOND, MICROSECOND, NANO_SECOND :
			The returned value will be converted to the specified unit.
			
    RETURNS
	The delta between the current clock and the value of the clock during the last call to 
	timer_clock_delta() or timer_clock().
		
 *===========================================================================================================*/
PHP_FUNCTION ( timer_clock_delta )
   {
	double 		old_ticks		=  __last_timer_clock, 
			current_ticks,
			ticks ;
	int 		resolution 		=  TIMER_RESOLUTION_NATIVE ;
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "|l", & resolution )  ==  FAILURE )
		return;
	
	// Get the number of elapsed ticks
	current_ticks 	=  internal_timer_clock ( ) ;
	
	// Convert to the requested resolution
	ticks 	=  internal_timer_ticks_to ( current_ticks - old_ticks, resolution ) ;
	
	RETURN_DOUBLE ( ticks ) ;
    }
    
    
