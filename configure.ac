AC_INIT([valar-bench], [0.1])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror])
AM_PROG_AS
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

# Silent rules if supported
ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Libtool
LT_INIT
AC_PROG_LIBTOOL

#############
# Test OpenCV
#############

have_opencv=yes

AC_CHECK_HEADER([opencv2/core/core_c.h], [], [have_opencv=no])
AC_CHECK_HEADER([opencv2/imgproc/imgproc_c.h], [], [have_opencv=no])
AC_CHECK_HEADER([opencv2/legacy/legacy.hpp], [], [have_opencv=no])
AC_CHECK_HEADER([opencv2/highgui/highgui_c.h], [], [have_opencv=no])
AC_CHECK_HEADER([opencv2/video/video.hpp], [], [have_opencv=no])

LDADD_OPENCV="-lopencv_imgproc -lopencv_legacy -lopencv_highgui -lopencv_video -lopencv_core"
AC_SUBST([LDADD_OPENCV])

# Export temporary LDFLAGS
saved_libs="$LIBS"
LIBS="$LDADD_OPENCV"

# Try link
AC_MSG_CHECKING([for $LDADD_OPENCV])
AC_LINK_IFELSE(
	[
		AC_LANG_PROGRAM(
			[[
				#include <opencv2/core/core_c.h>
				#include <opencv2/imgproc/imgproc_c.h>
				#include <opencv2/legacy/legacy.hpp>
				#include <opencv2/highgui/highgui_c.h>
				#include <opencv2/video/video.hpp>
			]],
			[[
				cvCloneImage(NULL);
			]]
		)
	],
	[
		AC_MSG_RESULT(yes)
	],
	[
		AC_MSG_RESULT(no)
		have_opencv=no
	]
)

# Restore original LIBS
LIBS="$saved_libs"

if test "x$have_opencv" = "xno"
then
		AC_MSG_WARN([

	The OpenCV libraries are not installed in your system. These
	libraries are needed to compile clSurf benchmark

	In Ubuntu, you can install the package by using the following command:

		sudo apt-get install libopencv-dev]
	)
fi


# Define macro for Automake and gcc
AM_CONDITIONAL(HAVE_OPENCV, test x$have_opencv = xyes )
if test x$have_opencv = xyes 
then
	AC_DEFINE(HAVE_OPENCV, [1])
fi

##################
# Test AMD OpenCL
##################
have_amd_opencl=yes

if test "x$AMDAPPSDKROOT" != "x"
then
	# Use temporary CFLAGS, LDFLAGS
	saved_cflags="$CFLAGS"
	saved_ldflags="$LDFLAGS"
	saved_libs="$LIBS"
	CFLAGS="-I$AMDAPPSDKROOT/include"
	LDFLAGS="-L$AMDAPPSDKROOT/lib/x86 -L$AMDAPPSDKROOT/lib/x86_64"
	LIBS="-lOpenCL"

	# Try program
	AC_LINK_IFELSE(
	[
		AC_LANG_PROGRAM(
			[[#include <CL/cl.h>]],
			[[
				cl_int ret;
				cl_platform_id platform;
				cl_uint num_platforms;
				ret = clGetPlatformIDs(1, &platform, &num_platforms);
			        return CL_SUCCESS;
			]]
		)
	],
	[
		have_amd_opencl=yes
	])

	# Restore original CFLAGS, LDFLAGS, LIBS
	CFLAGS="$saved_cflags"
	LDFLAGS="$saved_ldflags"
	LIBS="$saved_libs"
fi

# Warning if not
if test "x$have_amd_opencl" = "xno"
then
	AC_MSG_WARN([
	
	The AMD OpenCL SDK is not presently installed in your system, as
	detected by the absence of the AMDAPPSDKROOT environment variable. The
	headers and libraries provided by this package are needed to build the
	Valar benchmark. Otherwise the AMD version won't be build.
	])
fi


##########################
# Test clSurf requirement
##########################
AM_CONDITIONAL(BUILD_CLSURF_AMD, test x$have_amd_opencl == xyes -a x$have_opencv == xyes)
if test x$have_amd_opencl = xyes -a x$have_opencv = xyes
then
	AC_DEFINE(BUILD_CLSURF_AMD, [1])
	CLSURF_AMD_INCLUDES="-I$AMDAPPSDKROOT/include"
	AC_SUBST([CLSURF_AMD_INCLUDES])
	CLSURF_AMD_LDADD="$LDADD_OPENCV -lOpenCL"
	AC_SUBST([CLSURF_AMD_LDADD])
fi


###########
# Makefiles
###########

AC_CONFIG_FILES([
	Makefile
	clsurf/Makefile
	clsurf/src/Makefile
])

AC_OUTPUT