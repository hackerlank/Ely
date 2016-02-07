# STOP_ON_CHECKS_ERR
# ------------------
# Defines: 
# Arguments: required_programs,required_libraries,required_headers,program
# If $1 or $2 or $3 are not empty issue an error message for 
# building $4 and exit.
#

#dnl $1=required_programs $2=required_libraries $3=required_headers $4=program
AC_DEFUN([STOP_ON_CHECKS_ERR], 
[
if test -n "$1" -o -n "$2" -o -n "$3" ; then
	error_msg="
	----------------------------------------
	These packages:"
	if test -n "$1" ; then
		error_msg=${error_msg}"
	-$1 programs"
	fi
	if test -n "$2" ; then
		error_msg=${error_msg}"
	-$2 development libraries (*.so)"
	fi
	if test -n "$3" ; then
		error_msg=${error_msg}"
	-$3 header files"
	fi
	error_msg=${error_msg}"
	are required to build $4.
	Stopping...
	Check 'config.log' for more information.
	----------------------------------------"					
	AC_MSG_ERROR(["${error_msg}"])
fi
])# STOP_ON_CHECKS_ERR
