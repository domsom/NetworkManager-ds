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
 * (C) Copyright 2005 - 2010 Red Hat, Inc.
 */

#ifndef NM_ACTIVATION_REQUEST_H
#define NM_ACTIVATION_REQUEST_H

#include <glib.h>
#include <glib-object.h>
#include "nm-connection.h"
#include "nm-active-connection.h"
#include "nm-settings-flags.h"

#define NM_TYPE_ACT_REQUEST            (nm_act_request_get_type ())
#define NM_ACT_REQUEST(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NM_TYPE_ACT_REQUEST, NMActRequest))
#define NM_ACT_REQUEST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NM_TYPE_ACT_REQUEST, NMActRequestClass))
#define NM_IS_ACT_REQUEST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NM_TYPE_ACT_REQUEST))
#define NM_IS_ACT_REQUEST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), NM_TYPE_ACT_REQUEST))
#define NM_ACT_REQUEST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), NM_TYPE_ACT_REQUEST, NMActRequestClass))

typedef enum {
	NM_ACT_REQUEST_DEP_RESULT_UNKNOWN,
	NM_ACT_REQUEST_DEP_RESULT_WAIT,
	NM_ACT_REQUEST_DEP_RESULT_READY,
	NM_ACT_REQUEST_DEP_RESULT_FAILED,
} NMActRequestDependencyResult;

#define NM_ACT_REQUEST_DEPENDENCY_RESULT "dependency-result"

typedef struct {
	GObject parent;
} NMActRequest;

typedef struct {
	GObjectClass parent;

	/* Signals */
	void (*properties_changed) (NMActRequest *req, GHashTable *properties);

	void (*dependency_result) (NMActRequest *req, NMActRequestDependencyResult result);
} NMActRequestClass;

GType nm_act_request_get_type (void);

NMActRequest *nm_act_request_new          (NMConnection *connection,
                                           const char *specific_object,
                                           gboolean user_requested,
                                           gulong user_uid,
                                           gboolean assumed,
                                           gpointer *device,  /* An NMDevice */
                                           NMActiveConnection *dependency);

NMConnection *nm_act_request_get_connection     (NMActRequest *req);

gboolean      nm_act_request_get_user_requested (NMActRequest *req);

gboolean      nm_act_request_get_shared (NMActRequest *req);

void          nm_act_request_set_shared (NMActRequest *req, gboolean shared);

void          nm_act_request_add_share_rule (NMActRequest *req,
                                             const char *table,
                                             const char *rule);

GObject *     nm_act_request_get_device (NMActRequest *req);

gboolean      nm_act_request_get_assumed (NMActRequest *req);

NMActiveConnection *         nm_act_request_get_dependency (NMActRequest *req);

NMActRequestDependencyResult nm_act_request_get_dependency_result (NMActRequest *req);

/* Secrets handling */

typedef void (*NMActRequestSecretsFunc) (NMActRequest *req,
                                         guint32 call_id,
                                         NMConnection *connection,
                                         GError *error,
                                         gpointer user_data);

guint32 nm_act_request_get_secrets (NMActRequest *req,
                                    const char *setting_name,
                                    NMSettingsGetSecretsFlags flags,
                                    const char *hint,
                                    NMActRequestSecretsFunc callback,
                                    gpointer callback_data);

void nm_act_request_cancel_secrets (NMActRequest *req, guint32 call_id);

#endif /* NM_ACTIVATION_REQUEST_H */

