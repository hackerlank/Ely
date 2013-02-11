# BULLET_SEARCH
# --------------
# Defines: BULLET_CPPFLAGS
# Priority choise: cmd_line then usr_include then usr_local_include
#
AC_DEFUN([BULLET_SEARCH], 
[
CPPFLAGS_CMDLINE=${CPPFLAGS}
# check header first from cmd line specified include
AC_MSG_NOTICE([Looking for Bullet headers...])
BULLET_CPPFLAGS="-I/usr/include/bullet -I/usr/local/include/bullet"
CPPFLAGS="${CPPFLAGS} ${BULLET_CPPFLAGS}"
AC_CHECK_HEADERS([Bullet-C-Api.h])
if test "x${ac_cv_header_Bullet_C_Api_h}" != xyes; then
	AC_MSG_ERROR([
	----------------------------------------
	The Bullet header files are
	required to build Ely. Stopping...
	Check 'config.log' for more information.
	----------------------------------------])
fi	
CPPFLAGS=${CPPFLAGS_CMDLINE}
])# BULLET_SEARCH
