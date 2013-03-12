#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT(data-ipa, 1.0.0)
AM_INIT_AUTOMAKE(data-ipa, 1.0.0)
AC_OUTPUT(Makefile ipanat/src/Makefile ipacm/src/Makefile)
AC_CONFIG_SRCDIR([ipanat/src/ipa_nat_drv.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_CXX

PKG_CHECK_MODULES([LIBXML], [libxml-2.0])
AC_SUBST([LIBXML_CFLAGS])
AC_SUBST([LIBXML_LIBS])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h netinet/in.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([memset munmap])

AC_OUTPUT
