/************************************************************************************************************

    NAME
	string_statics.h
	
    DESCRIPTION
	Variables that must be initialized only once.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# ifndef 	__STRING_STATICS_H__
# 	define	__STRING_STATICS_H__

language_block ( C ) ;

/*-----------------------------------------------------------------------------------------------------------

	Function arguments type information for data.c.
	
 *-----------------------------------------------------------------------------------------------------------*/
/* string_atoi() */
THRAK_BEGIN_ARG_INFO_EX(string_atoi, 0, 0, 1)
	THRAK_BYVAL_ARG_INFO(value)
	THRAK_BYREF_ARG_INFO(end)
THRAK_END_ARG_INFO ;

/* string_append() */
THRAK_BEGIN_ARG_INFO_EX(string_append, 0, 0, 2)
	THRAK_BYREF_ARG_INFO(value)
	THRAK_BYVAL_ARG_VARIADIC_INFO(args)
THRAK_END_ARG_INFO ;

/* string_endswith() */
THRAK_BEGIN_ARG_INFO_EX(string_endswith, 0, 0, 2)
	THRAK_BYVAL_ARG_INFO(str)
	THRAK_BYVAL_ARG_INFO(suffix)
	THRAK_BYVAL_ARG_INFO(case_insensitive)
THRAK_END_ARG_INFO ;

/* string_explode() */
THRAK_BEGIN_ARG_INFO_EX(string_explode, 0, 0, 2)
	THRAK_BYVAL_ARG_INFO(separator)
	THRAK_BYVAL_ARG_INFO(value)
	THRAK_BYVAL_ARG_INFO(left)
	THRAK_BYVAL_ARG_INFO(right)
	THRAK_BYVAL_ARG_INFO(option)
	THRAK_BYVAL_ARG_INFO(callback)
THRAK_END_ARG_INFO ;

/* string_implode() */
THRAK_BEGIN_ARG_INFO_EX(string_implode, 0, 0, 2)
	THRAK_BYVAL_ARG_INFO(separator)
	THRAK_BYVAL_ARG_INFO(value)
	THRAK_BYVAL_ARG_INFO(left)
	THRAK_BYVAL_ARG_INFO(right)
	THRAK_BYVAL_ARG_INFO(option)
	THRAK_BYVAL_ARG_INFO(callback)
THRAK_END_ARG_INFO ;

/* string_startswith() */
THRAK_BEGIN_ARG_INFO_EX(string_startswith, 0, 0, 2)
	THRAK_BYVAL_ARG_INFO(str)
	THRAK_BYVAL_ARG_INFO(prefix)
	THRAK_BYVAL_ARG_INFO(case_insensitive)
THRAK_END_ARG_INFO ;

/* string_seteos() */
THRAK_BEGIN_ARG_INFO_EX(string_seteos, 0, 0, 2)
	THRAK_BYREF_ARG_INFO(str)
	THRAK_BYVAL_ARG_INFO(suffix)
	THRAK_BYVAL_ARG_INFO(case_insensitive)
	THRAK_BYVAL_ARG_INFO(return_value)
THRAK_END_ARG_INFO ;

/* string_setbos() */
THRAK_BEGIN_ARG_INFO_EX(string_setbos, 0, 0, 2)
	THRAK_BYREF_ARG_INFO(str)
	THRAK_BYVAL_ARG_INFO(prefix)
	THRAK_BYVAL_ARG_INFO(case_insensitive)
	THRAK_BYVAL_ARG_INFO(return_value)
THRAK_END_ARG_INFO ;

end_language_block ( C ) ;

# endif		/* __STRING_STATICS_H__  */