# STOP_ON_CHECKS_ERR
# ------------------
# Defines: 
# Arguments: required_libraries,required_headers,program
# If $1 or $2 are not empty issue an error message for 
# building $3 and exit.
#

#dnl $1=required_libraries $2=required_headers $3=program
AC_DEFUN([STOP_ON_CHECKS_ERR], 
[
if test -n "$1" -o -n "$2" ; then
	error_msg="
	----------------------------------------
	These packages:"
	if test -n "$1" ; then
		error_msg=${error_msg}"
	-$1 development libraries (*.so)"
	fi
	if test -n "$2" ; then
		error_msg=${error_msg}"
	-$2 header files"
	fi
	error_msg=${error_msg}"
	are required to build $3.
	Stopping...
	Check 'config.log' for more information.
	----------------------------------------"					
	AC_MSG_ERROR(["${error_msg}"])
fi
])# STOP_ON_CHECKS_ERR
