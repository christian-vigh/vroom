/************************************************************************************************************

    NAME
	data.c
	
    DESCRIPTION
	Data-related operations.
	
    AUTHOR
	Christian Vigh, 11/2014.
	
    HISTORY 
	tbd

 ************************************************************************************************************/
# include 	"php_vroom.h"
# include 	"data.h"


/*-----------------------------------------------------------------------------------------------------------

	Packing/unpacking function tables.
	
 *-----------------------------------------------------------------------------------------------------------*/
# define 	WORD_PACKER 			0		/* Packer / unpacker index in function table */
# define 	DWORD_PACKER 			1 
# define 	QWORD_PACKER			2

# define 	WORD_UNPACKER 			WORD_PACKER
# define 	DWORD_UNPACKER 			DWORD_PACKER
# define 	QWORD_UNPACKER			QWORD_PACKER

# define 	LITTLE_ENDIAN_PACKER		0		/* Endianness index in function table */
# define 	BIG_ENDIAN_PACKER 		1
# define 	MACHINE_ENDIAN_PACKER		2

# define 	LITTLE_ENDIAN_UNPACKER		LITTLE_ENDIAN_PACKER
# define 	BIG_ENDIAN_UNPACKER 		BIG_ENDIAN_PACKER
# define 	MACHINE_ENDIAN_UNPACKER		MACHINE_ENDIAN_PACKER


/* For double to long conversion */
typedef struct boxed_double
   {
	union 
	   {
		double 		double_value ;
		long long 	long_value ;
	    } ;
    }  boxed_double ;

/* Packing/unpacking function prototypes */
typedef 	zend_bool  ( * packer_function ) 	( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC ) ;
typedef 	zend_bool  ( * unpacker_function )	( zval *  binary_string, int  count, zval **  result TSRMLS_DC ) ;

/* Packing function table */
static 		packer_function			packer_table [3] [3]	=
   {
	{ internal_data_pack_dwl, internal_data_pack_dwb, NULL },
	{ internal_data_pack_ddl, internal_data_pack_ddb, NULL },
	{ internal_data_pack_dql, internal_data_pack_dqb, NULL }
    } ;

/* Unpacking function table */
static 		unpacker_function		unpacker_table [3] [3]	=
   {
	{ internal_data_unpack_dwl, internal_data_unpack_dwb, NULL },
	{ internal_data_unpack_ddl, internal_data_unpack_ddb, NULL },
	{ internal_data_unpack_dql, internal_data_unpack_dqb, NULL }
    } ;


/*===========================================================================================================

    NAME
	data_initialize - Initializes the data functions.
		
 *===========================================================================================================*/
void 	data_initialize ( THRAK_INIT_FUNC_ARGS )
   {
	/* Register constants for ZVAL types */
	REGISTER_LONG_CONSTANT ( "DATA_LITTLE_ENDIAN"	, LITTLE_ENDIAN		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "DATA_BIG_ENDIAN"	, BIG_ENDIAN		, CONST_CS | CONST_PERSISTENT ) ;
	REGISTER_LONG_CONSTANT ( "DATA_MACHINE_ENDIAN"	, MACHINE_ENDIAN	, CONST_CS | CONST_PERSISTENT ) ;
	
	/* Initialize the "machine-endianness-dependent" packer/unpacker entries */
	packer_table [ WORD_PACKER  ] [ MACHINE_ENDIAN_PACKER ] 	= packer_table [ WORD_PACKER  ] [ thrak_machine_endianness ] ;
	packer_table [ DWORD_PACKER ] [ MACHINE_ENDIAN_PACKER ] 	= packer_table [ DWORD_PACKER ] [ thrak_machine_endianness ] ;
	packer_table [ QWORD_PACKER ] [ MACHINE_ENDIAN_PACKER ] 	= packer_table [ DWORD_PACKER ] [ thrak_machine_endianness ] ;
	
	unpacker_table [ WORD_UNPACKER  ] [ MACHINE_ENDIAN_UNPACKER ] 	= unpacker_table [ WORD_UNPACKER  ] [ thrak_machine_endianness ] ;
	unpacker_table [ DWORD_UNPACKER ] [ MACHINE_ENDIAN_UNPACKER ] 	= unpacker_table [ DWORD_UNPACKER ] [ thrak_machine_endianness ] ;
	unpacker_table [ QWORD_UNPACKER ] [ MACHINE_ENDIAN_UNPACKER ] 	= unpacker_table [ DWORD_UNPACKER ] [ thrak_machine_endianness ] ;
    }


/*===========================================================================================================

    NAME
	data_shutdown
	
    DESCRIPTION
	Sub-extension shutdown function.
	
 *===========================================================================================================*/
void 	data_shutdown ( THRAK_SHUTDOWN_FUNC_ARGS )
   {
    }

  
/*===========================================================================================================

	Packing functions.
	
 *===========================================================================================================*/
 
/***
	WORD functions 
 ***/
THRAK_API zend_bool  internal_data_pack_dwm ( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC )
   { return ( packer_table [ WORD_PACKER ] [ MACHINE_ENDIAN_PACKER ] ( values, count, result, result_length TSRMLS_CC ) ) ; }

   
THRAK_API zend_bool  internal_data_pack_dwl ( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC )
   {
	HashTable *		array_hash 	=  Z_ARRVAL_P ( values ) ;
	HashPosition 		pointer ;
	zval **			item ;
	int 			real_count 	=  ( count  > 0 ) ?  min  ( count, zend_hash_num_elements ( array_hash ) ) : zend_hash_num_elements ( array_hash ) ;
	int 			real_size 	=  real_count * 2 ;
	char *			bindata 	=  emalloc ( real_size ) ;
	char *			p ;
	long long		value ;
	int 			index 		=  0 ;
	boxed_double		box ;
	
	
	if  ( bindata  ==  NULL )
		return ( 0 ) ;
		
	p 	=  bindata ;

	for  ( zend_hash_internal_pointer_reset_ex ( array_hash, & pointer) ; 
			index  <  real_count  &&  zend_hash_get_current_data_ex ( array_hash, ( void ** ) & item, & pointer )  ==  SUCCESS ; 
			zend_hash_move_forward_ex ( array_hash, & pointer ) ) 
	   {
		switch ( Z_TYPE_PP ( item ) )
		   {
			case 	IS_LONG :
			case 	IS_BOOL :
				value 	=  Z_LVAL_PP ( item ) ;
				break ;
				
			case 	IS_DOUBLE :
				box. double_value 	=  Z_DVAL_PP ( item ) ;
				value 			=  box. long_value ;
				break ;
				
			default :
				php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Non-numeric value found at index %d", index ) ;
				efree ( bindata ) ;
				return ( 0 ) ;
		    }
		    
		index ++ ;
		
		* ( p ++ )	=  value  &  0xFF ;
		* ( p ++ )	=  ( value  >>  8 )  &  0xFF ;
	    }	

	* result	=  bindata ;
	* result_length =  real_size ;
	
	return ( 1 ) ;
    }

    
THRAK_API zend_bool  internal_data_pack_dwb ( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC )
   {
	HashTable *		array_hash 	=  Z_ARRVAL_P ( values ) ;
	HashPosition 		pointer ;
	zval **			item ;
	int 			real_count 	=  ( count  > 0 ) ?  min  ( count, zend_hash_num_elements ( array_hash ) ) : zend_hash_num_elements ( array_hash ) ;
	int 			real_size 	=  real_count * 2 ;
	char *			bindata 	=  emalloc ( real_size ) ;
	char *			p ;
	long long		value ;
	int 			index 		=  0 ;
	boxed_double		box ;
	
	
	if  ( bindata  ==  NULL )
		return ( 0 ) ;
		
	p 	=  bindata ;

	for  ( zend_hash_internal_pointer_reset_ex ( array_hash, & pointer) ; 
			index  <  real_count  &&  zend_hash_get_current_data_ex ( array_hash, ( void ** ) & item, & pointer )  ==  SUCCESS ; 
			zend_hash_move_forward_ex ( array_hash, & pointer ) ) 
	   {
		switch ( Z_TYPE_PP ( item ) )
		   {
			case 	IS_LONG :
			case 	IS_BOOL :
				value 	=  Z_LVAL_PP ( item ) ;
				break ;
				
			case 	IS_DOUBLE :
				box. double_value 	=  Z_DVAL_PP ( item ) ;
				value 			=  box. long_value ;
				break ;
				
			default :
				php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Non-numeric value found at index %d", index ) ;
				efree ( bindata ) ;
				return ( 0 ) ;
		    }
		    
		index ++ ;
		
		* ( p ++ )	=  ( value  >>  8 )  &  0xFF ;
		* ( p ++ )	=  value  &  0xFF ;
	    }	

	* result	=  bindata ;
	* result_length =  real_size ;
	
	return ( 1 ) ;
    }


/***
	DWORD functions 
 ***/
THRAK_API zend_bool  internal_data_pack_ddm ( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC )
   { return ( packer_table [ DWORD_PACKER ] [ MACHINE_ENDIAN_PACKER ] ( values, count, result, result_length TSRMLS_CC ) ) ; }
 
 
THRAK_API zend_bool  internal_data_pack_ddl ( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC )
   {
	HashTable *		array_hash 	=  Z_ARRVAL_P ( values ) ;
	HashPosition 		pointer ;
	zval **			item ;
	int 			real_count 	=  ( count  > 0 ) ?  min  ( count, zend_hash_num_elements ( array_hash ) ) : zend_hash_num_elements ( array_hash ) ;
	int 			real_size 	=  real_count * 4 ;
	char *			bindata 	=  emalloc ( real_size ) ;
	char *			p ;
	long long		value ;
	int 			index 		=  0 ;
	boxed_double		box ;
	
	
	if  ( bindata  ==  NULL )
		return ( 0 ) ;
		
	p 	=  bindata ;

	for  ( zend_hash_internal_pointer_reset_ex ( array_hash, & pointer) ; 
			index  <  real_count  &&  zend_hash_get_current_data_ex ( array_hash, ( void ** ) & item, & pointer )  ==  SUCCESS ; 
			zend_hash_move_forward_ex ( array_hash, & pointer ) ) 
	   {
		switch ( Z_TYPE_PP ( item ) )
		   {
			case 	IS_LONG :
			case 	IS_BOOL :
				value 	=  Z_LVAL_PP ( item ) ;
				break ;
				
			case 	IS_DOUBLE :
				box. double_value 	=  Z_DVAL_PP ( item ) ;
				value 			=  box. long_value ;
				break ;
				
			default :
				php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Non-numeric value found at index %d", index ) ;
				efree ( bindata ) ;
				return ( 0 ) ;
		    }
		    
		index ++ ;
		
		* ( p ++ )	=  value  &  0xFF ;
		* ( p ++ )	=  ( value  >>   8 )  &  0xFF ;
		* ( p ++ ) 	=  ( value  >>  16 )  &  0xFF ;
		* ( p ++ ) 	=  ( value  >>  24 )  &  0xFF ;
	    }	

	* result	=  bindata ;
	* result_length =  real_size ;
	
	return ( 1 ) ;
    }

    
THRAK_API zend_bool  internal_data_pack_ddb ( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC )
   {
	HashTable *		array_hash 	=  Z_ARRVAL_P ( values ) ;
	HashPosition 		pointer ;
	zval **			item ;
	int 			real_count 	=  ( count  > 0 ) ?  min  ( count, zend_hash_num_elements ( array_hash ) ) : zend_hash_num_elements ( array_hash ) ;
	int 			real_size 	=  real_count * 4 ;
	char *			bindata 	=  emalloc ( real_size ) ;
	char *			p ;
	long long		value ;
	int 			index 		=  0 ;
	boxed_double		box ;
	
	
	if  ( bindata  ==  NULL )
		return ( 0 ) ;
		
	p 	=  bindata ;

	for  ( zend_hash_internal_pointer_reset_ex ( array_hash, & pointer) ; 
			index  <  real_count  &&  zend_hash_get_current_data_ex ( array_hash, ( void ** ) & item, & pointer )  ==  SUCCESS ; 
			zend_hash_move_forward_ex ( array_hash, & pointer ) ) 
	   {
		switch ( Z_TYPE_PP ( item ) )
		   {
			case 	IS_LONG :
			case 	IS_BOOL :
				value 	=  Z_LVAL_PP ( item ) ;
				break ;
				
			case 	IS_DOUBLE :
				box. double_value 	=  Z_DVAL_PP ( item ) ;
				value 			=  box. long_value ;
				break ;
				
			default :
				php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Non-numeric value found at index %d", index ) ;
				efree ( bindata ) ;
				return ( 0 ) ;
		    }
		    
		index ++ ;

		* ( p ++ )	=  ( value  >>  24 )  &  0xFF ;
		* ( p ++ )	=  ( value  >>  16 )  &  0xFF ;
		* ( p ++ )	=  ( value  >>   8 )  &  0xFF ;
		* ( p ++ )	=  value  &  0xFF ;
	    }	

	* result	=  bindata ;
	* result_length =  real_size ;
	
	return ( 1 ) ;
    }

    
/***
	QWORD functions 
 ***/
THRAK_API zend_bool  internal_data_pack_dqm ( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC )
   { return ( packer_table [ QWORD_PACKER ] [ MACHINE_ENDIAN_PACKER ] ( values, count, result, result_length TSRMLS_CC ) ) ; }

   
THRAK_API zend_bool  internal_data_pack_dql ( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC )
   {
	HashTable *		array_hash 	=  Z_ARRVAL_P ( values ) ;
	HashPosition 		pointer ;
	zval **			item ;
	int 			real_count 	=  ( count  > 0 ) ?  min  ( count, zend_hash_num_elements ( array_hash ) ) : zend_hash_num_elements ( array_hash ) ;
	int 			real_size 	=  real_count * 8 ;
	char *			bindata 	=  emalloc ( real_size ) ;
	char *			p ;
	long long		value ;
	int 			index 		=  0 ;
	boxed_double		box ;
	
	
	if  ( bindata  ==  NULL )
		return ( 0 ) ;
		
	p 	=  bindata ;

	for  ( zend_hash_internal_pointer_reset_ex ( array_hash, & pointer) ; 
			index  <  real_count  &&  zend_hash_get_current_data_ex ( array_hash, ( void ** ) & item, & pointer )  ==  SUCCESS ; 
			zend_hash_move_forward_ex ( array_hash, & pointer ) ) 
	   {
		switch ( Z_TYPE_PP ( item ) )
		   {
			case 	IS_LONG :
			case 	IS_BOOL :
				value 	=  Z_LVAL_PP ( item ) ;
				break ;
				
			case 	IS_DOUBLE :
				box. double_value 	=  Z_DVAL_PP ( item ) ;
				value 			=  box. long_value ;
				break ;
				
			default :
				php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Non-numeric value found at index %d", index ) ;
				efree ( bindata ) ;
				return ( 0 ) ;
		    }
		    
		index ++ ;
		
		* ( p ++ )	=  value  &  0xFF ;
		* ( p ++ )	=  ( value  >>   8 )  &  0xFF ;
		* ( p ++ ) 	=  ( value  >>  16 )  &  0xFF ;
		* ( p ++ ) 	=  ( value  >>  24 )  &  0xFF ;
		* ( p ++ ) 	=  ( value  >>  32 )  &  0xFF ;
		* ( p ++ ) 	=  ( value  >>  40 )  &  0xFF ;
		* ( p ++ ) 	=  ( value  >>  48 )  &  0xFF ;
		* ( p ++ ) 	=  ( value  >>  56 )  &  0xFF ;
	    }	

	* result	=  bindata ;
	* result_length =  real_size ;
	
	return ( 1 ) ;
    }

THRAK_API zend_bool  internal_data_pack_dqb ( zval *  values, int  count, char **  result, int *  result_length TSRMLS_DC )
   {
	HashTable *		array_hash 	=  Z_ARRVAL_P ( values ) ;
	HashPosition 		pointer ;
	zval **			item ;
	int 			real_count 	=  ( count  > 0 ) ?  min  ( count, zend_hash_num_elements ( array_hash ) ) : zend_hash_num_elements ( array_hash ) ;
	int 			real_size 	=  real_count * 8 ;
	char *			bindata 	=  emalloc ( real_size ) ;
	char *			p ;
	long long		value ;
	int 			index 		=  0 ;
	boxed_double		box ;
	
	
	if  ( bindata  ==  NULL )
		return ( 0 ) ;
		
	p 	=  bindata ;

	for  ( zend_hash_internal_pointer_reset_ex ( array_hash, & pointer) ; 
			index  <  real_count  &&  zend_hash_get_current_data_ex ( array_hash, ( void ** ) & item, & pointer )  ==  SUCCESS ; 
			zend_hash_move_forward_ex ( array_hash, & pointer ) ) 
	   {
		switch ( Z_TYPE_PP ( item ) )
		   {
			case 	IS_LONG :
			case 	IS_BOOL :
				value 	=  Z_LVAL_PP ( item ) ;
				break ;
				
			case 	IS_DOUBLE :
				box. double_value 	=  Z_DVAL_PP ( item ) ;
				value 			=  box. long_value ;
				break ;
				
			default :
				php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Non-numeric value found at index %d", index ) ;
				efree ( bindata ) ;
				return ( 0 ) ;
		    }
		    
		index ++ ;

		* ( p ++ )	=  ( value  >>  56 )  &  0xFF ;
		* ( p ++ )	=  ( value  >>  48 )  &  0xFF ;
		* ( p ++ )	=  ( value  >>  40 )  &  0xFF ;
		* ( p ++ )	=  ( value  >>  32 )  &  0xFF ;
		* ( p ++ )	=  ( value  >>  24 )  &  0xFF ;
		* ( p ++ )	=  ( value  >>  16 )  &  0xFF ;
		* ( p ++ )	=  ( value  >>   8 )  &  0xFF ;
		* ( p ++ )	=  value  &  0xFF ;
	    }	

	* result	=  bindata ;
	* result_length =  real_size ;
	
	return ( 1 ) ;
    }



/*===========================================================================================================

	Unpacking functions.
	
 *===========================================================================================================*/
 
/***
	WORD functions 
 ***/
THRAK_API zend_bool  internal_data_unpack_dwm ( zval *  binary_string, int  count, zval **  result TSRMLS_DC )
   { return ( unpacker_table [ WORD_PACKER ] [ MACHINE_ENDIAN_PACKER ] ( binary_string, count, result TSRMLS_CC ) ) ; }


THRAK_API zend_bool  internal_data_unpack_dwl ( zval *  binary_string, int  count, zval **  result TSRMLS_DC )
   {
	int  		real_count 		=  ( count  >  0 ) ?  min ( Z_STRLEN_P ( binary_string ) / 2, count ) : Z_STRLEN_P ( binary_string ) / 2 ;
	char *		p			=  Z_STRVAL_P ( binary_string ) ;
	zval *		array_result ;
	long long 	value ;
	
	
	MAKE_STD_ZVAL ( array_result ) ;
	array_init ( array_result ) ;
	
	while  ( real_count -- )
	   {
		value 	=  ( * p ) |
			   ( ( * ( p + 1 ) ) <<  8 ) ;
			   
		p += 2 ;
			   
		add_next_index_long ( array_result, ( long ) value ) ;
	    }
	    
	* result 	=  array_result ;
	return ( 1 ) ;
    }

    
THRAK_API zend_bool  internal_data_unpack_dwb ( zval *  binary_string, int  count, zval **  result TSRMLS_DC )
   {
	int  		real_count 		=  ( count  >  0 ) ?  min ( Z_STRLEN_P ( binary_string ) / 2, count ) : Z_STRLEN_P ( binary_string ) / 2 ;
	char *		p			=  Z_STRVAL_P ( binary_string ) ;
	zval *		array_result ;
	long long 	value ;
	
	
	MAKE_STD_ZVAL ( array_result ) ;
	array_init ( array_result ) ;
	
	while  ( real_count -- )
	   {
		value 	=  ( ( * p ) <<  8 ) |
			   ( ( * p + 1 ) ) ;
			   
		p += 2 ;
			   
		add_next_index_long ( array_result, ( long ) value ) ;
	    }
	    
	* result 	=  array_result ;
	return ( 1 ) ;
    }

    
/***
	DWORD functions 
 ***/
THRAK_API zend_bool  internal_data_unpack_ddm ( zval *  binary_string, int  count, zval **  result TSRMLS_DC )
   { return ( unpacker_table [ DWORD_PACKER ] [ MACHINE_ENDIAN_PACKER ] ( binary_string, count, result TSRMLS_CC ) ) ; }

   
THRAK_API zend_bool  internal_data_unpack_ddl ( zval *  binary_string, int  count, zval **  result TSRMLS_DC )
   {
	int  		real_count 		=  ( count  >  0 ) ?  min ( Z_STRLEN_P ( binary_string ) / 4, count ) : Z_STRLEN_P ( binary_string ) / 4 ;
	char *		p			=  Z_STRVAL_P ( binary_string ) ;
	zval *		array_result ;
	long long 	value ;
	
	
	MAKE_STD_ZVAL ( array_result ) ;
	array_init ( array_result ) ;
	
	while  ( real_count -- )
	   {
		value 	=  ( * p ) |
			   ( ( * ( p + 1 ) ) <<   8 ) |
			   ( ( * ( p + 2 ) ) <<  16 ) |
			   ( ( * ( p + 3 ) ) <<  24 ) ;
			   
		p += 4 ;

		add_next_index_long ( array_result, ( long ) value ) ;
	    }
	    
	* result 	=  array_result ;
	return ( 1 ) ;
    }
 
 
THRAK_API zend_bool  internal_data_unpack_ddb ( zval *  binary_string, int  count, zval **  result TSRMLS_DC )
   {
	int  		real_count 		=  ( count  >  0 ) ?  min ( Z_STRLEN_P ( binary_string ) / 4, count ) : Z_STRLEN_P ( binary_string ) / 4 ;
	char *		p			=  Z_STRVAL_P ( binary_string ) ;
	zval *		array_result ;
	long long 	value ;
	
	
	MAKE_STD_ZVAL ( array_result ) ;
	array_init ( array_result ) ;
	
	while  ( real_count -- )
	   {
		value 	=  ( ( * ( p + 3 ) )  ) |
			   ( ( * ( p + 2 ) ) <<   8 ) |
			   ( ( * ( p + 1 ) ) <<  16 ) |
			   ( ( * p ) << 24 ) ;
			   
		p += 4 ;
			   
		add_next_index_long ( array_result, ( long ) value ) ;
	    }
	    
	* result 	=  array_result ;
	return ( 1 ) ;
    }
    
    
/***
	QWORD functions 
 ***/
THRAK_API zend_bool  internal_data_unpack_dqm ( zval *  binary_string, int  count, zval **  result TSRMLS_DC )
   { return ( unpacker_table [ QWORD_PACKER ] [ MACHINE_ENDIAN_PACKER ] ( binary_string, count, result TSRMLS_CC ) ) ; }

   
THRAK_API zend_bool  internal_data_unpack_dql ( zval *  binary_string, int  count, zval **  result TSRMLS_DC )
   {
	int  		real_count 		=  ( count  >  0 ) ?  min ( Z_STRLEN_P ( binary_string ) / 8, count ) : Z_STRLEN_P ( binary_string ) / 8 ;
	char *		p			=  Z_STRVAL_P ( binary_string ) ;
	zval *		array_result ;
	long long 	value ;
	
	
	MAKE_STD_ZVAL ( array_result ) ;
	array_init ( array_result ) ;
	
	while  ( real_count -- )
	   {
		value 	=  ( long long ) ( * p ) |
			   ( ( ( long long ) ( * ( p + 1 ) ) ) <<   8 ) |
			   ( ( ( long long ) ( * ( p + 2 ) ) ) <<  16 ) |
			   ( ( ( long long ) ( * ( p + 3 ) ) ) <<  24 ) |
			   ( ( ( long long ) ( * ( p + 4 ) ) ) <<  32 ) |
			   ( ( ( long long ) ( * ( p + 5 ) ) ) <<  40 ) |
			   ( ( ( long long ) ( * ( p + 6 ) ) ) <<  48 ) |
			   ( ( ( long long ) ( * ( p + 7 ) ) ) <<  56 ) ;
			   
		p += 8 ;

		add_next_index_long ( array_result, ( long ) value ) ;
	    }
	    
	* result 	=  array_result ;
	return ( 1 ) ;
    }
    
THRAK_API zend_bool  internal_data_unpack_dqb ( zval *  binary_string, int  count, zval **  result TSRMLS_DC )
   {
	int  		real_count 		=  ( count  >  0 ) ?  min ( Z_STRLEN_P ( binary_string ) / 8, count ) : Z_STRLEN_P ( binary_string ) / 8 ;
	char *		p			=  Z_STRVAL_P ( binary_string ) ;
	zval *		array_result ;
	long long 	value ;
	
	
	MAKE_STD_ZVAL ( array_result ) ;
	array_init ( array_result ) ;
	
	while  ( real_count -- )
	   {
		value 	=  ( ( long long ) ( * ( p + 7 ) )  ) |
			   ( ( long long ) ( * ( p + 6 ) ) <<   8 ) |
			   ( ( long long ) ( * ( p + 5 ) ) <<  16 ) |
			   ( ( long long ) ( * ( p + 4 ) ) <<  24 ) |
			   ( ( long long ) ( * ( p + 3 ) ) <<  32 ) |
			   ( ( long long ) ( * ( p + 2 ) ) <<  40 ) |
			   ( ( long long ) ( * ( p + 1 ) ) <<  48 ) |
			   ( ( long long ) ( * p ) << 56 ) ;
			   
		p += 4 ;
			   
		add_next_index_long ( array_result, ( long ) value ) ;
	    }
	    
	* result 	=  array_result ;
	return ( 1 ) ;
    }
    
    
/*===========================================================================================================

	Packing/unpacking helper functions.
	
 *===========================================================================================================*/

/***
	packer_normalize_input -
		Ensures that the first argument of the xxxpack functions is always an array.
		Converts to an array if a single scalar value (long, double or bool) has been specified.
 ***/
static zend_always_inline zend_bool  packer_normalize_input ( zval **  value TSRMLS_DC )
   {
	int 		status 	=  1 ;
	
	switch  ( Z_TYPE_PP ( value ) )
	   {
		case 	IS_ARRAY :
			 break ;
			
		case 	IS_LONG :
		case 	IS_DOUBLE :
		case 	IS_BOOL :
			convert_to_array_ex ( value ) ;
			break ;
			
		default :
			php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Only an array or a numeric value can be supplied as the first argument of a packing function" ) ;
			status 	=  0 ;
	    }
	    
	return ( status ) ;
    }
 
/***
	packer_check_endianness -
		Check that the supplied endianness constant is correct.
 ***/
static zend_always_inline zend_bool  packer_check_endianness ( int  endianness TSRMLS_DC )
   {
	switch  ( endianness )
	   {
		case 	MACHINE_ENDIAN :
		case 	LITTLE_ENDIAN : 
		case 	BIG_ENDIAN :
			return ( 1 ) ;
			
		default :
			php_error_docref ( NULL TSRMLS_CC, E_WARNING, "The value supplied for the $endianness parameter must be one of the following constants : "
								      "DATA_LITTLE_ENDIAN, DATA_BIG_ENDIAN or DATA_MACHINE_ENDIAN" ) ;
			return ( 0 ) ;
	    }
    }  
 
 
/***
	packer_check_string -
		Checks that the specified zval is a string.
 ***/
static zend_always_inline zend_bool  packer_check_zstring ( zval *  data TSRMLS_DC )
   {
	if  ( Z_TYPE_P ( data )  ==  IS_STRING )
		return ( 1 ) ;
	else
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Data to be unpacked must be a string." ) ;
		
		return ( 0 ) ;
	    }
    }
    
    
/*===========================================================================================================

    NAME
	data_dwpack, ddpack, dqpack - Packs word/dword/qword values.
	
    PROTOTYPE
	$binstr 	=  data_dwpack ( $array, [ count, [ $endianness = DATA_MACHINE_ENDIANNESS, $count ] ] ) ;
	$binstr 	=  data_ddpack ( $array, [ count, [ $endianness = DATA_MACHINE_ENDIANNESS, $count ] ] ) ;
	$binstr 	=  data_dqpack ( $array, [ count, [ $endianness = DATA_MACHINE_ENDIANNESS, $count ] ] ) ;
	
	$binstr 	=  data_dwlpack ( $array, $count ) ;
	$binstr 	=  data_ddlpack ( $array, $count ) ;
	$binstr 	=  data_dqlpack ( $array, $count ) ;
	
	$binstr 	=  data_dwbpack ( $array, $count ) ;
	$binstr 	=  data_ddbpack ( $array, $count ) ;
	$binstr 	=  data_dqbpack ( $array, $count ) ;

	$binstr 	=  data_dwmpack ( $array, $count ) ;
	$binstr 	=  data_ddmpack ( $array, $count ) ;
	$binstr 	=  data_dqmpack ( $array, $count ) ;
	
    DESCRIPTION
	These functions pack word/dword/qwords quantities, respectively.
	The groups of lpack/bpack/mpack functions packs values in little endian, big endian or machine endian
	format.
	
    PARAMETERS
	$array (array or scalar value) -
		Array of values to be packed, or a scalar value if only one value is to be packed.
		In this case, the $count parameter is ignored.
		
	$count (integer) -
		Number of values from $array to be packed. If not specified, all values will be packed.
		
	$endianness (integer) -
		Endianness of the packed values ; either DATA_LITTLE_ENDIAN, DATA_BIG_ENDIAN or 
		DATA_MACHINE_ENDIAN to use the machine endianness.
		
    RETURN VALUE
	A binary string containing the packed values, or false if an error occurred, such as not enough 
	memory.
	
 *===========================================================================================================*/

/***
	WORD functions.
 ***/ 
PHP_FUNCTION ( data_dwpack )
   {
	zval *		array ;
	char *		result ;
	int 		result_length ;
	int 		count 		=  -1 ;
	int  		endianness	=  MACHINE_ENDIAN ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", & array, & count, & endianness )  ==  FAILURE )
		return;
	
	/* Ensure that the endianness has a correct value */
	if  ( ! packer_check_endianness ( endianness TSRMLS_CC ) )
		RETURN_FALSE ;
		
	/* Make sure the 1st argument is always an array */
	packer_normalize_input ( & array TSRMLS_CC ) ;
	
	/* Call the appropriate packing function - return false if out of memory */
	if  ( ! packer_table [ WORD_PACKER ] [ endianness ] ( array, count, & result, & result_length TSRMLS_CC ) )
		RETURN_FALSE ;

	/* Return binary data */
	RETURN_STRINGL ( result, result_length, 0 ) ;
    }
 
 
PHP_FUNCTION ( data_dwlpack )
   {
	zval *		array ;
	char *		result ;
	int 		result_length ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & array, & count )  ==  FAILURE )
		return;
	
	/* Make sure the 1st argument is always an array */
	packer_normalize_input ( & array TSRMLS_CC ) ;
	
	/* Call the appropriate packing function - return false if out of memory */
	if  ( ! packer_table [ WORD_PACKER ] [ LITTLE_ENDIAN_PACKER ] ( array, count, & result, & result_length TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Return binary data */
	RETURN_STRINGL ( result, result_length, 0 ) ;
    }

    
PHP_FUNCTION ( data_dwbpack )
   {
	zval *		array ;
	char *		result ;
	int 		result_length ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & array, & count )  ==  FAILURE )
		return;
	
	/* Make sure the 1st argument is always an array */
	packer_normalize_input ( & array TSRMLS_CC ) ;
	
	/* Call the appropriate packing function - return false if out of memory */
	if  ( ! packer_table [ WORD_PACKER ] [ BIG_ENDIAN_PACKER ] ( array, count, & result, & result_length TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Return binary data */
	RETURN_STRINGL ( result, result_length, 0 ) ;
    }

    
PHP_FUNCTION ( data_dwmpack )
   {
	zval *		array ;
	char *		result ;
	int 		result_length ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & array, & count )  ==  FAILURE )
		return;
	
	/* Make sure the 1st argument is always an array */
	packer_normalize_input ( & array TSRMLS_CC ) ;
	
	/* Call the appropriate packing function - return false if out of memory */
	if  ( ! packer_table [ WORD_PACKER ] [ MACHINE_ENDIAN_PACKER ] ( array, count, & result, & result_length TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Return binary data */
	RETURN_STRINGL ( result, result_length, 0 ) ;
    }

 
/***
	DWORD functions.
 ***/ 
 PHP_FUNCTION ( data_ddpack )
   {
	zval *		array ;
	char *		result ;
	int 		result_length ;
	int 		count 		=  -1 ;
	int  		endianness	=  MACHINE_ENDIAN ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", & array, & count, & endianness )  ==  FAILURE )
		return;
	
	/* Ensure that the endianness has a correct value */
	if  ( ! packer_check_endianness ( endianness TSRMLS_CC ) )
		RETURN_FALSE ;
		
	/* Make sure the 1st argument is always an array */
	packer_normalize_input ( & array TSRMLS_CC ) ;
	
	/* Call the appropriate packing function - return false if out of memory */
	if  ( ! packer_table [ DWORD_PACKER ] [ endianness ] ( array, count, & result, & result_length TSRMLS_CC ) )
		RETURN_FALSE ;

	/* Return binary data */
	RETURN_STRINGL ( result, result_length, 0 ) ;
    }
 
 
PHP_FUNCTION ( data_ddlpack )
   {
	zval *		array ;
	char *		result ;
	int 		result_length ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & array, & count )  ==  FAILURE )
		return;
	
	/* Make sure the 1st argument is always an array */
	packer_normalize_input ( & array TSRMLS_CC ) ;
	
	/* Call the appropriate packing function - return false if out of memory */
	if  ( ! packer_table [ DWORD_PACKER ] [ LITTLE_ENDIAN_PACKER ] ( array, count, & result, & result_length TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Return binary data */
	RETURN_STRINGL ( result, result_length, 0 ) ;
    }

    
PHP_FUNCTION ( data_ddbpack )
   {
	zval *		array ;
	char *		result ;
	int 		result_length ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & array, & count )  ==  FAILURE )
		return;
	
	/* Make sure the 1st argument is always an array */
	packer_normalize_input ( & array TSRMLS_CC ) ;
	
	/* Call the appropriate packing function - return false if out of memory */
	if  ( ! packer_table [ DWORD_PACKER ] [ BIG_ENDIAN_PACKER ] ( array, count, & result, & result_length TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Return binary data */
	RETURN_STRINGL ( result, result_length, 0 ) ;
    }

    
PHP_FUNCTION ( data_ddmpack )
   {
	zval *		array ;
	char *		result ;
	int 		result_length ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & array, & count )  ==  FAILURE )
		return;
	
	/* Make sure the 1st argument is always an array */
	packer_normalize_input ( & array TSRMLS_CC ) ;
	
	/* Call the appropriate packing function - return false if out of memory */
	if  ( ! packer_table [ DWORD_PACKER ] [ MACHINE_ENDIAN_PACKER ] ( array, count, & result, & result_length TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Return binary data */
	RETURN_STRINGL ( result, result_length, 0 ) ;
    }

 
/***
	QWORD functions.
 ***/ 
PHP_FUNCTION ( data_dqpack )
   {
	zval *		array ;
	char *		result ;
	int 		result_length ;
	int 		count 		=  -1 ;
	int  		endianness	=  MACHINE_ENDIAN ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", & array, & count, & endianness )  ==  FAILURE )
		return;
	
	/* Ensure that the endianness has a correct value */
	if  ( ! packer_check_endianness ( endianness TSRMLS_CC ) )
		RETURN_FALSE ;
		
	/* Make sure the 1st argument is always an array */
	packer_normalize_input ( & array TSRMLS_CC ) ;
	
	/* Call the appropriate packing function - return false if out of memory */
	if  ( ! packer_table [ DWORD_PACKER ] [ endianness ] ( array, count, & result, & result_length TSRMLS_CC ) )
		RETURN_FALSE ;

	/* Return binary data */
	RETURN_STRINGL ( result, result_length, 0 ) ;
    }
 
 
PHP_FUNCTION ( data_dqlpack )
   {
	zval *		array ;
	char *		result ;
	int 		result_length ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & array, & count )  ==  FAILURE )
		return;
	
	/* Make sure the 1st argument is always an array */
	packer_normalize_input ( & array TSRMLS_CC ) ;
	
	/* Call the appropriate packing function - return false if out of memory */
	if  ( ! packer_table [ DWORD_PACKER ] [ LITTLE_ENDIAN_PACKER ] ( array, count, & result, & result_length TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Return binary data */
	RETURN_STRINGL ( result, result_length, 0 ) ;
    }

    
PHP_FUNCTION ( data_dqbpack )
   {
	zval *		array ;
	char *		result ;
	int 		result_length ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & array, & count )  ==  FAILURE )
		return;
	
	/* Make sure the 1st argument is always an array */
	packer_normalize_input ( & array TSRMLS_CC ) ;
	
	/* Call the appropriate packing function - return false if out of memory */
	if  ( ! packer_table [ DWORD_PACKER ] [ BIG_ENDIAN_PACKER ] ( array, count, & result, & result_length TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Return binary data */
	RETURN_STRINGL ( result, result_length, 0 ) ;
    }

    
PHP_FUNCTION ( data_dqmpack )
   {
	zval *		array ;
	char *		result ;
	int 		result_length ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & array, & count )  ==  FAILURE )
		return;
	
	/* Make sure the 1st argument is always an array */
	packer_normalize_input ( & array TSRMLS_CC ) ;
	
	/* Call the appropriate packing function - return false if out of memory */
	if  ( ! packer_table [ DWORD_PACKER ] [ MACHINE_ENDIAN_PACKER ] ( array, count, & result, & result_length TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Return binary data */
	RETURN_STRINGL ( result, result_length, 0 ) ;
    }

 
/*===========================================================================================================

    NAME
	data_dwunpack, ddunpack, dqunpack - unpacks word/dword/qword values.
	
    PROTOTYPE
	$binstr 	=  data_dwunpack ( $data, [ count, [ $endianness = DATA_MACHINE_ENDIANNESS, $count ] ] ) ;
	$binstr 	=  data_ddunpack ( $data, [ count, [ $endianness = DATA_MACHINE_ENDIANNESS, $count ] ] ) ;
	$binstr 	=  data_dqunpack ( $data, [ count, [ $endianness = DATA_MACHINE_ENDIANNESS, $count ] ] ) ;
	
	$binstr 	=  data_dwlunpack ( $data, $count ) ;
	$binstr 	=  data_ddlunpack ( $data, $count ) ;
	$binstr 	=  data_dqlunpack ( $data, $count ) ;
	
	$binstr 	=  data_dwbunpack ( $data, $count ) ;
	$binstr 	=  data_ddbunpack ( $data, $count ) ;
	$binstr 	=  data_dqbunpack ( $data, $count ) ;

	$binstr 	=  data_dwmunpack ( $data, $count ) ;
	$binstr 	=  data_ddmunpack ( $data, $count ) ;
	$binstr 	=  data_dqmunpack ( $data, $count ) ;
	
    DESCRIPTION
	These functions unpack word/dword/qwords quantities, respectively.
	The groups of lunpack/bunpack/munpack functions unpacks values in little endian, big endian or machine endian
	format.
	
    PARAMETERS
	$data (binary string) -
		Values to be unpacked, stored as a binary string.
		
	$count (integer) -
		Number of values from $array to be unpacked. If not specified, all values will be unpacked.
		
	$endianness (integer) -
		Endianness of the unpacked values ; either DATA_LITTLE_ENDIAN, DATA_BIG_ENDIAN or 
		DATA_MACHINE_ENDIAN to use the machine endianness.
		
    RETURN VALUE
	An array containing the unpacked words/dwords/qwords.
	
 *===========================================================================================================*/

/***
	WORD functions.
 ***/ 
PHP_FUNCTION ( data_dwunpack )
   {
	zval *		data ;
	zval *		result ;
	int 		count 		=  -1 ;
	int  		endianness	=  MACHINE_ENDIAN ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", & data, & count, & endianness )  ==  FAILURE )
		return;
	
	/* Ensure that first argument is a string */
	if  ( ! packer_check_zstring ( data TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Ensure that the endianness has a correct value */
	if  ( ! packer_check_endianness ( endianness TSRMLS_CC ) )
		RETURN_FALSE ;
		
	/* Call the appropriate unpacking function  */
	if  ( ! unpacker_table [ WORD_UNPACKER ] [ endianness ] ( data, count, & result TSRMLS_CC ) )
		RETURN_FALSE ;

	/* Return binary data */
	RETURN_ZVAL ( result, 0, 0 ) ;
    }

    
PHP_FUNCTION ( data_dwlunpack )
   {
	zval *		data ;
	zval *		result ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & data, & count )  ==  FAILURE )
		return;
	
	/* Ensure that first argument is a string */
	if  ( ! packer_check_zstring ( data TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Call the appropriate unpacking function  */
	if  ( ! unpacker_table [ WORD_UNPACKER ] [ LITTLE_ENDIAN_UNPACKER ] ( data, count, & result TSRMLS_CC ) )
		RETURN_FALSE ;

	/* Return binary data */
	RETURN_ZVAL ( result, 0, 0 ) ;
    }

       
PHP_FUNCTION ( data_dwbunpack )
   {
	zval *		data ;
	zval *		result ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & data, & count )  ==  FAILURE )
		return;
	
	/* Ensure that first argument is a string */
	if  ( ! packer_check_zstring ( data TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Call the appropriate unpacking function  */
	if  ( ! unpacker_table [ WORD_UNPACKER ] [ BIG_ENDIAN_UNPACKER ] ( data, count, & result TSRMLS_CC ) )
		RETURN_FALSE ;

	/* Return binary data */
	RETURN_ZVAL ( result, 0, 0 ) ;
    }

    
PHP_FUNCTION ( data_dwmunpack )
   {
	zval *		data ;
	zval *		result ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & data, & count )  ==  FAILURE )
		return;
	
	/* Ensure that first argument is a string */
	if  ( ! packer_check_zstring ( data TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Call the appropriate unpacking function  */
	if  ( ! unpacker_table [ WORD_UNPACKER ] [ MACHINE_ENDIAN_UNPACKER ] ( data, count, & result TSRMLS_CC ) )
		RETURN_FALSE ;

	/* Return binary data */
	RETURN_ZVAL ( result, 0, 0 ) ;
    }

    
/***
	DWORD functions.
 ***/ 
PHP_FUNCTION ( data_ddunpack )
   {
	zval *		data ;
	zval *		result ;
	int 		count 		=  -1 ;
	int  		endianness	=  MACHINE_ENDIAN ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", & data, & count, & endianness )  ==  FAILURE )
		return;
	
	/* Ensure that first argument is a string */
	if  ( ! packer_check_zstring ( data TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Ensure that the endianness has a correct value */
	if  ( ! packer_check_endianness ( endianness TSRMLS_CC ) )
		RETURN_FALSE ;
		
	/* Call the appropriate unpacking function  */
	if  ( ! unpacker_table [ DWORD_UNPACKER ] [ endianness ] ( data, count, & result TSRMLS_CC ) )
		RETURN_FALSE ;

	/* Return binary data */
	RETURN_ZVAL ( result, 0, 0 ) ;
    }

    
PHP_FUNCTION ( data_ddlunpack )
   {
	zval *		data ;
	zval *		result ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & data, & count )  ==  FAILURE )
		return;
	
	/* Ensure that first argument is a string */
	if  ( ! packer_check_zstring ( data TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Call the appropriate unpacking function  */
	if  ( ! unpacker_table [ DWORD_UNPACKER ] [ LITTLE_ENDIAN_UNPACKER ] ( data, count, & result TSRMLS_CC ) )
		RETURN_FALSE ;

	/* Return binary data */
	RETURN_ZVAL ( result, 0, 0 ) ;
    }

       
PHP_FUNCTION ( data_ddbunpack )
   {
	zval *		data ;
	zval *		result ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & data, & count )  ==  FAILURE )
		return;
	
	/* Ensure that first argument is a string */
	if  ( ! packer_check_zstring ( data TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Call the appropriate unpacking function  */
	if  ( ! unpacker_table [ DWORD_UNPACKER ] [ BIG_ENDIAN_UNPACKER ] ( data, count, & result TSRMLS_CC ) )
		RETURN_FALSE ;

	/* Return binary data */
	RETURN_ZVAL ( result, 0, 0 ) ;
    }

    
PHP_FUNCTION ( data_ddmunpack )
   {
	zval *		data ;
	zval *		result ;
	int 		count 		=  -1 ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "z|l", & data, & count )  ==  FAILURE )
		return;
	
	/* Ensure that first argument is a string */
	if  ( ! packer_check_zstring ( data TSRMLS_CC ) )
		RETURN_FALSE ;
	
	/* Call the appropriate unpacking function  */
	if  ( ! unpacker_table [ DWORD_UNPACKER ] [ MACHINE_ENDIAN_UNPACKER ] ( data, count, & result TSRMLS_CC ) )
		RETURN_FALSE ;

	/* Return binary data */
	RETURN_ZVAL ( result, 0, 0 ) ;
    }

    

/*===========================================================================================================

    NAME
	internal_data_decode_format
	
    DESCRIPTION
	Decodes a data_pack/data_unpack format specified.
	
    RETURN VALUE
	Number of bytes
	
 *===========================================================================================================*/
 
// __get_repeat_count -
//	Parses a repeat count after a format specifier.
//	A repeat count can be :
//	- A positive integer
//	- A '*' sign, meaning "until input data is exhausted"
//	- Or nothing, which is equivalent to '1'
static zend_always_inline int 	__get_repeat_count ( char **  p, int *  pc )
   {
	int  		value ;
	char *		q 		=  * p ;
	
	if  ( * q  ==  '*' )
	   {
		q ++ ;
		* p 	=  q ;
		* pc  -- ;
		
		return ( - 1 ) ;
	    }
	else if  ( isdigit ( * q ) )
	   {
		value 		=  0 ;
		
		while  ( * pc  &&  isdigit ( * q ) )
		   {
			value 	=  ( value * 10 ) + ( * q - '0' ) ;
			q ++ ;
			* pc -- ;
		    }
		    
		* p 	=  q ;
		
		return ( value ) ;
	    }
	else
		return ( 1 ) ;
    }

// __get_remaining_size :
//	Given a repeat count, data size and current zval argument, returns the actual number of bytes
//	that can be retrieved.
//	Returns -1 if 
//static zend_always_inline int  __get_remaining_size ( 
// array_flatten ( zval * ) ;

THRAK_API int  internal_data_decode_format ( const char * 		format,
				   int 				format_length,
				   int 				argc,
				   zval **			argv,
				   packing_data_format **	output_format_specifiers,
				   int *			output_format_count 
				   TSRMLS_DC ) 
   {
	packing_data_format *		format_specifiers ;					/* Format specifiers and format specifier count 						*/
	int  				format_specifier_count 		=  0 ;
	int  				current_argc 			=  0 ;			/* Current input argument 									*/
	int  				byte_count 			=  0 ;			/* Number of bytes needed for packing 								*/
	packing_data_format *		fsp ;							/* Pointer to next format specifier structure 							*/
	char * 				p ;							/* Pointer to the format string 								*/
	int  				pc 				=  format_length ;	/* Number of format characters remaining							*/
	char  				format_character ;					/* Format specifier : format character 								*/
	int 				repeat_count,						/* Format specifier : number of elements to process (-1 = until the end of the input argument)	*/
	 				flags,							/* Format specifier : additional flags								*/
					size ;							/* Format specifier : computed size of input data 						*/
	
	// Check if a valid format string has been specified.
	if  ( format_length  <  1 )
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "Missing or null format specifier." ) ;
		return ( -1 ) ;
	    }
	    
	// Allocate first as much format specifier entries as there are characters in the input string. We will adjust 
	// to the real size before leaving
	format_specifiers 	=  ( packing_data_format * ) emalloc ( format_length * sizeof ( packing_data_format ) ) ;
	
	if  ( format_specifiers  ==  NULL )
		return ( -1 ) ;

	p 	=  ( char * ) format ;
	fsp 	=  format_specifiers ;
	
	while  ( pc )
	   {
		repeat_count 		=  0 ;
		flags 			=  0 ;
		size 			=  0 ;
		format_character 	=  * p ;
		
		/* Determine format specifier, length and input arguments needed */
		switch  ( * p )
		   {
			/* Formats that always take one argument : */
				
				/* Format 'a' : null-padded string */
				case 	PACK_FORMAT_NULL_PADDED_STRING :
					repeat_count 	=  __get_repeat_count ( & p, & pc ) ;
					//size 		=  __get_remaining_size ( & argv, argc, repeat_count, 1 ) ;
					break ;
		    }
		
		/* Create new format specifier entry if format character is a valid one */
		if  ( format_character )
		   {
			fsp -> format 	=  format_character ;
			fsp -> flags 	=  flags ;
			fsp -> repeat 	=  repeat_count ;
			fsp -> size 	=  size ;
			
			fsp ++, format_specifier_count ++ ;
		    }
		// Otherwise, abort parsing
		else
		   {
			efree ( format_specifiers ) ;
			return ( -1 ) ;
		    }
	    }
	
	// Last checks before returning
	if  ( ! format_specifier_count )		// No format specifier ?
	   {
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "No valid format specifier found." ) ;
		return ( -1 ) ;
	    }
	
	if  ( current_argc  <  argc )			// Unused arguments ?
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "%d arguments unused", argc - current_argc ) ;
	
	// Resize down the format specifiers array (remember : we allocated format_length entries at the beginning of the function, that was a max)
	if  ( format_specifier_count  !=  format_length ) 		
	   {
		format_specifiers 	=  erealloc ( format_specifiers, format_specifier_count * sizeof ( packing_data_format ) ) ;
		
		if  ( format_specifiers  ==  NULL )
			return ( -1 ) ;
	    }
	    
	// All done, return
	* output_format_specifiers 	=  format_specifiers ;
	* output_format_count 		=  format_specifier_count ;
    }
    
    
/*===========================================================================================================

    NAME
	data_pack - Packs series of data.
	
    DESCRIPTION
	
	
 *===========================================================================================================*/
THRAK_API int  internal_data_pack ( const char * 			format_string,
			  int  				format_string_length,
			  int 				argc,
			  zval **			argv,
			  char ** 			result,
			  size_t *			result_length
			  TSRMLS_DC ) 
   {
	int  		current_argc 	=  argc ;
	zval **		current_argv 	=  argv ;
	char 		ch ;
	int  		repeat_count ;
	int 		total_size 		=  0 ;
	
	while  ( format_string_length -- )
	   {
		ch 	=  * format_string ++ ;
		
		if  ( format_string_length )
		   {
			if  ( * format_string  ==  '*' )
			   {
				repeat_count 	=  -1 ;
				format_string ++ ;
				format_string_length -- ;
				
				total_size 	+=  Z_STRLEN_PP ( current_argv ) ;
			    }
			else if  ( isdigit ( * format_string ) )
			   {
				repeat_count 	=  0 ;
				
				while  ( isdigit ( * format_string ) )
				   {
					repeat_count 	=  ( repeat_count * 10 ) + ( * format_string - '0' ) ;
					format_string ++ ;
					format_string_length -- ;
				    }
				    
				total_size 	+=  repeat_count ;
			    }
		    }

		current_argc --, current_argv ++ ;
	    }

	if  ( current_argc )
		php_error_docref ( NULL TSRMLS_CC, E_WARNING, "%d arguments unused", ( argc - current_argc ) ) ;

	return ( 1 ) ;
    }
    
    
PHP_FUNCTION(data_pack)
   {
	char * 		format ;
	int 		format_length ;
	zval ***	argv ;
	int  		argc ;
	char *		result ;
	size_t 		result_length ;
	
	
	/* Parse arguments */
	if  ( zend_parse_parameters ( ZEND_NUM_ARGS() TSRMLS_CC, "s+", & format, & format_length, & argv, & argc ) == FAILURE )
		return;
	
	if  ( internal_data_pack ( format, format_length, argc, * argv, & result, & result_length TSRMLS_CC ) )
	   {
		efree ( argv ) ;
		//RETURN_STRINGL ( result, ( long ) result_length, 0 ) ;
		RETURN_FALSE;
	    }
	else
	   {
		efree ( argv ) ;
		RETURN_FALSE ;
	    }
   }

    
/*===========================================================================================================

    NAME
	data_unpack - Packs series of data.
	
    DESCRIPTION
	
	
 *===========================================================================================================*/
PHP_FUNCTION(data_unpack)
   {
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "not yet implemented");
    }
