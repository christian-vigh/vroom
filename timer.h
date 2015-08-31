/************************************************************************************************************

    NAME
	timer.h
	
    DESCRIPTION
	Definitions for timer-related functions.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# ifndef 	__TIMER_H__
# 	define	__TIMER_H__


# if  ( ! PLATFORM_WINDOWS  &&  ! PLATFORM_APPLE  &&  ! PLATFORM_POSIX )
#	error 	Unable to determine current platform.
# endif

language_block ( C ) ;

/*-----------------------------------------------------------------------------------------------------------

	Platform-dependent includes.
	
 *-----------------------------------------------------------------------------------------------------------*/
# if	PLATFORM_WINDOWS
# elif 	PLATFORM_APPLE
# 	include 	<mach/mach_time.h> 
# else 
#  	include 	<time.h> 
# endif

 
/*-----------------------------------------------------------------------------------------------------------

	Macros & constants.
	
 *-----------------------------------------------------------------------------------------------------------*/
# define 	TIMER_RESOLUTION_NATIVE				0
# define 	TIMER_RESOLUTION_SECOND 			1
# define 	TIMER_RESOLUTION_MILLISECOND			2
# define 	TIMER_RESOLUTION_MICROSECOND 			3
# define 	TIMER_RESOLUTION_NANOSECOND 			4


/*-----------------------------------------------------------------------------------------------------------

	Types.
	
 *-----------------------------------------------------------------------------------------------------------*/
# if 	PLATFORM_WINDOWS
typedef 	unsigned __int64 				tick_t ; 
# else 
typedef 	uint64_t         				tick_t ;
# endif 

/*-----------------------------------------------------------------------------------------------------------

	External functions.
	
 *-----------------------------------------------------------------------------------------------------------*/
extern				PHP_FUNCTION  ( timer_clock ) ;
extern THRAK_API double 	internal_timer_clock 				( void ) ;

extern 				PHP_FUNCTION ( timer_clock_delta ) ;
extern THRAK_API double 	internal_timer_clock_delta 			( double  	ticks ) ;

extern 				PHP_FUNCTION ( timer_diff ) ;
extern THRAK_API double 	internal_timer_diff 				( double  	current,
										  double 	previous ) ;

extern 				PHP_FUNCTION  ( timer_ticks_to ) ;
extern 				PHP_FUNCTION  ( timer_ticks_to_seconds ) ;
extern 				PHP_FUNCTION  ( timer_ticks_to_milliseconds ) ;
extern 				PHP_FUNCTION  ( timer_ticks_to_microseconds ) ;
extern 				PHP_FUNCTION  ( timer_ticks_to_nanoseconds ) ;
extern THRAK_API double 	internal_timer_ticks_to				( double 	ticks,
										  int 		resolution ) ;


/* initialization/shutdown */
extern void 			timer_initialize			( THRAK_INIT_FUNC_ARGS ) ;
extern void 			timer_shutdown				( THRAK_SHUTDOWN_FUNC_ARGS ) ;

end_language_block ( C ) ;

# endif		/* __TIMER_H__  */