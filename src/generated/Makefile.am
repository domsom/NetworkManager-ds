include $(GLIB_MAKEFILE)

noinst_LTLIBRARIES = libnm-generated.la

libnm_generated_la_SOURCES = \
	nm-enum-types.c \
	nm-enum-types.h \
	nm-marshal.c \
	nm-marshal.h

nm_daemon_all_sources = \
	$(top_srcdir)/src/*.[ch] \
	$(top_srcdir)/src/logging/*.[ch] \
	$(top_srcdir)/src/dns-manager/*.[ch] \
	$(top_srcdir)/src/vpn-manager/*.[ch] \
	$(top_srcdir)/src/dhcp-manager/*.[ch] \
	$(top_srcdir)/src/ip6-manager/*.[ch] \
	$(top_srcdir)/src/supplicant-manager/*.[ch] \
	$(top_srcdir)/src/backends/*.[ch] \
	$(top_srcdir)/src/ppp-manager/*.[ch] \
	$(top_srcdir)/src/dnsmasq-manager/*.[ch] \
	$(top_srcdir)/src/modem-manager/*.[ch] \
	$(top_srcdir)/src/bluez-manager/*.[ch] \
	$(top_srcdir)/src/firewall-manager/*.[ch] \
	$(top_srcdir)/src/settings/*.[ch]

if WITH_WIMAX
nm_daemon_all_sources += $(top_srcdir)/src/wimax/*.[ch]
endif

nm_daemon_sources = \
	$(filter-out %-glue.h %-bindings.h, $(wildcard $(nm_daemon_all_sources)))

GLIB_GENERATED = nm-enum-types.h nm-enum-types.c
nm_enum_types_sources = $(nm_daemon_sources)
GLIB_MKENUMS_H_FLAGS = --identifier-prefix NM
GLIB_MKENUMS_C_FLAGS = --identifier-prefix NM

GLIB_GENERATED += nm-marshal.h nm-marshal.c
nm_marshal_sources = $(nm_daemon_sources)

INCLUDES = \
	-I${top_srcdir}/include \
	-I${top_builddir}/include \
	-I${top_srcdir}/src \
	-I${top_srcdir}/src/logging \
	-I${top_srcdir}/src/dns-manager \
	-I${top_srcdir}/src/vpn-manager \
	-I${top_srcdir}/src/dhcp-manager \
	-I${top_srcdir}/src/ip6-manager \
	-I${top_srcdir}/src/supplicant-manager \
	-I${top_srcdir}/src/backends \
	-I${top_srcdir}/src/ppp-manager \
	-I${top_srcdir}/src/dnsmasq-manager \
	-I${top_srcdir}/src/modem-manager \
	-I$(top_srcdir)/src/bluez-manager \
	-I$(top_srcdir)/src/firewall-manager \
	-I$(top_srcdir)/src/settings \
	-I$(top_srcdir)/src/wifi \
	-I$(top_srcdir)/src/wimax \
	-I$(top_srcdir)/libnm-util

libnm_generated_la_CPPFLAGS = \
	$(DBUS_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(GUDEV_CFLAGS) \
	$(LIBNL_CFLAGS) \
	$(GMODULE_CFLAGS) \
	$(POLKIT_CFLAGS) \
	$(SYSTEMD_CFLAGS) \
	$(IWMX_SDK_CFLAGS)

libnm_generated_la_LIBADD = \
	$(GLIB_LIBS)

BUILT_SOURCES = $(GLIB_GENERATED)

