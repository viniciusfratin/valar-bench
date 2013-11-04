SUBDIRS =\
	libanalysis-devices \
	libprofiler \
	librules \
	libmisc \
	libtopology

lib_LIBRARIES = $(top_builddir)/lib/libhaptic.a

__top_builddir__lib_libhaptic_a_CPPFLAGS = $(AMDAPPSDKROOT)/include

__top_builddir__lib_libhaptic_a_LIBADD = \
	$(top_builddir)/clhaptic/libanalysis-devices/*.o \
	$(top_builddir)/clhaptic/libprofiler/*.o \
	$(top_builddir)/clhaptic/libmisc/*.o \
	$(top_builddir)/clhaptic/librules/*.o \
	$(top_builddir)/clhaptic/libtopology/*.o

__top_builddir__lib_libhaptic_a_SOURCES = \
	$(top_builddir)/clhaptic/libanalysis-devices/*.o \
	$(top_builddir)/clhaptic/libprofiler/*.o \
	$(top_builddir)/clhaptic/libmisc/*.o \
	$(top_builddir)/clhaptic/librules/*.o \
	$(top_builddir)/clhaptic/libtopology/*.o
