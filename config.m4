dnl $Id$
PHP_ARG_ENABLE(vroom, whether to enable Vroom support, 
	[ --enable-vroom          Enable Vroom support] )

if test "$PHP_VROOM" != "no"; then
	AC_DEFINE ( HAVE_VROOM, 1, [ ] )
	PHP_NEW_EXTENSION ( vroom, "php_vroom.c vroom.c array_buffer.c zval.c", $ext_shared )
	PHP_INSTALL_HEADERS ( ext/vroom, "php_vroom.h array.h zval.h" )
fi
