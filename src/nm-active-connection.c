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
 * Copyright (C) 2008 - 2012 Red Hat, Inc.
 */

#include <glib.h>
#include "nm-active-connection.h"
#include "NetworkManager.h"
#include "nm-logging.h"
#include "nm-dbus-glib-types.h"
#include "nm-dbus-manager.h"
#include "nm-properties-changed-signal.h"

#include "nm-active-connection-glue.h"

/* Base class for anything implementing the Connection.Active D-Bus interface */
G_DEFINE_ABSTRACT_TYPE (NMActiveConnection, nm_active_connection, G_TYPE_OBJECT)

#define NM_ACTIVE_CONNECTION_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
                                             NM_TYPE_ACTIVE_CONNECTION, \
                                             NMActiveConnectionPrivate))

typedef struct {
	gboolean disposed;

	NMConnection *connection;
	char *path;
	char *specific_object;
	char *device_path;
	gboolean is_default;
	gboolean is_default6;
	NMActiveConnectionState state;
	gboolean vpn;
} NMActiveConnectionPrivate;

enum {
	PROP_0,
	PROP_CONNECTION,
	PROP_UUID,
	PROP_SPECIFIC_OBJECT,
	PROP_DEVICES,
	PROP_STATE,
	PROP_DEFAULT,
	PROP_DEFAULT6,
	PROP_VPN,
	PROP_MASTER,

	LAST_PROP
};

enum {
	PROPERTIES_CHANGED,
	LAST_SIGNAL
};
static guint signals[LAST_SIGNAL] = { 0 };

/****************************************************************/

NMActiveConnectionState
nm_active_connection_get_state (NMActiveConnection *self)
{
	return NM_ACTIVE_CONNECTION_GET_PRIVATE (self)->state;
}

void
nm_active_connection_set_state (NMActiveConnection *self,
                                NMActiveConnectionState new_state)
{
	NMActiveConnectionPrivate *priv = NM_ACTIVE_CONNECTION_GET_PRIVATE (self);

	if (priv->state != new_state) {
		priv->state = new_state;
		g_object_notify (G_OBJECT (self), NM_ACTIVE_CONNECTION_STATE);
	}
}

NMConnection *
nm_active_connection_get_connection (NMActiveConnection *self)
{
	return NM_ACTIVE_CONNECTION_GET_PRIVATE (self)->connection;
}

const char *
nm_active_connection_get_path (NMActiveConnection *self)
{
	return NM_ACTIVE_CONNECTION_GET_PRIVATE (self)->path;
}

const char *
nm_active_connection_get_specific_object (NMActiveConnection *self)
{
	return NM_ACTIVE_CONNECTION_GET_PRIVATE (self)->specific_object;
}

void
nm_active_connection_set_specific_object (NMActiveConnection *self,
                                          const char *specific_object)
{
	NMActiveConnectionPrivate *priv = NM_ACTIVE_CONNECTION_GET_PRIVATE (self);

	if (g_strcmp0 (priv->specific_object, specific_object) == 0)
		return;

	g_free (priv->specific_object);
	priv->specific_object = g_strdup (specific_object);
	g_object_notify (G_OBJECT (self), NM_ACTIVE_CONNECTION_SPECIFIC_OBJECT);
}

void
nm_active_connection_set_default (NMActiveConnection *self, gboolean is_default)
{
	NMActiveConnectionPrivate *priv;

	g_return_if_fail (NM_IS_ACTIVE_CONNECTION (self));

	priv = NM_ACTIVE_CONNECTION_GET_PRIVATE (self);
	if (priv->is_default == is_default)
		return;

	priv->is_default = is_default;
	g_object_notify (G_OBJECT (self), NM_ACTIVE_CONNECTION_DEFAULT);
}

gboolean
nm_active_connection_get_default (NMActiveConnection *self)
{
	g_return_val_if_fail (NM_IS_ACTIVE_CONNECTION (self), FALSE);

	return NM_ACTIVE_CONNECTION_GET_PRIVATE (self)->is_default;
}

void
nm_active_connection_set_default6 (NMActiveConnection *self, gboolean is_default6)
{
	NMActiveConnectionPrivate *priv;

	g_return_if_fail (NM_IS_ACTIVE_CONNECTION (self));

	priv = NM_ACTIVE_CONNECTION_GET_PRIVATE (self);
	if (priv->is_default6 == is_default6)
		return;

	priv->is_default6 = is_default6;
	g_object_notify (G_OBJECT (self), NM_ACTIVE_CONNECTION_DEFAULT6);
}

gboolean
nm_active_connection_get_default6 (NMActiveConnection *self)
{
	g_return_val_if_fail (NM_IS_ACTIVE_CONNECTION (self), FALSE);

	return NM_ACTIVE_CONNECTION_GET_PRIVATE (self)->is_default6;
}

gboolean
nm_active_connection_export (NMActiveConnection *self,
                             NMConnection *connection,
                             const char *devpath)
{
	NMActiveConnectionPrivate *priv = NM_ACTIVE_CONNECTION_GET_PRIVATE (self);
	NMDBusManager *dbus_mgr;
	static guint32 counter = 0;

	g_return_val_if_fail (connection != NULL, FALSE);
	g_return_val_if_fail (devpath != NULL, FALSE);

	priv->connection = g_object_ref (connection);
	priv->path = g_strdup_printf (NM_DBUS_PATH "/ActiveConnection/%d", counter++);
	priv->device_path = g_strdup (devpath);

	dbus_mgr = nm_dbus_manager_get ();
	dbus_g_connection_register_g_object (nm_dbus_manager_get_connection (dbus_mgr),
	                                     priv->path, G_OBJECT (self));
	g_object_unref (dbus_mgr);
	return TRUE;
}

/****************************************************************/

static void
nm_active_connection_init (NMActiveConnection *self)
{
}

static void
dispose (GObject *object)
{
	NMActiveConnectionPrivate *priv = NM_ACTIVE_CONNECTION_GET_PRIVATE (object);

	if (!priv->disposed) {
		priv->disposed = TRUE;

		g_free (priv->path);
		g_free (priv->specific_object);
		g_free (priv->device_path);
		g_object_unref (priv->connection);
	}

	G_OBJECT_CLASS (nm_active_connection_parent_class)->dispose (object);
}

static void
set_property (GObject *object, guint prop_id,
			  const GValue *value, GParamSpec *pspec)
{
	NMActiveConnectionPrivate *priv = NM_ACTIVE_CONNECTION_GET_PRIVATE (object);

	switch (prop_id) {
	case PROP_SPECIFIC_OBJECT:
		priv->specific_object = g_value_dup_boxed (value);
		break;
	case PROP_DEFAULT:
		priv->is_default = g_value_get_boolean (value);
		break;
	case PROP_DEFAULT6:
		priv->is_default6 = g_value_get_boolean (value);
		break;
	case PROP_VPN:
		priv->vpn = g_value_get_boolean (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
get_property (GObject *object, guint prop_id,
			  GValue *value, GParamSpec *pspec)
{
	NMActiveConnectionPrivate *priv = NM_ACTIVE_CONNECTION_GET_PRIVATE (object);
	GPtrArray *devices;

	switch (prop_id) {
	case PROP_CONNECTION:
		g_value_set_boxed (value, nm_connection_get_path (priv->connection));
		break;
	case PROP_UUID:
		g_value_set_string (value, nm_connection_get_uuid (priv->connection));
		break;
	case PROP_SPECIFIC_OBJECT:
		g_value_set_boxed (value, priv->specific_object ? priv->specific_object : "/");
		break;
	case PROP_DEVICES:
		devices = g_ptr_array_sized_new (1);
		g_ptr_array_add (devices, g_strdup (priv->device_path));
		g_value_take_boxed (value, devices);
		break;
	case PROP_STATE:
		g_value_set_uint (value, priv->state);
		break;
	case PROP_DEFAULT:
		g_value_set_boolean (value, priv->is_default);
		break;
	case PROP_DEFAULT6:
		g_value_set_boolean (value, priv->is_default6);
		break;
	case PROP_VPN:
		g_value_set_boolean (value, priv->vpn);
		break;
	case PROP_MASTER:
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
nm_active_connection_class_init (NMActiveConnectionClass *vpn_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (vpn_class);

	g_type_class_add_private (vpn_class, sizeof (NMActiveConnectionPrivate));

	/* virtual methods */
	object_class->get_property = get_property;
	object_class->set_property = set_property;
	object_class->dispose = dispose;

	/* properties */
	g_object_class_install_property (object_class, PROP_CONNECTION,
		g_param_spec_boxed (NM_ACTIVE_CONNECTION_CONNECTION,
		                    "Connection",
		                    "Connection",
		                    DBUS_TYPE_G_OBJECT_PATH,
		                    G_PARAM_READABLE));

	g_object_class_install_property (object_class, PROP_UUID,
		g_param_spec_string (NM_ACTIVE_CONNECTION_UUID,
		                     "Connection UUID",
		                     "Connection UUID",
		                     NULL,
		                     G_PARAM_READABLE));

	g_object_class_install_property (object_class, PROP_SPECIFIC_OBJECT,
		g_param_spec_boxed (NM_ACTIVE_CONNECTION_SPECIFIC_OBJECT,
		                    "Specific object",
		                    "Specific object",
		                    DBUS_TYPE_G_OBJECT_PATH,
		                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property (object_class, PROP_DEVICES,
		g_param_spec_boxed (NM_ACTIVE_CONNECTION_DEVICES,
		                    "Devices",
		                    "Devices",
		                    DBUS_TYPE_G_ARRAY_OF_OBJECT_PATH,
		                    G_PARAM_READABLE));

	g_object_class_install_property (object_class, PROP_STATE,
		g_param_spec_uint (NM_ACTIVE_CONNECTION_STATE,
		                   "State",
		                   "State",
		                   NM_ACTIVE_CONNECTION_STATE_UNKNOWN,
		                   NM_ACTIVE_CONNECTION_STATE_DEACTIVATING,
		                   NM_ACTIVE_CONNECTION_STATE_UNKNOWN,
		                   G_PARAM_READABLE));

	g_object_class_install_property (object_class, PROP_DEFAULT,
		g_param_spec_boolean (NM_ACTIVE_CONNECTION_DEFAULT,
		                      "Default",
		                      "Is the default IPv4 active connection",
		                      FALSE,
		                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_DEFAULT6,
		g_param_spec_boolean (NM_ACTIVE_CONNECTION_DEFAULT6,
		                      "Default6",
		                      "Is the default IPv6 active connection",
		                      FALSE,
		                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_VPN,
		g_param_spec_boolean (NM_ACTIVE_CONNECTION_VPN,
		                      "VPN",
		                      "Is a VPN connection",
		                      FALSE,
		                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property (object_class, PROP_MASTER,
		g_param_spec_boxed (NM_ACTIVE_CONNECTION_MASTER,
		                    "Master",
		                    "Path of master device",
		                    DBUS_TYPE_G_OBJECT_PATH,
		                    G_PARAM_READABLE));

	/* Signals */
	signals[PROPERTIES_CHANGED] = 
		nm_properties_changed_signal_new (object_class,
		                                  G_STRUCT_OFFSET (NMActiveConnectionClass, properties_changed));

	dbus_g_object_type_install_info (G_TYPE_FROM_CLASS (vpn_class),
									 &dbus_glib_nm_active_connection_object_info);
}

