/************************************************************************************************************

    NAME
	array_statics.h
	
    DESCRIPTION
	Variables that must be initialized only once.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# ifndef 	__ARRAY_STATICS_H__
# 	define	__ARRAY_STATICS_H__

language_block ( C ) ;

/*-----------------------------------------------------------------------------------------------------------

	Function arguments type information for array.c.
	
 *-----------------------------------------------------------------------------------------------------------*/
/* array_flatten() */
THRAK_BEGIN_ARG_INFO ( array_flatten, 0, 0, 1 )
	THRAK_ARG_INFO ( 0, array )
	THRAK_ARG_INFO ( 0, copy_keys )
THRAK_END_ARG_INFO ;

/* array_ikey_exists() */
THRAK_BEGIN_ARG_INFO ( array_ikey_exists, 0, 0, 2 )
	THRAK_ARG_INFO ( 0, needle )
	THRAK_ARG_INFO ( 0, haystack )
	THRAK_ARG_INFO ( 0, strict ) 
THRAK_END_ARG_INFO ;

/* in_iarray() */
THRAK_BEGIN_ARG_INFO ( iin_array, 0, 0, 2 )
	THRAK_ARG_INFO ( 0, needle )
	THRAK_ARG_INFO ( 0, haystack )
THRAK_END_ARG_INFO ;


/*-----------------------------------------------------------------------------------------------------------

	Function arguments type information for array_buffer.c.
	
 *-----------------------------------------------------------------------------------------------------------*/
/* array_buffer_clone() */
THRAK_BEGIN_ARG_INFO ( array_buffer_clone, 0, 0, 1 )
	THRAK_ARG_INFO ( 0, resource )
THRAK_END_ARG_INFO ;

/* array_buffer_copy() */
THRAK_BEGIN_ARG_INFO ( array_buffer_copy, 0, 0, 2 )
	THRAK_ARG_INFO ( 0, dest )
	THRAK_ARG_INFO ( 0, src )
	THRAK_ARG_INFO ( 0, num_bytes )
	THRAK_ARG_INFO ( 0, dst_offset )
	THRAK_ARG_INFO ( 0, src_offset )
THRAK_END_ARG_INFO ;

/* array_buffer_create() */
THRAK_BEGIN_ARG_INFO ( array_buffer_create, 0, 0, 1 )
	THRAK_ARG_INFO ( 0, size )
THRAK_END_ARG_INFO ;

/* array_buffer_destroy() */
THRAK_BEGIN_ARG_INFO ( array_buffer_destroy, 0, 0, 1 )
	THRAK_ARG_INFO ( 0, resource )
THRAK_END_ARG_INFO ;

/* array_buffer_get() */
THRAK_BEGIN_ARG_INFO ( array_buffer_get, 0, 0, 1 )
	THRAK_ARG_INFO ( 0, resource )
	THRAK_ARG_INFO ( 0, offset )
	THRAK_ARG_INFO ( 0, num_bytes )
THRAK_END_ARG_INFO ;

/* array_buffer_resize() */
THRAK_BEGIN_ARG_INFO ( array_buffer_resize, 0, 0, 2 )
	THRAK_ARG_INFO ( 0, resource )
	THRAK_ARG_INFO ( 0, new_size )
THRAK_END_ARG_INFO ;

/* array_buffer_set() */
THRAK_BEGIN_ARG_INFO ( array_buffer_set, 0, 0, 2 )
	THRAK_ARG_INFO ( 0, buffer )
	THRAK_ARG_INFO ( 0, init_value )
	THRAK_ARG_INFO ( 0, num_bytes )
	THRAK_ARG_INFO ( 0, offset )
THRAK_END_ARG_INFO ;

/* array_buffer_size() */
THRAK_BEGIN_ARG_INFO ( array_buffer_size, 0, 0, 0 )
THRAK_END_ARG_INFO ;

end_language_block ( C ) ;

# endif		/* __ARRAY_STATICS_H__  */