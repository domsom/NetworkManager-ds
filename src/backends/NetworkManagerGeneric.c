/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* NetworkManager -- Network link manager
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * (C) Copyright 2004 - 2012 Red Hat, Inc.
 * (C) Copyright 2006 Timothee Lecomte <timothee.lecomte@ens.fr>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <arpa/inet.h>
#include <string.h>

#include "NetworkManagerGeneric.h"
#include "nm-system.h"
#include "NetworkManagerUtils.h"
#include "nm-logging.h"
#include "nm-netlink-compat.h"
#include "nm-netlink-monitor.h"
#include "nm-netlink-utils.h"

/* Because of a bug in libnl, rtnl.h should be included before route.h */
#include <netlink/route/rtnl.h>

#include <netlink/route/addr.h>
#include <netlink/netlink.h>

/*
 * nm_generic_enable_loopback
 *
 * Bring up the loopback interface
 *
 */
void nm_generic_enable_loopback (void)
{
	struct nl_sock *	nlh = NULL;
	struct rtnl_addr *	addr = NULL;
	struct nl_addr *	nl_addr = NULL;
	guint32			binaddr = 0;
	int			iface_idx = -1;
	int			err;

	nlh = nm_netlink_get_default_handle ();
	if (!nlh)
		return;

	iface_idx = nm_netlink_iface_to_index ("lo");
	if (iface_idx < 0)
		return;

	nm_system_iface_set_up (iface_idx, TRUE, NULL);

	addr = rtnl_addr_alloc ();
	if (!addr)
		return;

	binaddr = htonl (0x7f000001); /* 127.0.0.1 */
	nl_addr = nl_addr_build (AF_INET, &binaddr, sizeof(binaddr));
	if (!nl_addr)
		goto out;
	rtnl_addr_set_local (addr, nl_addr);
	nl_addr_put (nl_addr);

	binaddr = htonl (0x7fffffff); /* 127.255.255.255 */
	nl_addr = nl_addr_build (AF_INET, &binaddr, sizeof(binaddr));
	if (!nl_addr)
		goto out;
	rtnl_addr_set_broadcast (addr, nl_addr);
	nl_addr_put (nl_addr);

	rtnl_addr_set_prefixlen (addr, 8);
	rtnl_addr_set_ifindex (addr, iface_idx);
	rtnl_addr_set_scope (addr, RT_SCOPE_HOST);
	rtnl_addr_set_label (addr, "lo");

	err = rtnl_addr_add (nlh, addr, 0);
	if (err && (err != -NLE_EXIST)) {
		nm_log_warn (LOGD_CORE, "error setting loopback address: (%d) %s",
			         err, nl_geterror (err));
	}
out:
	if (addr)
		rtnl_addr_put (addr);
}

/*
 * nm_generic_update_dns
 *
 * Make glibc/nscd aware of any changes to the resolv.conf file by
 * restarting nscd.
 *
 */
void nm_generic_update_dns (void)
{
}

/*
 * nm_generic_ipv6_use_tempaddr
 *
 * Get net.ipv6.conf.default.use_tempaddr value from /etc/sysctl.conf or
 * /lib/sysctl.d/sysctl.conf
 *
 */
int nm_generic_ipv6_use_tempaddr (void)
{
	char *contents = NULL;
	gsize len = 0;
	const char *group_name = "[forged_group]\n";
	char *sysctl_data = NULL;
	GKeyFile *keyfile;
	GError *error = NULL;
	int tmp, ret = -1;

	/* Read file contents to a string. */
	if (!g_file_get_contents ("/etc/sysctl.conf", &contents, &len, NULL))
		if (!g_file_get_contents ("/lib/sysctl.d/sysctl.conf", &contents, &len, NULL))
			return -1;

	/* Prepend a group so that we can use GKeyFile parser. */
	sysctl_data = g_strdup_printf ("%s%s", group_name, contents);

	keyfile = g_key_file_new ();
	if (keyfile == NULL)
		goto done;

	if (!g_key_file_load_from_data (keyfile, sysctl_data, len + strlen (group_name), G_KEY_FILE_NONE, NULL))
		goto done;

	tmp = g_key_file_get_integer (keyfile, "forged_group", "net.ipv6.conf.default.use_tempaddr", &error);
	if (error == NULL)
		ret = tmp;

done:
	g_free (contents);
	g_free (sysctl_data);
	g_clear_error (&error);
	g_key_file_free (keyfile);

	return ret;
}

#ifdef TARGET_GENERIC
void nm_backend_enable_loopback (void)
{
	nm_generic_enable_loopback ();
}

void nm_backend_update_dns (void)
{
	nm_generic_update_dns ();
}

int nm_backend_ipv6_use_tempaddr (void)
{
	return nm_generic_ipv6_use_tempaddr ();
}
#endif

