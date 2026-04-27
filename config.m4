dnl SPDX-License-Identifier: LGPL-2.0-or-later
dnl vim:se ts=2 sw=2 et:

PHP_ARG_ENABLE(sfcgal, whether to enable sfcgal support,
[  --enable-sfcgal             Enable sfcgal support])

PHP_ARG_WITH(sfcgal-config, for sfcgal-config,
[  --with-sfcgal-config[=DIR]  Use sfcgal-config (default: search PATH)])

if test -z "$PHP_DEBUG"; then
  AC_ARG_ENABLE(debug,
  [  --enable-debug          compile with debugging symbols],[
    PHP_DEBUG=$enableval
  ],[    PHP_DEBUG=no
  ])
fi

if test "$PHP_SFCGAL" != "no"; then
  if test "$PHP_SFCGAL_CONFIG" != "yes" -a -n "$PHP_SFCGAL_CONFIG"; then
    if test -x "$PHP_SFCGAL_CONFIG"; then
      SFCGAL_CONFIG="$PHP_SFCGAL_CONFIG"
    elif test -x "$PHP_SFCGAL_CONFIG/bin/sfcgal-config"; then
      SFCGAL_CONFIG="$PHP_SFCGAL_CONFIG/bin/sfcgal-config"
    fi
  else
    AC_PATH_PROG(SFCGAL_CONFIG, sfcgal-config)
  fi

  if test -z "$SFCGAL_CONFIG"; then
    AC_MSG_ERROR([Cannot find sfcgal-config. Pass --with-sfcgal-config=DIR.])
  fi

  SFCGAL_VERSION=`$SFCGAL_CONFIG --version`
  SFCGAL_INCLUDE=`$SFCGAL_CONFIG --cflags | sed -e 's/^-I//'`
  SFCGAL_LDFLAGS=`$SFCGAL_CONFIG --libs`
  AC_MSG_RESULT([Using SFCGAL version $SFCGAL_VERSION])
  AC_DEFINE(HAVE_SFCGAL_C_H, 1, [Whether to have SFCGAL/capi/sfcgal_c.h])

  dnl Hard floor: SFCGAL >= 2.2.0
  AC_MSG_CHECKING([for SFCGAL >= 2.2.0])
  AS_VERSION_COMPARE([$SFCGAL_VERSION], [2.2.0],
    [AC_MSG_ERROR([SFCGAL >= 2.2.0 is required, found $SFCGAL_VERSION])],
    [AC_MSG_RESULT([yes ($SFCGAL_VERSION)])],
    [AC_MSG_RESULT([yes ($SFCGAL_VERSION)])])

  old_CFLAGS=$CFLAGS
  CFLAGS="-I$SFCGAL_INCLUDE $CFLAGS"

  AC_CHECK_HEADER(SFCGAL/capi/sfcgal_c.h,, AC_MSG_ERROR(Cannot find SFCGAL/capi/sfcgal_c.h))

  CFLAGS=$old_CFLAGS

  PHP_EVAL_LIBLINE($SFCGAL_LDFLAGS, SFCGAL_SHARED_LIBADD)
  PHP_SUBST(SFCGAL_SHARED_LIBADD)
  PHP_ADD_INCLUDE($SFCGAL_INCLUDE)
  PHP_NEW_EXTENSION(sfcgal, sfcgal.c, $ext_shared,,)
fi
