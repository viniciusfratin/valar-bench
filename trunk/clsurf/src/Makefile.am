# Build CLSURF with AMD OpenCL libraries
if BUILD_CLSURF_AMD
bin_PROGRAMS = $(top_builddir)/bin/amd/OpenSurf

__top_builddir__bin_amd_OpenSurf_SOURCES = \
	clutils.cpp \
	clutils.h \
	cvutils.cpp \
	cvutils.h \
	eventlist.cpp \
	eventlist.h \
	fasthessian.cpp \
	fasthessian.h \
	kmeans.h \
	main.cpp \
	nearestNeighbor.cpp \
	nearestNeighbor.h \
	responselayer.cpp \
	responselayer.h \
	surf.cpp \
	surf.h \
	utils.cpp \
	utils.h

AM_CPPFLAGS = \
	@CLSURF_AMD_INCLUDES@

LDADD = \
	@CLSURF_AMD_LDADD@

endif