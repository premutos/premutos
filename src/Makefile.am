SUBDIRS = lib gui monitor

sbin_PROGRAMS = livecast-monitor

livecast_monitor_SOURCES = \
	livecast_monitor.cc \
	Configuration.cc

livecast_monitor_LDADD = \
	$(top_builddir)/src/lib/liblivecast-admin-lib.la \
	$(top_builddir)/src/monitor/liblivecast-admin-monitor.la \
	$(top_builddir)/src/gui/liblivecast-admin-gui.la \
	-lboost_thread -lboost_system -lboost_program_options

livecast_monitor_LDFLAGS = -pthread
