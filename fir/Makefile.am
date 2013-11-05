if HAVE_AMD_OPENCL

bin_PROGRAMS = $(top_builddir)/bin/amd/fir

__top_builddir__bin_amd_fir_SOURCES = \
	FIR_tapchange.cpp \
	FIR_tapchange.h \
	tap-change-device.cpp \
	tap-change-device.h

AM_CPPFLAGS = @AMD_OPENCL_INCLUDES@ -I$(top_builddir)/clhaptic/libanalysis-devices -I$(top_builddir)/clhaptic/libinterceptor -I$(top_builddir)/clhaptic/libmisc -I$(top_builddir)/clhaptic/libprofiler -I$(top_builddir)/clhaptic/librules -I$(top_builddir)/clhaptic/libtopology 

AM_LDFLAGS = @AMD_OPENCL_LDFLAGS@ -L$(top_builddir)/lib

LDADD = @AMD_OPENCL_LDADD@ -lhaptic

endif