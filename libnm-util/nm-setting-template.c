/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

/*
 * Tambet Ingo <tambet@gmail.com>
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
 */

/* This file is just a template - it's not built nor included in the tarball.
   It's sole purpose is to make the process of creating new settings easier.
   Just replace 'template' with new setting name (preserving the case),
   remove this comment, and you're almost done.
*/

#include "nm-setting-template.h"

/**
 * nm_setting_template_error_quark:
 *
 * Registers an error quark for #NMSettingTemplate if necessary.
 *
 * Returns: the error quark used for #NMSettingTemplate errors.
 **/
GQuark
nm_setting_template_error_quark (void)
{
	static GQuark quark;

	if (G_UNLIKELY (!quark))
		quark = g_quark_from_static_string ("nm-setting-template-error-quark");
	return quark;
}

G_DEFINE_TYPE (NMSettingTemplate, nm_setting_template, NM_TYPE_SETTING)

#define NM_SETTING_TEMPLATE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), NM_TYPE_SETTING_TEMPLATE, NMSettingTemplatePrivate))

typedef struct {
} NMSettingTemplatePrivate;

enum {
	PROP_0,

	LAST_PROP
};

NMSetting *
nm_setting_template_new (void)
{
	return (NMSetting *) g_object_new (NM_TYPE_SETTING_TEMPLATE, NULL);
}

static gboolean
verify (NMSetting *setting, GSList *all_settings, GError **error)
{
	NMSettingTemplate *self = NM_SETTING_TEMPLATE (setting);
	return TRUE;
}

static void
nm_setting_template_init (NMSettingTemplate *setting)
{
	g_object_set (setting, NM_SETTING_NAME, NM_SETTING_TEMPLATE_SETTING_NAME, NULL);
}

static void
finalize (GObject *object)
{
	NMSettingTemplate *self = NM_SETTING_TEMPLATE (object);

	G_OBJECT_CLASS (nm_setting_template_parent_class)->finalize (object);
}

static void
set_property (GObject *object, guint prop_id,
		    const GValue *value, GParamSpec *pspec)
{
	NMSettingTemplate *setting = NM_SETTING_TEMPLATE (object);

	switch (prop_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
get_property (GObject *object, guint prop_id,
		    GValue *value, GParamSpec *pspec)
{
	NMSettingTemplate *setting = NM_SETTING_TEMPLATE (object);

	switch (prop_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
nm_setting_template_class_init (NMSettingTemplateClass *setting_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (setting_class);
	NMSettingClass *parent_class = NM_SETTING_CLASS (setting_class);

	g_type_class_add_private (setting_class, sizeof (NMSettingTemplatePrivate));

	/* virtual methods */
	object_class->set_property = set_property;
	object_class->get_property = get_property;
	object_class->finalize     = finalize;
	parent_class->verify       = verify;

	/* Properties */
}
