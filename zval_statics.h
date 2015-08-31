/************************************************************************************************************

    NAME
	zval_statics.h
	
    DESCRIPTION
	Variables that should be declared only once.
	
    AUTHOR
	Christian Vigh, 10/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# ifndef __ZVAL_STATICS_H__
# 	define 	__ZVAL_STATICS_H__

language_block ( C ) ;

/*-----------------------------------------------------------------------------------------------------------

	Function arguments type information.
	
 *-----------------------------------------------------------------------------------------------------------*/
/* zval_decref() */
THRAK_BEGIN_ARG_INFO ( zval_decref, 0, 0, 1 )
	THRAK_BYREF_ARG_INFO ( variable )
THRAK_END_ARG_INFO ;

/* zval_dump() */
THRAK_BEGIN_ARG_INFO ( zval_dump, 0, 0, 1 )
	THRAK_BYREF_ARG_INFO ( variable )
THRAK_END_ARG_INFO ;

/* zval_incref() */
THRAK_BEGIN_ARG_INFO ( zval_incref, 0, 0, 1 )
	THRAK_BYREF_ARG_INFO ( variable )
THRAK_END_ARG_INFO ;

/* zval_isref() */
THRAK_BEGIN_ARG_INFO ( zval_isref, 0, 0, 1 )
	THRAK_BYREF_ARG_INFO ( variable )
THRAK_END_ARG_INFO ;

/* zval_refcount() */
THRAK_BEGIN_ARG_INFO ( zval_refcount, 0, 0, 1 )
	THRAK_BYREF_ARG_INFO ( variable )
THRAK_END_ARG_INFO ;

/* zval_setref() */
THRAK_BEGIN_ARG_INFO ( zval_setref, 0, 0, 1 )
	THRAK_BYREF_ARG_INFO ( variable )
	THRAK_BYVAL_ARG_INFO ( refcount )
THRAK_END_ARG_INFO ;

/* zval_swap() */
THRAK_BEGIN_ARG_INFO ( zval_swap, 0, 0, 2 )
	THRAK_BYREF_ARG_INFO ( value1 )
	THRAK_BYVAL_ARG_INFO ( value2 )
THRAK_END_ARG_INFO ;

/* zval_type() */
THRAK_BEGIN_ARG_INFO ( zval_type, 0, 0, 1 )
	THRAK_BYVAL_ARG_INFO ( variable )
THRAK_END_ARG_INFO ;

/* zval_type_constant() */
THRAK_BEGIN_ARG_INFO ( zval_type_constant, 0, 0, 1 )
	THRAK_BYVAL_ARG_INFO ( variable )
THRAK_END_ARG_INFO ;

end_language_block ( C ) ;

# endif		/* __ZVAL_STATICS_H__ */
