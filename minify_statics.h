/************************************************************************************************************

    NAME
	minify_statics.h
	
    DESCRIPTION
	Variables that must be initialized only once.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# ifndef 	__MINIFY_STATICS_H__
# 	define	__MINIFY_STATICS_H__

language_block ( C ) ;

/*-----------------------------------------------------------------------------------------------------------

	Function arguments type information for minify.c.
	
 *-----------------------------------------------------------------------------------------------------------*/
/* minify_php() */
THRAK_BEGIN_ARG_INFO ( minify_php, 0, 0, 1 )
	THRAK_ARG_INFO ( 0, file )
	THRAK_ARG_INFO ( 0, preserve_newlines )
THRAK_END_ARG_INFO ;

/* minify_php_string() */
THRAK_BEGIN_ARG_INFO ( minify_php_string, 0, 0, 1 )
	THRAK_ARG_INFO ( 0, string )
	THRAK_ARG_INFO ( 0, preserve_newlines )
THRAK_END_ARG_INFO ;

/* minify_php_stream() */
THRAK_BEGIN_ARG_INFO ( minify_php_stream, 0, 0, 1 )
	THRAK_ARG_INFO ( 0, file )
	THRAK_ARG_INFO ( 0, preserve_newlines )
	THRAK_ARG_INFO ( 0, use_include_path )
THRAK_END_ARG_INFO ;

# endif 	/* __MINIFY_STATICS_H__ */