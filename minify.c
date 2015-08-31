/************************************************************************************************************

    NAME
	minify.c
	
    DESCRIPTION
	Minifying functions.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# include 	"php_vroom.h"
# include 	"minify.h"


/*===========================================================================================================

    NAME
	minify_initialize - initializes the array_buffer module.

 *===========================================================================================================*/
void 	minify_initialize	( THRAK_INIT_FUNC_ARGS )
   {
    }

    
/*===========================================================================================================

    NAME
	minify_shutdown
	
    DESCRIPTION
	Sub-extension shutdown function.
	
 *===========================================================================================================*/
void 	minify_shutdown ( THRAK_SHUTDOWN_FUNC_ARGS )
   {
    }    

    
    
/*===========================================================================================================

    internal_minify_php -
	Does the actual minifying work.
	This function has been optimized and is around 300% faster than using php_strip_whitespace(). It is
	designed for fast easy compression of PHP files before autoload.
	
 *===========================================================================================================*/
 
/* Customized character classification */
# define  	_SP 		1				/* Space 					*/
# define 	_VF		2				/* First letter of a variable 			*/
# define 	_VN		4				/* Next (from second) letter of a variable 	*/
# define 	_VA		( _VF | _VN )			/* Variable letter 				*/

/* Debug macros */
# define 	MINIFY_PHP_DEBUG 		0

# if  	MINIFY_PHP_DEBUG
# 	define 		DBGLINE()		php_printf ( "Current line = %d\n", current_line ) 
# 	define 		DBGCASE(name)		php_printf ( "Case %s, line %d\n", name, current_line ) 
# else
#	define 		DBGLINE()
#	define 		DBGCASE(name)
# endif

/* Macros for failures on scanning here documents */
# define 	NOWHERE_DECLARE()				char * __nowhere_error 	=  "" ;
# define 	NOWHERE_FAILED(str)				{ __nowhere_error = str ; goto NOWHEREDOC ; }
# define 	NOWHERE_ERROR 					__nowhere_error 

/* Customized character classification table */
static char 	internal_minify_php_classification [ 256 ] 	=
   {
	  0,   0,   0,   0,   0,   0,   0,   0,   0, _SP, _SP,   0,   0, _SP,   0,   0,		/* 0x00 - 0x0F */
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,		/* 0x10 - 0x1F */
	_SP,   0,   0,   0, _VA,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,		/* 0x20 - 0x2F */
	_VN, _VN, _VN, _VN, _VN, _VN, _VN, _VN, _VN, _VN,   0,   0,   0,   0,   0,   0,		/* 0x30 - 0x3F */
	  0, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA,		/* 0x40 - 0x4F */
	_VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA,   0,   0,   0,   0, _VA,		/* 0x50 - 0x5F */
	  0, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA,		/* 0x60 - 0x6F */
	_VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA, _VA,   0,   0,   0,   0,   0,		/* 0x70 - 0x7F */
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,		/* 0x80 - 0x8F */
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,		/* 0x90 - 0x9F */
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,		/* 0xA0 - 0xAF */
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,		/* 0xB0 - 0xBF */
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,		/* 0xC0 - 0xCF */
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,		/* 0xD0 - 0xDF */
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,		/* 0xE0 - 0xEF */
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0		/* 0xF0 - 0xFF */
    } ;
    
THRAK_API zend_bool 	internal_minify_php ( char *		filename,
					      char * 		input_content,
				              int 		input_content_length,
				              zend_bool 	preserve_newlines,
				              char **		output_content,
				              int *		output_content_length
				              TSRMLS_DC )
   {
	char *			buffer ; 					/* Max size, will be resized before leaving 			*/
	register char *		p,						/* Pointer to input buffer 					*/
		      *		p_end,						/* Pointer to the 1st char after the end of input buffer	*/
		      *		q ;						/* Pointer to output buffer 					*/
	int  			buffer_size ;					/* Actual output buffer size 					*/
	int 			current_line 		=  1,			/* Current line 						*/
				comment_line ;
	int 			last_token_was_name	=  0 ;			/* True when last token was a name or a variable name 		*/
	register char 		ch ;
	char  			delim ;
	NOWHERE_DECLARE ( ) ;

	
	/* The right pointers to the right buffers */
	buffer 	=  input_content ;
	p 	=  buffer ;
	p_end 	=  buffer + input_content_length ;
	q 	=  buffer ;
		
	/* Main loop - We don't use ctype functions here because they cost la peau des couilles */
	while  ( p  <  p_end )
	   {
		DBGLINE ( ) ;
		
		/* Constructs such as : name$name2$name3 can cause buffer overflows because '$' signals the start of a new variable, which in 	*/
		/* this case follows a token and therefore needs a space to be inserted between both. 						*/
		if  ( ( int ) ( q - buffer )  >  input_content_length ) 
		   {
			php_error_docref ( NULL TSRMLS_CC, E_ERROR, "Unexpected buffer overflow with file %s",  filename ) ;
			return ( 0 ) ;
		    }
		    
		ch 			= * p ;
		
		/* Ignore spaces */
		if  ( internal_minify_php_classification [ ch ]  &  _SP ) 
		   {
			DBGCASE ( "Space" ) ;
			
			do
			   {
				if  ( * p  ==  '\n' )
				   {
					if  (  preserve_newlines )		// Still count newlines
						* q ++ 	=  '\n' ;
						
					current_line ++ ;
				    }
				    
				p ++ ;
			    }
			while ( p  <  p_end  &&  ( internal_minify_php_classification [ * p ]  &  _SP ) ) ;
		    }
		/* Variable or token name */
		else if  ( internal_minify_php_classification [ ch ]  &  _VA )
		   {
			DBGCASE ( "Token" ) ;
			
			/* Add a space if last token was a name */
			if  ( last_token_was_name )
				* q ++ = ' ' ;
				
			last_token_was_name	=  1 ;
			* q ++ 			=  ch ;
			p ++ ;
			
			/* Collect additional token/variable characters */
			while  ( p  <  p_end  &&  ( internal_minify_php_classification [ * p ]  &  _VN ) )
				* q ++ 	=  * p ++ ;
		    }
		/* Other stuff, such as operators, numbers, etc. */
		else if  ( ch  !=  '/'  &&  ch  !=  '"'  &&  ch  !=  '\''  &&  ch  !=  '<' )
		   {
			DBGCASE ( "Special char" ) ;
			last_token_was_name  	=  0 ;
			* q ++ 			=  ch ;
			p ++ ;
		    }
		/* Comment */
		else if  ( ch  ==  '/' )
		   {
			DBGCASE ( "Comment" ) ;
			p ++ ;
			
			if  ( p  <  p_end )
			   {
				/* C-style comment */
				if  ( * p  ==  '*' )
				   {
					comment_line 	=  current_line ;
					p ++ ;
					
					while  ( p + 1  <  p_end  &&  ( * p  !=  '*'  ||  * ( p + 1 )  !=  '/' ) )
					   {
						if  ( * p  ==  '\n' )
						   {
							if  ( preserve_newlines )
								* q ++ 	=  '\n' ;
								
							current_line ++ ;
						    }
							
						p ++ ;
					    }
						
					p  +=  2 ;

					if  ( p  >=  p_end )
					   {
						php_error_docref ( NULL TSRMLS_CC, E_ERROR, "EOF inside a comment at line %d of file %s", 
									comment_line, filename ) ;
						return ( 0 ) ;
					    }
				    }
				/* C++-style comment */
				else if  ( * p  ==  '/' )
				   {
					p ++ ;
					
					while  ( p  <  p_end  &&  * p  !=  '\n' )
						p ++ ;
				    }
				/* No comment at all - simply write back the '/' character */
				else
				   {
					* q ++ 	=  '/' ;
					last_token_was_name 	=  0 ;
				    }
			    }
			/* Super special case where '/' is the last character of the file... */
			else
			   {
				* q ++ 			=  '/' ;
				last_token_was_name 	=  0 ;
			    }
		    }
		/* Quoted string */
		else if  ( ch  ==  '"'  ||  ch  ==  '\'' )
		   {
			DBGCASE ( "String" ) ;
			
			p ++ ;
			last_token_was_name 	=  0 ;	
			comment_line 		=  current_line ;
			delim	=  * q ++ 	=  ch ;
			
			while  ( p  <  p_end  &&  * p  !=  delim )
			   {
				if  ( * p  ==  '\n' )
				   {
					if (  preserve_newlines )
						* q ++ 	=  '\n' ;
						
					current_line ++ ;
				    }
					
				if  ( * p  ==  '\\' )
					* q ++ = * p ++  ;
					
				if  ( p  ==  p_end )
					break ;
					
				* q ++  =  * p ++ ;
			    }
			    
			if  ( p  ==  p_end )
			   {
				php_error_docref ( NULL TSRMLS_CC, E_ERROR, "EOF inside a string at line %d of file %s", 
							comment_line, filename ) ;
				return ( 0 ) ;
			    }
			    
			* q ++ 	=  * p ++ ;
		    }
		/* Check for HERE documents */
		else if  ( ch  ==  '<' )
		   {
			/*** 
				We need at least 4 remaining characters :
				- Two '<' signs
				- One character for the heredoc/nowdoc delimiting identifier
				- A newline
				Well, we could need much more than that...
			 ***/
			if  ( p + 3   <   p_end  &&  * ( p + 1 )  ==  '<'  &&  * ( p + 2 )  ==  '<' )
			   {
				char *		forward_p 	=  p + 3 ;
				char * 		here_start ;
				char * 		here_stop ;
				char 		terminator 	=  0 ;
				
				DBGCASE ( "HEREDOC" ) ;
				comment_line 	=  current_line ;

				/* Skip spaces after the '<<<' construct */
				while  ( forward_p  <  p_end  &&  ( internal_minify_php_classification [ * forward_p ]  &  _SP ) )
					forward_p ++ ;
					
				if  ( forward_p  ==  p_end )
					NOWHERE_FAILED ( "EOF after the '<<<' construct" ) ;
					
				/* The heredoc may be surrounded by doublequotes (HEREDOC) or single quotes (NOWDOC) */
				if  ( * forward_p  ==  '"'  ||  * forward_p  ==  '\'' )
				   {
					terminator 	=  * forward_p ++ ;
					
					if  ( forward_p  ==  p_end )
						NOWHERE_FAILED ( "EOF after opening quote" ) ;
				    }
				    
				/* Now whe have to scan for the HEREDOC identifier, which must be a valid PHP token */
				if  ( internal_minify_php_classification [ * forward_p ]  &  _VF )
				   {
					char * 		here_string ;
					char *		end_string ;
					int  		size ;
					
					here_start	=  forward_p ++ ;
					
					while  ( forward_p  <  p_end  &&  internal_minify_php_classification [ * forward_p ]  & _VA )
						forward_p ++ ;
					
					if  ( forward_p  ==  p_end )
						NOWHERE_FAILED ( "EOF after HEREDOC identifier" ) ;
						
					here_stop 	=  forward_p ;
					
					/* We may have a trailing quote/double quote to process */
					if  ( terminator )
					   {
						/* Bad luck : HEREDOC identifier is at end of file or does not end with the starting delimiter... */
						if  ( forward_p  ==  p_end  ||  * forward_p  !=  terminator )
							NOWHERE_FAILED ( "no closing quote" ) ;
							
						forward_p ++ ;
					    }
					    
					/* Now we should have a newline after the HEREDOC identifier */
					if (  forward_p  <  p_end  &&  * forward_p  ==  '\r' )		/* Don't forget Dos-style line endings : ignore them */
						forward_p ++ ;
						
					if  ( forward_p  ==  p_end  ||  * forward_p  !=  '\n' )
						NOWHERE_FAILED ( "EOF after HEREDOC identifier" ) ;
					
					forward_p ++ ;
					
					if  ( forward_p  ==  p_end )
						NOWHERE_FAILED ( "EOF after HEREDOC start newline" ) ;
						
					/* Allocate space for the heredoc identifier ; we will search for identifier followed by a semicolon */
					size 		=  ( int ) ( here_stop - here_start ) ;
					here_string	=  emalloc ( size + 3 ) ;
					
					if  ( here_string  ==  NULL )
						NOWHERE_FAILED ( "memory allocation failed" ) ;
						
					memcpy ( here_string, here_start, size ) ;
					here_string [ size ]		=  ';' ;
					here_string [ size + 1 ]	=  0 ;
					
					/* Ok, let's search for the HEREDOC ending string */
					end_string 	=  strstr ( forward_p, here_string ) ;
					
					if  ( end_string  ==  NULL )
					   {
						efree ( here_string ) ;
						NOWHERE_FAILED ( "ending identifier not found" ) ;
					    }
					    
					/* Point to the character after the ending identifier */
					forward_p 	=  end_string + size + 1 ;
					
					/* Ignore Dos-style line endings */
					if  ( forward_p  <  p_end  &&  * forward_p  ==  '\r' )
						forward_p ++ ;
						
					/* We should have a newline after the ending identifier */
					if  ( forward_p  ==  p_end  ||  * forward_p  !=  '\n' )
						NOWHERE_FAILED ( "missing newline after ending HEREDOC identifier" ) ;
						
					current_line ++ ;
						
					/* End identifier found ; point to the character after it */
					size 		 =  size + 1 + ( int ) ( end_string - p ) ;
					
					/* And copy the whole stuff, from the initial '<<<' construct to the end of the ending identifier */
					while  ( size -- )
					   {
						switch  ( * p )
						   {
							case 	'\r' :  break ;
							case 	'\n' :  current_line ++ ;
							default :
								* q ++ 	=  * p ;
						    }
						    
						p ++ ;
					    }
					
					/* Don't forget the trailing newline after the HEREDOC ending identifier */
					if  ( * p  ==  '\r' )
						p ++ ;
						
					* q ++	=  '\n' ;
					p ++ ;
					
					efree ( here_string ) ;
				    }
				else
					NOWHERE_FAILED ( "invalid HEREDOC identifier character" )  ;
			    }
			else
			   {
				DBGCASE ( "Special char 2" ) ;
				* q ++ 	=  * p ++ ;
			    }
		    }
	    }

	/* All done, return success */
	buffer_size 		=  ( int ) ( q - buffer ) ;
	* output_content 	=  buffer ;
	* output_content_length =  buffer_size ;
	
	return ( 1 ) ;

	/* All "here document" parsing errors arrive here */
NOWHEREDOC :
	php_error_docref ( NULL TSRMLS_CC, E_ERROR, "Bad HEREDOC construct (reason : %s), at line %d of file %s", 
				NOWHERE_ERROR, comment_line, filename ) ;
	return ( 0 ) ;
    }
    
    
/*===========================================================================================================

    NAME
	minify_php - Minifies a PHP file.
	
    PROTOTYPE 
	$string 	=  minify_php 		( $file [, $preserve_newlines = false ] ) ;
	$string 	=  minify_php_stream 	( $file [, $preserve_newlines = false 
							[, $use_include_path = false ] ] ) ;
	$string 	=  minify_php_string    ( $string [, $preserve_newlines = false ] ) ;
	
    DESCRIPTION
	Returns the minified contents of the specified PHP source file.
	minify_php_stream() behaves as file_get_contents() for opening files (thus, an http url could be 
	provided). There is a really low overhead compared to minify_php (around 2% on Windows).
	
    PARAMETERS 
	$file (string) -
		PHP source file to be minified.
		
	$preserve_newlines (boolean) -
		When true, line endings are preserved except for empty lines. When false, file contents are
		returned as a single line. 
		This is useful for debugging because errors will reflect the line in the original source
		file.
	
	$use_include_path (boolean) -
		See file_get_contents().
	
    RETURNS
	The minified contents of the specified PHP source file, or false if an error occurred.
	
 *===========================================================================================================*/
PHP_FUNCTION ( minify_php_string )
   {
	char *			input_content ;						/* String contents & length			*/
	int 			input_content_length ;
	char *			minified_content ;					/* Output from internal_minify_php :		*/
	int 			minified_content_length ;				/* Contents and contents length 		*/
	zend_bool 		preserve_newlines 		=  0 ;			/* Whether to preserve newlines or not 		*/
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "s|b", & input_content, & input_content_length, & preserve_newlines )  ==  FAILURE )
		return ;

	/* Duplicate string */
	input_content 	=  estrndup ( input_content, input_content_length ) ;
	
	if  ( input_content  ==  NULL )
		RETURN_FALSE ;
		
	/* Abort if minifying error */
	if  ( internal_minify_php ( "(string)", input_content, input_content_length, preserve_newlines, & minified_content, & minified_content_length TSRMLS_CC ) )
	   {
		if  ( minified_content_length  <  input_content_length )
		   {
			minified_content 	=  erealloc ( minified_content, minified_content_length ) ;
			
			if  ( minified_content  ==  NULL )
				RETURN_FALSE ;
		    }
		    
		RETVAL_STRINGL ( minified_content, minified_content_length, 0 ) ;
	    }
	else
	   {
		efree ( input_content ) ;
		RETVAL_FALSE ;
	    }
    }

    
PHP_FUNCTION ( minify_php )
   {
	char *			filename ;						/* Input filename 				*/
	int  			filename_length ;					/* Input filename length 			*/
	zend_bool 		preserve_newlines 		=  0 ;			/* Whether to preserve newlines or not 		*/
	char *			minified_content ;					/* Output from internal_minify_php :		*/
	int 			minified_content_length ;				/* Contents and contents length 		*/
	FILE *			fp ;							/* File pointer for input filename 		*/
	struct stat 		stat ;							/* Stat struct to retrieve its size 		*/
	char *			input_content ;						/* File contents & length			*/
	int 			input_content_length ;
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "s|b", & filename, & filename_length, & preserve_newlines )  ==  FAILURE )
		return ;

	/* Check that the supplied filename is not empty */
 	if  ( filename_length  ==  0 ) 
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Empty string specified as filename" ) ;
		RETURN_FALSE ;
	    }
	    
	/* Open the file */
	if  ( ( fp = fopen ( filename, "r" ) )  ==  NULL )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Can't open file %s for reading", filename ) ;
		RETURN_FALSE ;
	    }
	    
	/* Get file size */
	if  ( fstat ( fileno ( fp ), & stat )  ==  -1 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Can't stat() file %s ", filename ) ;
		fclose ( fp ) ;
		RETURN_FALSE ;
	    }
	
	/* Allocate enough memory to read the file */
	input_content_length	=  stat. st_size ;
	input_content 		=  emalloc ( input_content_length + 128 ) ;
	
	if  ( input_content  ==  NULL )
	   {
		fclose ( fp ) ;
		RETURN_FALSE ;
	    }
	
	/* Read file contents */
	if  (  ! fread ( input_content, input_content_length, 1, fp ) )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Failed to read contents of file %s ", filename ) ;
		fclose ( fp ) ;
		RETURN_FALSE ;
	    }
	
	/* All done, close the input file */
	fclose ( fp ) ;
	
	/* Abort if minifying error */
	if  ( internal_minify_php ( filename, input_content, input_content_length, preserve_newlines, & minified_content, & minified_content_length TSRMLS_CC ) )
	   {
		if  ( minified_content_length  <  input_content_length )
		   {
			minified_content 	=  erealloc ( minified_content, minified_content_length ) ;
			
			if  ( minified_content  ==  NULL )
				RETURN_FALSE ;
		    }
		    
		RETVAL_STRINGL ( minified_content, minified_content_length, 0 ) ;
	    }
	else
		RETVAL_FALSE ;
    }
    
    
PHP_FUNCTION ( minify_php_stream )
   {
	char *			filename ;						/* Input filename 				*/
	int  			filename_length ;					/* Input filename length 			*/
	zend_bool 		preserve_newlines 		=  1;			/* Whether to preserve newlines or not 		*/
	zend_bool 		use_include_path 		=  0 ;			/* Whether to use include path or not 		*/
	
	char 			resolved_path [ MAXPATHLEN + 1 ] ;			/* Expanded path 				*/
	php_stream *		stream ;						/* Input file stream 				*/
	char *			input_content ;						/* Input contents 				*/
	int 			input_content_length ;					/* Input contents length 			*/
	char *			minified_content ;					/* Output from internal_minify_php :		*/
	int 			minified_content_length ;				/* contents and contents length 		*/
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "s|bb", & filename, & filename_length, & preserve_newlines, & use_include_path )  ==  FAILURE )
		return ;

	/* Check that the supplied filename is not empty */
 	if  ( filename_length  ==  0 ) 
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Empty string specified as filename" ) ;
		RETURN_FALSE ;
	    }
	    
	/* Expand it ; update filename length if necessary */
 	if  ( ! expand_filepath ( filename, resolved_path TSRMLS_CC ) ) 
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Path expansion failed for file %s", filename ) ;
		RETURN_FALSE ;
	    }
	else
		filename_length 	=  ( long ) strlen ( resolved_path ) ;
		
	/* Open the file */
	stream 		=  php_stream_open_wrapper_ex ( resolved_path, "rb",
					( ( use_include_path ) ?  USE_PATH : 0 ) | REPORT_ERRORS,
					NULL, NULL ) ;
					
	if  ( ! stream ) 	/* Well, not really opened... */
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Unable to open file %s", filename ) ;
		RETURN_FALSE ;
	    }

	/* Read file data */
	if  ( ( input_content_length = ( long ) php_stream_copy_to_mem ( stream, & input_content, PHP_STREAM_COPY_ALL, 0 ) )  <  0 ) 
		RETURN_FALSE ;
		
	/* Data have been read, close the stream here since we are subject to encounter potential errors after that */
	php_stream_close ( stream ) ;
	
	/* Not very interesting on 32-bits flavors, but I got it from file_get_contents() */
	if  ( input_content_length  >  INT_MAX ) 
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "content truncated from %ld to %d bytes", input_content_length, INT_MAX ) ;
		input_content_length 	=  INT_MAX ;
	    }
	    
	/* Empty input file */
	if  ( ! input_content_length )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "empty file" ) ;
		
		if  ( input_content )
			efree ( input_content ) ;
			
		RETURN_FALSE ;
	    }
	
	/* Abort if minifying error */
	/* Abort if minifying error */
	if  ( internal_minify_php ( filename, input_content, input_content_length, preserve_newlines, & minified_content, & minified_content_length TSRMLS_CC ) )
	   {
		if  ( minified_content_length  <  input_content_length )
		   {
			minified_content 	=  erealloc ( minified_content, minified_content_length ) ;
			
			if  ( minified_content  ==  NULL )
				RETURN_FALSE ;
		    }
		    
		RETVAL_STRINGL ( minified_content, minified_content_length, 0 ) ;
	    }
	else
		RETVAL_FALSE ;
    }    