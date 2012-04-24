check_PROGRAMS = \
	test-log \
	test-schema

test_log_CXXFLAGS = -I$(top_builddir)/src
test_log_SOURCES = \
	test_log.cc
test_log_LDADD = \
	$(top_builddir)/src/lib/liblivecast-admin-lib.la

test_schema_CXXFLAGS = -I$(top_builddir)/src
test_schema_SOURCES = \
	test_schema.cc
test_schema_LDADD = \
	$(top_builddir)/src/gui/liblivecast_admin_gui_la-Util.o \
	$(top_builddir)/src/gui/liblivecast_admin_gui_la-StatusSchema.o \
	$(top_builddir)/src/lib/liblivecast-admin-lib.la \
	-lboost_program_options
