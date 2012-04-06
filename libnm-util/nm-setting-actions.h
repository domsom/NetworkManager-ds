#ifndef NM_SETTING_ACTIONS_H
#define NM_SETTING_ACTIONS_H

#include <NetworkManager.h>
#include <nm-setting.h>

G_BEGIN_DECLS

#define NM_TYPE_SETTING_ACTIONS            (nm_setting_actions_get_type ())
#define NM_SETTING_ACTIONS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NM_TYPE_SETTING_ACTIONS, NMSettingActions))
#define NM_SETTING_ACTIONS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NM_TYPE_SETTING_ACTIONS, NMSettingActionsClass))
#define NM_IS_SETTING_ACTIONS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NM_TYPE_SETTING_ACTIONS))
#define NM_IS_SETTING_ACTIONS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), NM_TYPE_SETTING_ACTIONS))
#define NM_SETTING_ACTIONS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), NM_TYPE_SETTING_ACTIONS, NMSettingActionsClass))

#define NM_SETTING_ACTIONS_SETTING_NAME "actions"

GQuark nm_setting_actions_error_quark (void);

#define NM_SETTING_ACTIONS_CONNECT_SCRIPT    "connect-script"
#define NM_SETTING_ACTIONS_DISCONNECT_SCRIPT "disconnect-script"

typedef struct {
	NMSetting parent;
} NMSettingActions;

typedef struct {
	NMSettingClass parent;

	/* Padding for future expansion */
	void (*_reserved1) (void);
	void (*_reserved2) (void);
	void (*_reserved3) (void);
	void (*_reserved4) (void);
} NMSettingActionsClass;

#define NM_SETTING_ACTIONS_ERROR nm_setting_actions_error_quark ()
GType nm_setting_actions_get_type (void);

NMSetting *nm_setting_actions_new (void);

const char       *nm_setting_actions_get_connect_script    (NMSettingActions *setting);
const char       *nm_setting_actions_get_disconnect_script (NMSettingActions *setting);

G_END_DECLS

#endif /* NM_SETTING_ACTIONS_H */
