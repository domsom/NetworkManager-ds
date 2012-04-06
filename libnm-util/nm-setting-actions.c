#include <string.h>

#include "nm-setting-actions.h"

/**
 * nm_setting_actions_error_quark:
 *
 * Registers an error quark for #NMSettingActions if necessary.
 *
 * Returns: the error quark used for #NMSettingActions errors.
 **/
GQuark
nm_setting_actions_error_quark (void)
{
	static GQuark quark;

	if (G_UNLIKELY (!quark))
		quark = g_quark_from_static_string ("nm-setting-actions-error-quark");
	return quark;
}


G_DEFINE_TYPE (NMSettingActions, nm_setting_actions, NM_TYPE_SETTING)

#define NM_SETTING_ACTIONS_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), NM_TYPE_SETTING_ACTIONS, NMSettingActionsPrivate))

typedef struct {
	char *connect_script;
	char *disconnect_script;
} NMSettingActionsPrivate;

enum {
	PROP_0,
	PROP_CONNECT_SCRIPT,
	PROP_DISCONNECT_SCRIPT,

	LAST_PROP
};

NMSetting *
nm_setting_actions_new (void)
{
	return (NMSetting *) g_object_new (NM_TYPE_SETTING_ACTIONS, NULL);
}

const char *
nm_setting_actions_get_connect_script (NMSettingActions *setting)
{
	g_return_val_if_fail (NM_IS_SETTING_ACTIONS (setting), NULL);

	return NM_SETTING_ACTIONS_GET_PRIVATE (setting)->connect_script;
}

const char *
nm_setting_actions_get_disconnect_script (NMSettingActions *setting)
{
	g_return_val_if_fail (NM_IS_SETTING_ACTIONS (setting), NULL);

	return NM_SETTING_ACTIONS_GET_PRIVATE (setting)->disconnect_script;
}

//static gint
//find_setting_by_name (gconstpointer a, gconstpointer b)
//{
//	NMSetting *setting = NM_SETTING (a);
//	const char *str = (const char *) b;
//
//	return strcmp (nm_setting_get_name (setting), str);
//}

static gboolean
verify (NMSetting *setting, GSList *all_settings, GError **error)
{
//	NMSettingActionsPrivate *priv = NM_SETTING_ACTIONS_GET_PRIVATE (setting);

	// FIXME: Check scripts exist & are executable

	return TRUE;
}

static void
nm_setting_actions_init (NMSettingActions *setting)
{
	g_object_set (setting, NM_SETTING_NAME, NM_SETTING_ACTIONS_SETTING_NAME, NULL);
}

static void
finalize (GObject *object)
{
	NMSettingActionsPrivate *priv = NM_SETTING_ACTIONS_GET_PRIVATE (object);

	g_free (priv->connect_script);
	g_free (priv->disconnect_script);

	G_OBJECT_CLASS (nm_setting_actions_parent_class)->finalize (object);
}

static void
set_property (GObject *object, guint prop_id,
		    const GValue *value, GParamSpec *pspec)
{
	NMSettingActionsPrivate *priv = NM_SETTING_ACTIONS_GET_PRIVATE (object);

	switch (prop_id) {
	case PROP_CONNECT_SCRIPT:
		g_free (priv->connect_script);
		priv->connect_script = g_value_dup_string (value);
		break;
	case PROP_DISCONNECT_SCRIPT:
		g_free (priv->disconnect_script);
		priv->disconnect_script = g_value_dup_string (value);
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
	NMSettingActions *setting = NM_SETTING_ACTIONS (object);

	switch (prop_id) {
	case PROP_CONNECT_SCRIPT:
		g_value_set_string (value, nm_setting_actions_get_connect_script (setting));
		break;
	case PROP_DISCONNECT_SCRIPT:
		g_value_set_string (value, nm_setting_actions_get_disconnect_script (setting));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
nm_setting_actions_class_init (NMSettingActionsClass *setting_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (setting_class);
	NMSettingClass *parent_class = NM_SETTING_CLASS (setting_class);

	g_type_class_add_private (setting_class, sizeof (NMSettingActionsPrivate));

	/* virtual methods */
	object_class->set_property = set_property;
	object_class->get_property = get_property;
	object_class->finalize     = finalize;
	parent_class->verify       = verify;

	/* Properties */
	g_object_class_install_property
		(object_class, PROP_CONNECT_SCRIPT,
		 g_param_spec_string (NM_SETTING_ACTIONS_CONNECT_SCRIPT,
						  "Connect script",
						  "The shell script to run on connect as $USER",
						  NULL,
						  G_PARAM_READWRITE | NM_SETTING_PARAM_SERIALIZE));

	g_object_class_install_property
		(object_class, PROP_DISCONNECT_SCRIPT,
		 g_param_spec_string (NM_SETTING_ACTIONS_DISCONNECT_SCRIPT,
						  "Disconnect script",
						  "The shell script to run on disconnect as $USER",
						  NULL,
						  G_PARAM_READWRITE | NM_SETTING_PARAM_SERIALIZE));
}
