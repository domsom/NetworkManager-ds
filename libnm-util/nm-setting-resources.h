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

#ifndef NM_SETTING_RESOURCES_H
#define NM_SETTING_RESOURCES_H

#include <nm-setting.h>

G_BEGIN_DECLS

#define NM_TYPE_SETTING_RESOURCES            (nm_setting_resources_get_type ())
#define NM_SETTING_RESOURCES(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NM_TYPE_SETTING_RESOURCES, NMSettingResources))
#define NM_SETTING_RESOURCES_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NM_TYPE_SETTING_RESOURCES, NMSettingResourcesClass))
#define NM_IS_SETTING_RESOURCES(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NM_TYPE_SETTING_RESOURCES))
#define NM_IS_SETTING_RESOURCES_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), NM_TYPE_SETTING_RESOURCES))
#define NM_SETTING_RESOURCES_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), NM_TYPE_SETTING_RESOURCES, NMSettingResourcesClass))

#define NM_SETTING_RESOURCES_SETTING_NAME "resources"

/**
 * NMSettingResourcesError:
 * @NM_SETTING_RESOURCES_ERROR_UNKNOWN: unknown or unclassified error
 * @NM_SETTING_RESOURCES_ERROR_INVALID_PROPERTY: the property was invalid
 * @NM_SETTING_RESOURCES_ERROR_MISSING_PROPERTY: the property was missing and is
 * required
 */
typedef enum {
	NM_SETTING_RESOURCES_ERROR_UNKNOWN = 0,           /*< nick=UnknownError >*/
	NM_SETTING_RESOURCES_ERROR_INVALID_PROPERTY,      /*< nick=InvalidProperty >*/
	NM_SETTING_RESOURCES_ERROR_MISSING_PROPERTY,      /*< nick=MissingProperty >*/
	NM_SETTING_RESOURCES_ERROR_NOT_ALLOWED_FOR_METHOD /*< nick=NotAllowedForMethod >*/
} NMSettingResourcesError;

#define NM_SETTING_RESOURCES_ERROR nm_setting_resources_error_quark ()
GQuark nm_setting_resources_error_quark (void);

#define NM_SETTING_RESOURCES_NETWORK_DRIVES             "network-drives"

typedef struct {
	NMSetting parent;
} NMSettingResources;

typedef struct {
	NMSettingClass parent;

	/* Padding for future expansion */
	void (*_reserved1) (void);
	void (*_reserved2) (void);
	void (*_reserved3) (void);
	void (*_reserved4) (void);
} NMSettingResourcesClass;

GType nm_setting_resources_get_type (void);

NMSetting *nm_setting_resources_new (void);

guint32       nm_setting_resources_get_num_network_drives (NMSettingResources *setting);
const char *  nm_setting_resources_get_network_drive      (NMSettingResources *setting, guint32 i);
gboolean      nm_setting_resources_add_network_drive      (NMSettingResources *setting, const char *network_drive);
void          nm_setting_resources_remove_network_drive_by_index (NMSettingResources *setting, guint32 i);
void          nm_setting_resources_remove_network_drive_by_uri (NMSettingResources *setting, const char *network_drive);
void          nm_setting_resources_clear_network_drives   (NMSettingResources *setting);
gboolean      nm_setting_resources_has_network_drive      (NMSettingResources *setting, const char *network_drive);
gboolean      nm_setting_resources_is_scheme_allowed      (char *scheme);
const char ** nm_setting_resources_get_allowed_schemes    (void);

G_END_DECLS

#endif /* NM_SETTING_RESOURCES_H */
