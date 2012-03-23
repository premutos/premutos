AC_DEFUN([AX_CHECK_MYSQL],
[
    ## Determine MySQL library path
    # try and find libs without any additional libraries
    AC_CHECK_LIB(mysqlclient, mysql_init,
    	MYSQL_LIBS=" ",
    	MYSQL_LIBS=
    )
    
    # hunt through several directories trying to find the libraries
    if test "x$MYSQL_LIBS" = "x"; then
    	old_LDFLAGS="$LDFLAGS"
    	for try in $mysql_lib_dir /usr/lib /usr/lib/mysql /usr/local/lib /usr/local/mysql/lib /usr/local/lib/mysql ; do
    		LDFLAGS="$old_LDFLAGS -L$try"
    		unset ac_cv_lib_mysqlclient_mysql_init
    		AC_CHECK_LIB(mysqlclient, mysql_init,
    			MYSQL_LIBS="-L$try -lmysqlclient",
    			MYSQL_LIBS=
    		)
    		if test "x$MYSQL_LIBS" != "x"; then
    			break;
    		fi
    	done
      #	LDFLAGS="$old_LDFLAGS $MYSQL_LIBS"
      LDFLAGS="$old_LDFLAGS"
    fi
    
    if test "x$MYSQL_LIBS" = "x"; then
    	AC_MSG_ERROR([MySQL libraries not found.  Use --with-mysql-lib-dir=<path>])
    fi
    # don't forget to add mysqlclient to the list of libraries
    # LIBS="-lmysqlclient $LIBS"
  
    ## Determine MySQL include path
    AC_MSG_CHECKING(for mysql/mysql.h)
    
    # Can we include headers using system include dirs?
    AC_TRY_COMPILE([#include <mysql/mysql.h>], [int a = 1;],
    	MYSQL_INCLUDE=" ",
    	MYSQL_INCLUDE=
    )
    
    # hunt through several directories trying to find the includes
    if test "x$MYSQL_INCLUDE" = "x"; then
    	old_CPPFLAGS="$CPPFLAGS"
    	for i in $mysql_include_dir /usr/include /usr/local/include /usr/local/mysql/include /usr/local/include/mysql ; do
    		CPPFLAGS="$old_CPPFLAGS -I$i"
    		AC_TRY_COMPILE([#include <mysql/mysql.h>], [],
    			MYSQL_INCLUDE="-I$i",
    			MYSQL_INCLUDE=
    		)
    		if test "x$MYSQL_INCLUDE" != "x"; then
    			break;
    		fi
    	done
    	# CPPFLAGS="$old_CPPFLAGS $MYSQL_INCLUDE"
    	CPPFLAGS="$old_CPPFLAGS"
    fi
    
    if test "x$MYSQL_INCLUDE" = "x"; then
    	AC_MSG_RESULT(no) 
    	AC_MSG_ERROR([MySQL headers not found.  Use --with-mysql-include-dir=<path>])
    fi
    AC_MSG_RESULT(yes)
])
