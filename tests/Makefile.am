check_PROGRAMS = \
	test-infos \
	test-schema

test_infos_CXXFLAGS = -I$(top_builddir)/src
test_infos_SOURCES = \
	test_infos.cc
test_infos_LDADD = \
	$(top_builddir)/src/gui/liblivecast_admin_gui_la-Util.o \
	$(top_builddir)/src/gui/liblivecast_admin_gui_la-LivecastListCtrl.o \
	$(top_builddir)/src/gui/liblivecast_admin_gui_la-LivecastInfos.o \
	$(top_builddir)/src/lib/liblivecast-admin-lib.la \
	-lboost_program_options

test_schema_CXXFLAGS = -I$(top_builddir)/src
test_schema_SOURCES = \
	test_schema.cc
test_schema_LDADD = \
	$(top_builddir)/src/gui/liblivecast_admin_gui_la-StatusSchema.o \
	$(top_builddir)/src/lib/liblivecast-admin-lib.la \
	-lboost_program_options
