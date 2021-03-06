/*
 *  Copyright (C) 2009
 *      Authors (alphabetical) :
 *              Klaus 'mrmoku' Kurzmann <mok@fluxnetz.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Public License as published by
 *  the Free Software Foundation; version 2 of the license or any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 */

#ifndef _PHONEUID_SETTINGS_H
#define _PHONEUID_SETTINGS_H

#define PHONEUID_TYPE_SETTINGS_SERVICE            (phoneuid_settings_service_get_type ())
#define PHONEUID_SETTINGS_SERVICE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), PHONEUID_TYPE_SETTINGS_SERVICE, PhoneuidSettingsService))
#define PHONEUID_SETTINGS_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PHONEUID_TYPE_SETTINGS_SERVICE, PhoneuidSettingsServiceClass))
#define PHONEUID_IS_SETTINGS_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), PHONEUID_TYPE_SETTINGS_SERVICE))
#define PHONEUID_IS_SETTINGS_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PHONEUID_TYPE_SETTINGS_SERVICE))
#define PHONEUID_SETTINGS_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PHONEUID_TYPE_SETTINGS_SERVICE, PhoneuidSettingsServiceClass))


typedef struct _PhoneuidSettingsService PhoneuidSettingsService;
typedef struct _PhoneuidSettingsServiceClass PhoneuidSettingsServiceClass;

GType phoneuid_settings_service_get_type(void);

struct _PhoneuidSettingsService {
	GObject parent;
};

struct _PhoneuidSettingsServiceClass {
	GObjectClass parent;
	DBusGConnection *connection;
};

gboolean
phoneuid_settings_service_display_quick_settings(PhoneuidSettingsService *object,
		DBusGMethodInvocation *context);

gboolean
phoneuid_settings_service_display_sim_manager(PhoneuidSettingsService *object,
		DBusGMethodInvocation *context);

PhoneuidSettingsService *phoneuid_settings_service_new(void);


#endif

