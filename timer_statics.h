/************************************************************************************************************

    NAME
	timer_statics.h
	
    DESCRIPTION
	Variables that must be initialized only once.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# ifndef 	__TIMER_STATICS_H__
# 	define	__TIMER_STATICS_H__

language_block ( C ) ;

/*-----------------------------------------------------------------------------------------------------------

	Function arguments type information for data.c.
	
 *-----------------------------------------------------------------------------------------------------------*/
/* timer_clock() */
THRAK_BEGIN_ARG_INFO_EX(timer_clock, 0, 0, 0)
	THRAK_BYVAL_ARG_INFO(resolution)
THRAK_END_ARG_INFO ;

/* timer_clock_delta() */
THRAK_BEGIN_ARG_INFO_EX(timer_clock_delta, 0, 0, 0)
	THRAK_BYVAL_ARG_INFO(resolution)
THRAK_END_ARG_INFO ;

/* timer_ticks_to() */
THRAK_BEGIN_ARG_INFO_EX(timer_ticks_to, 0, 0, 1)
	THRAK_BYVAL_ARG_INFO(ticks)
	THRAK_BYVAL_ARG_INFO(resolution)
THRAK_END_ARG_INFO ;

/* timer_ticks_to_seconds() */
THRAK_BEGIN_ARG_INFO_EX(timer_ticks_to_seconds, 0, 0, 1)
	THRAK_BYVAL_ARG_INFO(ticks)
THRAK_END_ARG_INFO ;

/* timer_ticks_to_milliseconds() */
THRAK_BEGIN_ARG_INFO_EX(timer_ticks_to_milliseconds, 0, 0, 1)
	THRAK_BYVAL_ARG_INFO(ticks)
THRAK_END_ARG_INFO ;

/* timer_ticks_to_microseconds() */
THRAK_BEGIN_ARG_INFO_EX(timer_ticks_to_microseconds, 0, 0, 1)
	THRAK_BYVAL_ARG_INFO(ticks)
THRAK_END_ARG_INFO ;

/* timer_ticks_to_nanoseconds() */
THRAK_BEGIN_ARG_INFO_EX(timer_ticks_to_nanoseconds, 0, 0, 1)
	THRAK_BYVAL_ARG_INFO(ticks)
THRAK_END_ARG_INFO ;

/* timer_diff() */
THRAK_BEGIN_ARG_INFO_EX(timer_diff, 0, 0, 2)
	THRAK_BYVAL_ARG_INFO(current)
	THRAK_BYVAL_ARG_INFO(previous)
THRAK_END_ARG_INFO ;

end_language_block ( C ) ;

# endif		/* __TIMER_STATICS_H__  */