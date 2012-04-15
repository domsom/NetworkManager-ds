/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

/*
 * Dominik Sommer <dominik@sommer.name>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 * (C) Copyright 2007 - 2008 Novell, Inc.
 * (C) Copyright 2012 Dominik Sommer
 */

#include "nm-setting-resources.h"

#include <gio/gio.h>
#include <string.h>
#include <stdlib.h>

#include "nm-utils.h"
#include "nm-param-spec-specialized.h"
#include "nm-dbus-glib-types.h"

/**
 * nm_setting_resources_error_quark:
 *
 * Registers an error quark for #NMSettingResources if necessary.
 *
 * Returns: the error quark used for #NMSettingResources errors.
 **/
GQuark
nm_setting_resources_error_quark (void)
{
	static GQuark quark;

	if (G_UNLIKELY (!quark))
		quark = g_quark_from_static_string ("nm-setting-resources-error-quark");
	return quark;
}

G_DEFINE_TYPE (NMSettingResources, nm_setting_resources, NM_TYPE_SETTING)

#define NM_SETTING_RESOURCES_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), NM_TYPE_SETTING_RESOURCES, NMSettingResourcesPrivate))

typedef struct {
	GSList *network_drives;  /* array of const char* */
} NMSettingResourcesPrivate;

enum {
	PROP_0,
	PROP_NETWORK_DRIVES,

	LAST_PROP
};

static const char *allowed_schemes[] = {
		"smb",
		"dav",
		"davs",
		NULL
};

NMSetting *
nm_setting_resources_new (void)
{
	return (NMSetting *) g_object_new (NM_TYPE_SETTING_RESOURCES, NULL);
}

/**
 * nm_setting_resources_get_num_network_drives:
 * @setting: the #NMSettingResources
 *
 * Returns: the number of configured network drives
 **/
guint32
nm_setting_resources_get_num_network_drives (NMSettingResources *setting)
{
	g_return_val_if_fail (NM_IS_SETTING_RESOURCES (setting), 0);

	return g_slist_length (NM_SETTING_RESOURCES_GET_PRIVATE (setting)->network_drives);
}

/**
 * nm_setting_resources_get_network_drive:
 * @setting: the #NMSettingResources
 * @i: index number of the network drive to return
 *
 * Returns: the network drive at index @i
 **/
const char *
nm_setting_resources_get_network_drive (NMSettingResources *setting, guint32 i)
{
	NMSettingResourcesPrivate *priv;

	g_return_val_if_fail (NM_IS_SETTING_RESOURCES (setting), NULL);

	priv = NM_SETTING_RESOURCES_GET_PRIVATE (setting);
	g_return_val_if_fail (i <= g_slist_length (priv->network_drives), NULL);

	return (const char *) g_slist_nth_data (priv->network_drives, i);
}

/**
 * nm_setting_resources_add_network_drive:
 * @setting: the #NMSettingResources
 * @network_drive: the uri of the new network drive to add
 *
 * Adds a new network drive to the setting.  The given network drive is
 * duplicated internally and is not changed by this function.
 *
 * Returns: %TRUE if the network drive was added; %FALSE if the network drive
 * was already known.
 **/
gboolean
nm_setting_resources_add_network_drive (NMSettingResources *setting,
		const char *network_drive)
{
	NMSettingResourcesPrivate *priv;
	char *copy;
	GSList *iter;

	g_return_val_if_fail (NM_IS_SETTING_RESOURCES (setting), FALSE);
	g_return_val_if_fail (network_drive != NULL, FALSE);

	priv = NM_SETTING_RESOURCES_GET_PRIVATE (setting);
	for (iter = priv->network_drives; iter; iter = g_slist_next (iter)) {
		if (strcasecmp ((const char *) iter->data, network_drive) == 0)
			return FALSE;
	}

	copy = g_strdup (network_drive);
	g_return_val_if_fail (copy != NULL, FALSE);

	priv->network_drives = g_slist_append (priv->network_drives, copy);
	return TRUE;
}

/**
 * nm_setting_resources_remove_network_drive:
 * @setting: the #NMSettingResources
 * @i: index number of the network drive to remove
 *
 * Removes the network drive at index @i.
 **/
void
nm_setting_resources_remove_network_drive_by_index (NMSettingResources *setting, guint32 i)
{
	NMSettingResourcesPrivate *priv;
	GSList *elt;

	g_return_if_fail (NM_IS_SETTING_RESOURCES (setting));

	priv = NM_SETTING_RESOURCES_GET_PRIVATE (setting);
	elt = g_slist_nth (priv->network_drives, i);
	g_return_if_fail (elt != NULL);

	g_free(elt->data);
	priv->network_drives = g_slist_delete_link (priv->network_drives, elt);
}

/**
 * nm_setting_resources_remove_network_drive:
 * @setting: the #NMSettingResources
 * @network_drive: the uri of the network drive to remove
 *
 * Removes the network drive.
 **/
void
nm_setting_resources_remove_network_drive_by_uri (NMSettingResources *setting, const char *network_drive)
{
	NMSettingResourcesPrivate *priv;
	guint32 i;

	g_return_if_fail (NM_IS_SETTING_RESOURCES (setting));
	g_return_if_fail (network_drive != NULL);

	priv = NM_SETTING_RESOURCES_GET_PRIVATE (setting);

	for (i = 0; i < nm_setting_resources_get_num_network_drives(setting); i++) {
		if (strcasecmp ((char *)g_slist_nth_data (priv->network_drives, i), network_drive) == 0)
		{
			nm_setting_resources_remove_network_drive_by_index(setting, i);
			return;
		}
	}
}

/**
 * nm_setting_resources_clear_network_drives:
 * @setting: the #NMSettingResources
 *
 * Removes all configured network drives.
 **/
void
nm_setting_resources_clear_network_drives (NMSettingResources *setting)
{
	NMSettingResourcesPrivate *priv = NM_SETTING_RESOURCES_GET_PRIVATE (setting);

	g_return_if_fail (NM_IS_SETTING_RESOURCES (setting));

	nm_utils_slist_free (priv->network_drives, g_free);
	priv->network_drives = NULL;
}

/**
 * nm_setting_resources_has_network_drive:
 * @setting: the #NMSettingResources
 * @network_drive: the network drive to check
 *
 * Returns: %TRUE if this network drive is configured with these settings;
 * %FALSE if it is unknown
 **/
gboolean
nm_setting_resources_has_network_drive (NMSettingResources *setting, const char *network_drive)
{
	NMSettingResourcesPrivate *priv;
	GSList *iter;

	g_return_val_if_fail (NM_IS_SETTING_RESOURCES (setting), FALSE);
	g_return_val_if_fail (network_drive != NULL, FALSE);

	priv = NM_SETTING_RESOURCES_GET_PRIVATE (setting);
	for (iter = priv->network_drives; iter; iter = g_slist_next (iter)) {
		if (strcasecmp ((const char *) iter->data, network_drive) == 0)
			return TRUE;
	}

	return FALSE;
}

/**
 * nm_setting_resources_is_scheme_allowed:
 * @scheme: The scheme to be checked
 *
 * Checks if a given scheme (smb, ftp, dav, ...) is valid for auto-mounting
 **/
gboolean
nm_setting_resources_is_scheme_allowed (char *scheme)
{
	int i = 0;

	if ((scheme == NULL) || (strlen(scheme) == 0)) return FALSE;

	while (allowed_schemes[i] != NULL)
	{
		if (strcmp(allowed_schemes[i], scheme) == 0) return TRUE;
		i++;
	}

	return FALSE;
}

/**
 * nm_setting_resources_get_allowed_schemes:
 *
 * Returns the reference to the array of char* holding the allowed schemes
 * for auto-mounting, which is not to be modified / freed.
 **/
const char **
nm_setting_resources_get_allowed_schemes (void)
{
	return allowed_schemes;
}

static gboolean
verify (NMSetting *setting, GSList *all_settings, GError **error)
{
//	NMSettingResources *self = NM_SETTING_RESOURCES (setting);
	NMSettingResourcesPrivate *priv = NM_SETTING_RESOURCES_GET_PRIVATE (setting);
	GSList *iter;
	gboolean valid = TRUE;

	// Validate network drives for allowed network protocols, correct uri scheme
	for (iter = priv->network_drives; valid && iter; iter = g_slist_next (iter))
	{
		char *scheme, *host, *folder;

		// Check if uri scheme is supported:
		if ((!nm_utils_parse_mount_uri(iter->data, &scheme, &host, &folder)) || // uri is valid
			(!nm_setting_resources_is_scheme_allowed(scheme)) || // uri scheme is supported
			(host == NULL) || (strlen(host) == 0) || // host is set
			(((folder == NULL) || (strlen(folder) == 0)) && (strcmp(scheme, "smb") == 0))) // for smb scheme: folder is set
		{
			g_set_error (error,
			             NM_SETTING_RESOURCES_ERROR,
			             NM_SETTING_RESOURCES_ERROR_INVALID_PROPERTY,
			             NM_SETTING_RESOURCES_NETWORK_DRIVES);
			valid = FALSE;
		}

		if (scheme != NULL) g_free(scheme);
		if (host != NULL) g_free(host);
		if (folder != NULL) g_free(folder);
	}

	return valid;
}

static void
nm_setting_resources_init (NMSettingResources *setting)
{
	g_object_set (setting, NM_SETTING_NAME, NM_SETTING_RESOURCES_SETTING_NAME, NULL);
}

static void
finalize (GObject *object)
{
//	NMSettingResources *self = NM_SETTING_RESOURCES (object);

	G_OBJECT_CLASS (nm_setting_resources_parent_class)->finalize (object);
}

static void
set_property (GObject *object, guint prop_id,
		    const GValue *value, GParamSpec *pspec)
{
	NMSettingResources *setting = NM_SETTING_RESOURCES (object);
	NMSettingResourcesPrivate *priv = NM_SETTING_RESOURCES_GET_PRIVATE (setting);

	switch (prop_id) {
	case PROP_NETWORK_DRIVES:
		nm_utils_slist_free (priv->network_drives, g_free);
		priv->network_drives = g_value_dup_boxed (value);
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
	NMSettingResources *setting = NM_SETTING_RESOURCES (object);
	NMSettingResourcesPrivate *priv = NM_SETTING_RESOURCES_GET_PRIVATE (setting);

	switch (prop_id) {
	case PROP_NETWORK_DRIVES:
		g_value_set_boxed (value, priv->network_drives);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
nm_setting_resources_class_init (NMSettingResourcesClass *setting_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (setting_class);
	NMSettingClass *parent_class = NM_SETTING_CLASS (setting_class);

	g_type_class_add_private (setting_class, sizeof (NMSettingResourcesPrivate));

	/* virtual methods */
	object_class->set_property = set_property;
	object_class->get_property = get_property;
	object_class->finalize     = finalize;
	parent_class->verify       = verify;

	/* Properties */
	g_object_class_install_property
		(object_class, PROP_NETWORK_DRIVES,
		 _nm_param_spec_specialized (NM_SETTING_RESOURCES_NETWORK_DRIVES,
						  "Network drives",
						  "The network drives to mount/unmount when connected to / disconnected from this network",
						  DBUS_TYPE_G_LIST_OF_STRING,
						  G_PARAM_READWRITE | NM_SETTING_PARAM_SERIALIZE));
}
