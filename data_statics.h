/************************************************************************************************************

    NAME
	data_statics.h
	
    DESCRIPTION
	Variables that must be initialized only once.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# ifndef 	__DATA_STATICS_H__
# 	define	__DATA_STATICS_H__

/*-----------------------------------------------------------------------------------------------------------

	Function arguments type information for data.c.
	
 *-----------------------------------------------------------------------------------------------------------*/
/* data_dxpack() */
THRAK_BEGIN_ARG_INFO_EX(data_dxpack, 0, 0, 1)
	THRAK_BYVAL_ARG_INFO(array)
	THRAK_BYVAL_ARG_INFO(count)
	THRAK_BYVAL_ARG_INFO(endianness)
THRAK_END_ARG_INFO ;

/* data_dxxpack(),  */
THRAK_BEGIN_ARG_INFO_EX(data_dxxpack, 0, 0, 1)
	THRAK_BYVAL_ARG_INFO(array)
	THRAK_BYVAL_ARG_INFO(count)
THRAK_END_ARG_INFO ;

/* data_dxunpack() */
THRAK_BEGIN_ARG_INFO_EX(data_dxunpack, 0, 0, 1)
	THRAK_BYVAL_ARG_INFO(data)
	THRAK_BYVAL_ARG_INFO(count)
	THRAK_BYVAL_ARG_INFO(endianness)
THRAK_END_ARG_INFO ;

/* data_dxxunpack() */
THRAK_BEGIN_ARG_INFO_EX(data_dxxunpack, 0, 0, 1)
	THRAK_BYVAL_ARG_INFO(data)
	THRAK_BYVAL_ARG_INFO(count)
THRAK_END_ARG_INFO ;

/* data_pack() */
THRAK_BEGIN_ARG_INFO_EX(data_pack, 0, 0, 2)
	THRAK_BYVAL_ARG_INFO(format)
	THRAK_BYVAL_ARG_VARIADIC_INFO(args)
THRAK_END_ARG_INFO ;

/* data_unpack() */
THRAK_BEGIN_ARG_INFO_EX(data_unpack, 0, 0, 2)
	THRAK_BYVAL_ARG_INFO(format)
	THRAK_BYVAL_ARG_VARIADIC_INFO(args)
THRAK_END_ARG_INFO ;


# endif		/* __DATA_STATICS_H__  */