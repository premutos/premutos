check_PROGRAMS = \
	test-schema

test_schema_CXXFLAGS = -I$(top_builddir)/src
test_schema_SOURCES = \
	test_schema.cc
test_schema_LDADD = \
	$(top_builddir)/src/gui/liblivecast_admin_gui_la-StatusSchema.o \
	$(top_builddir)/src/lib/liblivecast-admin-lib.la \
	-lboost_program_options
