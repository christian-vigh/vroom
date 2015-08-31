# INTRODUCTION #

The **Vroom** Php extension is an external library that provides various extension functions to the user-space area.

# LIMITATIONS #

- The current version has only been tested on Windows platforms.
- It has no object extensions yet.

# FUNCTIONS #

## Array Functions ##

Miscellaneous array functions.

### $result = array\_flatten ( $array, $copy\_keys = false ) ###

Flattens an array so that it has only one nesting level.

#### PARAMETERS ####
	$array (array) -
		Array to be flattened.
		
	copy_keys (boolean) -
		When true, array keys are copied into the flattened array, whenever possible.
		
#### RETURN VALUE ####
Returns the flattened array.

#### NOTES ####
When both an outer array and a nested array contain the same associative key, the corresponding value in the outer array will be overridden by the one of the nested array.

Example :
	  
		print_r ( array_flatten ( [ 'a' => 1, [ 'a' => 'nested value', 'b' => 1 ] ] ) ) ;
		
will output :
	  
		[ 'a' => 'nested value', 'b' => 1 ]
		
Keys will be lost during the flattening process if their value is a nested array :
	
		print_r ( array_flatten ( [ 1, 'nested_array' => [ 2, 3 ] ] ) ) ;
		
will output :
	  
		[ 1, 2, 3 ] 

----------
### $bool 	=  iin\_array ( $needle, $haystack ) ###
Performs a case-insensitive search of the value $needle in the array $haystack.

#### PARAMETERS ####
	$needle (mixed) -
		Value to be searched.
		
	$haystack (array) -
		Array to be searched.

#### RETURN VALUE ####
True if the array contains the specified value, false otherwise.

----------

### $bool 	=  array\_ikey\_exists ( $needle, $haystack ) ###
Performs a case-insensitive search of the value $needle in the keys of the array $haystack.

#### PARAMETERS ####
	$needle (mixed) -
		Key to be searched.
		
	$haystack (array) -
		Array to be searched.

#### RETURN VALUE ####
True if the array contains the specified key, false otherwise.

----------

## Array buffer functions ##

Array buffer functions are intended to create fixed-size arrays containing fixed-size elements.
This allows for significant improvements in memory management ; for example, a 10000-integers array will occupy 40Kb, while its PHP equivalent will consume up to 1,4Mb.

In future versions of the Vroom library, an object interface will be implemented.

Note that array buffers are seen as resources in the Php user-space area.

### $new_resource = array\_buffer\_clone ( $resource ) ###
Clones an existing array buffer.

#### PARAMETERS ####
	$resource (resource) -
		Array buffer resource to be cloned.

#### RETURN VALUE ####
A resource that points to the clone of the specified input array buffer, or false if something went wrong.

----------

### $bytes_copied = array\_buffer\_copy ( $dst\_resource, $src\_resource, $num\_bytes = 0, dst\_offset = 0, $src\_offset = 0 ) ###
Copies the whole or a part of an array buffer over an existing one. Only the bytes that can fit into the destination buffer array will be copied.

#### PARAMETERS ####
	$dst_resource (resource) -
		Destination array buffer.
		
	$src_resource (resource) -
		Source array buffer.
		
	$num_bytes (integer) -
		Number of bytes to copy. If unspecified or zero, the whole contents of the source array buffer
		will be copied (up to destination buffer size).
		
	$dst_offset (integer) -
		Offset where to start the copy in the destination buffer.
		Defaults to zero.
		
	$src_offset (integer) -
		Offset where to start the copy from the source buffer.
		Defaults to zero.

#### RETURN VALUE ####
Returns the number of bytes copied, or false is some invalid parameters were specified.

----------

### $resource 	=  array\_buffer\_create ( $size ) ###
Creates an array buffer of the specified size.

#### PARAMETERS ####
	$size (int) -
		Array buffer size, in bytes.

#### RETURN VALUE ####
A resource identifying the allocated array buffer, or false if something went wrong.

----------

### array\_buffer\_free ( $resource ) ###
Frees the resources associated with the specified array buffer.

#### PARAMETERS ####
	$resource (resource) -
		Array buffer resource.

----------

### array\_buffer\_get ( $resource, $offset, $num\_bytes = 1 ) ###
Retrieves one or more bytes from an array buffer starting at the specified offset.

#### PARAMETERS ####
	$resource (resource) -
		Array buffer resource.
		
	$offset (int) -
		Offset of the byte to be retrieved.
		
	$num_bytes (integer) -
		Number of bytes to retrieve.

#### RETURN VALUE ####
When $num\_bytes is 1 (the default), array\_buffer\_get() returns the byte value at the specified offset of the array buffer.

When $num\_bytes is greater than 1, consecutive bytes are returned as a binary string.
This function returns false if the offset is invalid.

----------

### array\_buffer\_resize ( $resource, $new\_size ) ###
Resizes an array buffer.

#### PARAMETERS ####
	$resource (resource) -
		Array buffer resource.
		
	$size (int) -
		New size for the array buffer. If less than the current size, existing values above will be	truncated.

#### RETURN VALUE ####
True if the array buffer has been resized or remains unchanged because the new size is equal to the	actual size, false otherwise.

----------

### $bytes\_set = array\_buffer\_set ( $resource, $offset = 0, $init\_value = 0, $num\_bytes = 0  ) ###
Initializes a whole or range of an array buffer to the specified value.

#### PARAMETERS ####
	$resource (resource) -
		Array buffer to be initialized.
		
	$init\_value (integer) -
		Initialization value.
		
	$num\_bytes (integer) -
		Number of bytes to initialize. If unspecified or zero, the whole contents of the array buffer
		are initialized.
		
	$offset (integer) -
		Offset where to start initialization from.
		Defaults to zero.

#### RETURN VALUE ####
The number of bytes initialized, or false is some invalid parameters were specified.

----------
### $size 	=  array\_buffer\_length ( $resource ) ###
Returns the size in bytes of the array buffer associated to the specified resource.

#### PARAMETERS ####
	$resource (resource) -
		Array buffer resource.

#### RETURN VALUE ####
Size in bytes of the array buffer.

----------
### $binstr 	=  data\_dwpack ( $array, [ count, [ $endianness = DATA\_MACHINE\_ENDIANNESS ] ] ) ; ###
### $binstr 	=  data\_ddpack ( $array, [ count, [ $endianness = DATA\_MACHINE\_ENDIANNESS ] ] ) ; ###
### $binstr 	=  data\_dqpack ( $array, [ count, [ $endianness = DATA\_MACHINE\_ENDIANNESS ] ] ) ; ###
	
### $binstr 	=  data\_dwlpack ( $array, $count ) ; ###
### $binstr 	=  data\_ddlpack ( $array, $count ) ; ###
### $binstr 	=  data\_dqlpack ( $array, $count ) ; ###
	
### $binstr 	=  data\_dwbpack ( $array, $count ) ; ###
### $binstr 	=  data\_ddbpack ( $array, $count ) ; ###
### $binstr 	=  data\_dqbpack ( $array, $count ) ; ###

### $binstr 	=  data\_dwmpack ( $array, $count ) ; ###
### $binstr 	=  data\_ddmpack ( $array, $count ) ; ###
### $binstr 	=  data\_dqmpack ( $array, $count ) ; ###
These functions pack word/dword/qwords quantities, respectively. The default endianness is the machine one.

The groups of lpack/bpack/mpack functions packs values in little endian, big endian or machine endian format.

#### PARAMETERS ####
	$array (array or scalar value) -
		Array of values to be packed, or a scalar value if only one value is to be packed.
		In this case, the $count parameter is ignored.
		
	$count (integer) -
		Number of values from $array to be packed. If not specified, all values will be packed.
		
	$endianness (integer) -
		Endianness of the packed values ; either DATA_LITTLE_ENDIAN, DATA_BIG_ENDIAN or 
		DATA_MACHINE_ENDIAN to use the machine endianness.

#### RETURN VALUE ####
A binary string containing the packed values, or false if an error occurred, such as not enough memory.

## Minifying functions ##

This section describes fast minifying functions.

----------

### $string 	=  minify\_php 		( $file [, $preserve\_newlines = false ] ) ###
### $string 	=  minify\_php\_stream 	( $file [, $preserve\_newlines = false [, $use\_include\_path = false ] ] ) ###
### $string 	=  minify\_php\_string    ( $string [, $preserve\_newlines = false ] ) ###
Returns the minified contents of the specified PHP source file.

minify\_php\_stream() behaves as file\_get\_contents() for opening files (thus, an http url could be provided). There is a really low overhead compared to minify\_php (around 2% on Windows).

## String functions ##

This section describes extended string functions.

----------
### string_append ( &$value, ... ) ###
Appends series of values to the specified string.

#### PARAMETERS ####
	$value (string) -
		String where to append values.
		
	$... (mixed) -
		Any argument that can be converted to a string and appended to $value.

#### NOTES ####
This code is theorically faster than the inline string-catenation operator, except that calling a PHP_FUNCTION() from user-space has a cost.

When catenating 100 000 times a set of value takes 170ms using the catenation operator, it takes 615ms	for string\_append(), 535 of them being consumed just by calling the PHP\_FUNCTION().

----------
### $value 	=  string_atoi ( $str [, &$end = null ) ###
Provides an equivalent of the C atoi() function, interpreting the following codes :

- Decimal numbers 	: [1-9][0-9]*
- Octal numbers 	: 0[1-7]* or 0?[oO][0-7]+
- Hexadecimals numbers  : 0?[xX][0-9a-f]+
- Binary numbers 	: 0?[bB][01]+

#### PARAMETERS ####
	$str (string) -
		String where to append values.
		
	&$end (integer) -
		If specified, will receive the position in $str after the scanned value.

#### RETURN VALUE ####
The decoded integer value, or false if no integer value was present in the input string.

#### NOTES ####
Leading spaces are ignored.

----------
### $bool 	=  string\_endswith ( $value, $suffix, $case\_insensitive = false ) ###
Checks if the specified string ends with the specified suffix.

The case_insensitive parameter determines whether comparison should be case-sensitive or not.

#### PARAMETERS ####
	$value (string) -
		String to be checked.
		
	$suffix (string) -
		String to be compared with the end of $value.
		
	$case_insensitive (boolean) -
		When true, comparison is case-insensitive.

#### RETURN VALUE ####
True if the supplied strings ends with the specified parameter, false otherwise.

----------
### $array 	=  string_explode ( $separator, $value [ [ [ , $left [, $right ] ], $options ], $callback ] ) ###
### $string	=  string_implode ( $separator, $array [ [ [ , $left [, $right ] ], $options ], $callback ] ) ###
Behaves similarly to the standard PHP functions implode()/explode(), with a few enhancements :

- Multiple separators can be given (in this case, $separator must be an array, not a string)
- Values must be prepended or appended to the exploded string ($left / $right)
- Several operations can be applied to each expanded value ($options).
	
Note that the $left, $right, $options and $callback parameters can be specified in any order, the	only restriction being that $left must always be specified before $right.

#### PARAMETERS ####
	$separator (string or array of strings) -
		Separator(s) to be used for string separation.
		
	$value (string) -
		String to be exploded or array to be imploded.

	$left (string) -
		When specified, string to be prepended to each exploded/imploded value.
		
	$right (string) -
		When specified, string to be appended to each exploded/imploded value.
		
	$options (integer) -
		For string_explode(), one of the following bit options :
		- EXPLODE_TRIM_LEFT :
			Left trims each exploded string.
		- EXPLODE_TRIM_RIGHT :
			Right trims each exploded string.
		- EXPLODE_TRIM :
			Trims each exploded string left and right.
		- EXPLODE_ESCAPE_QUOTES :
			Escapes each single quote withing the exploded string.
		- EXPLODE_ESCAPE_DOUBLEQUOTES :
			Escapes each double quote withing the exploded string.
		- EXPLODE_NOCASE :
			When specified, separator(s) is(are) considered to be case-insensitive.
		- EXPLODE_IGNORE_EMPTY :
			When specified, empty strings, after processing, are ignored.
		- EXPLODE_TOLOWER :
			Converts the string to lowercase.
		- EXPLODE_TOUPPER :
			Converts the string to uppercase.
		- EXPLODE_UCFIRST :
			Uppercases the first character of the string, lowercases every other one.
		- EXPLODE_UCWORDS :
			Uppercases the first letter of each word.
		- EXPLODE_ESCAPE_QUOTES :
			Escapes quote characters.
		- EXPLODE_ESCAPE_DOUBLEQUOTES :
			Escapes doublequote characters.

		For string_implode(), constants are the same except that EXPLODE_ is replaced by IMPLODE_.
		
	$callback (callback) -
		A callback function that have the following signature :
		
			mixed  callback ( $index, $value ) ;
			
		The callback function is called before any processing guided by the $options parameter is
		performed.

		The function can return one of this values :
		- null or boolean false (or no value returned at all) :
			The input value is kept as is.
		- boolean true :
			The input value is discarded.
		- other (string or scalar type) :
			The input value is replaced by the returned one.

#### RETURN VALUE ####
For string_explode(), an array containing the exploded strings.

Note that the function behaves a little bit differently from explode() : if the input value is empty, then an empty array will be returned.
	
For string_implode(), a string containing the concatenated values.
	
Both functions return false if something went wrong (eg, invalid parameter type or memory allocation error).

----------
### $bool 	=  string\_startswith ( $value, $prefix, $case\_insensitive = false ) ###
Checks if the specified string starts with the specified prefix.
The case_insensitive parameter determines whether comparison should be case-sensitive or not.

#### PARAMETERS ####
	$value (string) -
		String to be checked.
		
	$prefix (string) -
		String to be compared with the start of $value.
		
	$case_insensitive (boolean) -
		When true, comparison is case-insensitive.

#### RETURN VALUE ####
True if the specified string starts with the specified prefix, false otherwise.

----------
### $bool 	=  string\_setbos ( &$value, $prefix, [ $case\_insensitive = false, [ $return\_value = false ] ] ) ###
Prepends the $prefix string to $value if not already present.

#### PARAMETERS #### 
	$value (string) -
		String to be checked.
		
	$prefix (string) -
		String to be prepended to $value if $value does not start with that string.
		
	$case_insensitive (boolean) -
		When true, comparison is case-insensitive.
		
	$return_value (boolean) -
		When true, the new value is returned and $value is left as is.
		When false, $value is modified in-place.

#### RETURN VALUE ####
When the $return\_value parameter is false, returns true if operation succeeded or false otherwise.

When $return\_value is true, returns either the new string or false if operation failed or no update was performed.

----------
### $bool 	=  string\_seteos ( &$value, $suffix, [ $case\_insensitive = false, [ $return\_value = false ] ] ) ###
Adds the $suffix string to $value if not already present.

#### PARAMETERS ####
	$value (string) -
		String to be checked.
		
	$suffix (string) -
		String to be appended to $value if $value does not end with that string.
		
	$case_insensitive (boolean) -
		When true, comparison is case-insensitive.
		
	$return_value (boolean) -
		When true, the new value is returned and $value is left as is.
		When false, $value is modified in-place.

#### RETURN VALUE ####
When the $return\_value parameter is false, returns true if operation succeeded or false otherwise.

When $return\_value is true, returns either the new string or false if operation failed or no update was performed.

## Timer functions ##

### $value 	=  timer\_clock ( [ $resolution = TIMER\_RESOLUTION\_NATIVE ]) ####
Returns the current value of the high resolution clock.

#### PARAMETERS ####
	$resolution (integer) -
		Indicates the resolution of the returned value :
		- TIMER_RESOLUTION_NATIVE :
			The natural number of ticks will be returned?
		- TIMER_RESOLUTION_SECOND, MILLISECOND, MICROSECOND, NANO_SECOND :
			The returned value will be converted to the specified unit.

#### RETURN VALUE ####
The current value of the high resolution clock.

----------
### $value 	=  timer\_ticks\_to ( $ticks [,  $resolution = TIMER\_RESOLUTION\_NATIVE ]) ###
### $value 	=  timer\_ticks\_to\_seconds ( $ticks ) ###
### $value 	=  timer\_ticks\_to__milliseconds ( $ticks ) ###
### $value 	=  timer\_ticks\_to\_microseconds ( $ticks ) ###
### $value 	=  timer\_ticks\_to\_nanoseconds ( $ticks ) ###
Converts timer ticks to the specified resolution.

#### PARAMETERS ####
	$ticks (double) -
		Tick count to be converted.
		
	$resolution (integer) -
		Indicates the resolution of the returned value :
		- TIMER_RESOLUTION_NATIVE :
			The natural number of ticks will be returned.
		- TIMER_RESOLUTION_SECOND, MILLISECOND, MICROSECOND, NANO_SECOND :
			The supplied tick count value will be converted to the specified unit.

#### RETURN VALUE ####
The number of ticks supplied, converted to the specified resolution.

----------
### $value 	=  timer\_diff ( $current, $previous [, $resolution = TIMER\_RESOLUTION\_NATIVE ]) ###
Computes the differences between two timer clock values and returns the result, converted to the	specified resolution.

#### PARAMETERS ####
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

#### RETURN VALUE ####
The delta between the current and previous timer values, converted to the specified resolution.

----------
### $value 	=  timer\_clock\_delta ( [ $resolution = TIMER\_RESOLUTION\_NATIVE ]) ###
Returns the delta between the call of the last timer\_clock() or timer\_clock\_delta() functions.

If none of these functions was called, returns the current clock value.

#### PARAMETERS ####
	$resolution (integer) -
		Indicates the resolution of the returned value :
		- TIMER_RESOLUTION_NATIVE :
			The natural number of ticks will be returned?
		- TIMER_RESOLUTION_SECOND, MILLISECOND, MICROSECOND, NANO_SECOND :
			The returned value will be converted to the specified unit.

#### RETURN VALUE ####
The delta between the current clock and the value of the clock during the last call to timer\_clock\_delta() or timer\_clock().

## ZVAL functions ##
This section describes lots of uninteresting functions about PHP ZVALs.


----------
### $array 	= zval\_dump ( &$variable ) ###
Returns an associative array containing the fields of a zval structure.

#### PARAMETERS ####
	$variable (mixed) -
		Variable to be dumped.

#### RETURN VALUE ####
An associative array containing the following keys :

- 'type' : zval value type.
- 'type-string' : zval value type, as a string.
- 'refcount' : Reference count. See notes below.
- 'isref' : 	A boolean indicating whether this zval is a reference or not. See notes below.
- 'value' : 	The underlying zval value.
- 'value-length' : For string values, contain the string length.

#### NOTES ####
Since this function accepts a reference to a zval, its reference count field will be automatically incremented and the zval will be set to be a reference.

We try to simulate how the original value was before calling this function :

- Reference count is decremented by one
- The "isref" field of the zval is set to zero if the decremented reference count is one.
- 
This may not reflect the actual situation but it tends to be more accurate than debug\_zval\_dump() function.

----------
### int  	zval_decref ( &$variable ) ###
### int  	zval_incref ( &$variable ) ###
### int  	zval_setref ( &$variable, $refcount ) ###
Manipulates a zval reference count :

- zval_decref decrements the reference count
- zval_incref increments it
- and zval_setref assigns a new reference count

#### PARAMETERS #### 
	$variable (mixed) -
		Variable whose reference count is to be returned.
		
	$refcount (int) -
		New reference count.

#### RETURN VALUE ####
All functions return the previous reference count.

----------
### bool  	zval_isref ( $variable ) ###
Returns a boolean indicating whether the specified variable is a reference or not.

#### PARAMETERS ####
	$variable (mixed) -
		Variable whose type is to be retrieved.

#### RETURN VALUE ####
True if the specified variable is a reference, false otherwise.

----------
### zval_swap ( $value1, $value2 ) ###
Swaps the contents of two zvals.

#### PARAMETERS ####
	$value1, $value2 (mixed) -
		Variables to be swapped.

----------
### int  	zval_type ( $variable ) ###
Returns the type of a zval.

#### PARAMETERS ####
	$variable (mixed) -
		Variable whose type is to be retrieved.

#### RETURN VALUE ####
One of the constants ZVAL\_NULL, ZVAL\_BOOL, ZVAL\_LONG, ZVAL\_STRING, ZVAL\_RESOURCE, ZVAL\_ARRAY, ZVAL\_OBJECT, or ZVAL\_UNKNOWN if the zval type could not be determined.

----------
### int  	zval_type_constant ( $variable ) ###
Returns the type of a zval as a string constant.

#### PARAMETERS ####
	$variable (mixed) -
		Variable whose type is to be retrieved.

#### RETURN VALUE ####
One of the constants "ZVAL\_NULL", "ZVAL\_BOOL", "ZVAL\_LONG", "ZVAL\_STRING", "ZVAL\_RESOURCE", "ZVAL\_ARRAY",	"ZVAL\_OBJECT", or "ZVAL\_UNKNOWN" if the zval type could not be determined.
