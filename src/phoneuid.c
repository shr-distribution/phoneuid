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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <time.h>
#include <sys/time.h>
#include <phoneui/phoneui.h>

#include "phoneuid-call-management.h"
#include "phoneuid-dialer.h"
#include "phoneuid-notification.h"
#include "phoneuid-contacts.h"
#include "phoneuid-messages.h"
#include "phoneuid-dbus-common.h"
#include "phoneuid-settings.h"
#include "phoneuid-idle-screen.h"
#include "phoneuid-phone-log.h"
#include "phoneuid-calendar.h"

static FILE *logfile = NULL;
/*FIXME: hardcoded, should change */
#define CONF_FILE "/etc/phoneuid.conf"
#define LOGFILE "/var/log/phoneuid.log"
#define DEFAULT_DEBUG_LEVEL "INFO"
static GLogLevelFlags log_flags;

static void
_log_handler(const gchar *domain, GLogLevelFlags level, const gchar *message,
		gpointer userdata)
{
	char date_str[30];
	struct timeval tv;
	struct tm ptime;
	char *levelstr;
	(void) userdata;
	if (!(log_flags & G_LOG_LEVEL_MASK & level)) {
		return;
	}
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &ptime);

	strftime(date_str, 30, "%Y.%m.%d %T", &ptime);

	switch (level) {
	case G_LOG_LEVEL_ERROR:
		levelstr = "ERROR";
		break;
	case G_LOG_LEVEL_CRITICAL:
		levelstr = "CRITICAL";
		break;
	case G_LOG_LEVEL_WARNING:
		levelstr = "WARNING";
		break;
	case G_LOG_LEVEL_MESSAGE:
		levelstr = "MESSAGE";
		break;
	case G_LOG_LEVEL_INFO:
		levelstr = "INFO";
		break;
	case G_LOG_LEVEL_DEBUG:
		levelstr = "DEBUG";
		break;
	default:
		levelstr = "";
		break;
	}

	fprintf(logfile, "%s.%06d [%s]\t%s: %s\n", date_str, (int) tv.tv_usec,
			domain, levelstr, message);
	fflush(logfile);
}

static void
_load_config()
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;

	GError *error = NULL;
	int failed = 0;
	char *logpath = NULL;
	char *debug_level = NULL;

	/* Read the phoneuid preferences */
	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
	if (g_key_file_load_from_file
	    (keyfile, CONF_FILE, flags, &error)) {
		logpath = g_key_file_get_string(keyfile, "logging",
					"log_file", NULL);

		debug_level = g_key_file_get_string(keyfile, "logging",
					"log_level", NULL);
		/* initialize logging */
	}
	else {
		failed = 1;
	}
	debug_level = (debug_level) ? debug_level : DEFAULT_DEBUG_LEVEL;
	logpath = (logpath) ? logpath : LOGFILE;
	printf("Log file: %s\nLog level: %s\n", logpath, debug_level);

	logfile = fopen(logpath, "a");
	if (!logfile) {
		printf("Error creating the logfile (%s) !!!", logpath);
		g_log_set_default_handler(g_log_default_handler, NULL);
	}

	log_flags = G_LOG_FLAG_FATAL;

	if (!strcmp(debug_level, "DEBUG")) {
		log_flags |= G_LOG_LEVEL_MASK;
	}
	else if (!strcmp(debug_level, "INFO")) {
		log_flags |= G_LOG_LEVEL_MASK ^ (G_LOG_LEVEL_DEBUG);
	}
	else if (!strcmp(debug_level, "MESSAGE")) {
		log_flags |= G_LOG_LEVEL_MASK ^ (G_LOG_LEVEL_DEBUG
			| G_LOG_LEVEL_INFO);
	}
	else if (!strcmp(debug_level, "WARNING")) {
		log_flags |= G_LOG_LEVEL_MASK ^ (G_LOG_LEVEL_DEBUG
			| G_LOG_LEVEL_INFO | G_LOG_LEVEL_MESSAGE);
	}
	else if (!strcmp(debug_level, "CRITICAL")) {
		log_flags |= G_LOG_LEVEL_MASK ^ (G_LOG_LEVEL_DEBUG
			| G_LOG_LEVEL_INFO | G_LOG_LEVEL_MESSAGE
			| G_LOG_LEVEL_WARNING);
	}
	else if (!strcmp(debug_level, "ERROR")) {
		log_flags |= G_LOG_LEVEL_MASK ^ (G_LOG_LEVEL_DEBUG
			| G_LOG_LEVEL_INFO | G_LOG_LEVEL_MESSAGE
			| G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);
	}
	else {
	}


	if (failed) {
		g_warning("Error reading configuration file: %s", error->message);
		g_debug("Reading configuration file error, skipping");
	}
	else {
		g_debug("Configuration file read");
		g_message("Using log level '%s'", debug_level);
	}
}

/* not used atm */
#if 0
static gpointer
dbus_register_object(DBusGConnection * connection,
		     DBusGProxy * proxy,
		     GType object_type,
		     const DBusGObjectInfo * info, const gchar * path)
{
	(void) proxy;
	GObject *object = g_object_new(object_type, NULL);
	dbus_g_object_type_install_info(object_type, info);
	dbus_g_connection_register_g_object(connection, path, object);
	return object;
}
#endif

static void
phoneuid_dbus_setup()
{
	DBusGConnection *bus;
	DBusGProxy *driver_proxy;
	GError *error = NULL;
	unsigned int request_ret;

	/* Register the service name, the constant here are defined in dbus-glib-bindings.h */
	bus = dbus_g_bus_get (DBUS_BUS_SYSTEM, &error);
	driver_proxy = dbus_g_proxy_new_for_name (bus,
						  DBUS_SERVICE_DBUS,
						  DBUS_PATH_DBUS,
						  DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (driver_proxy,
			PHONEUID_SERVICE, 0, &request_ret, &error)) {
		g_warning("Unable to register service: %s", error->message);
		g_error_free (error);
	}
	g_object_unref(driver_proxy);

	phoneuid_call_management_service_new();
	phoneuid_dialer_service_new();
	phoneuid_notification_service_new();
	phoneuid_contacts_service_new();
	phoneuid_messages_service_new();
	phoneuid_settings_service_new();
	phoneuid_idle_screen_service_new();
	phoneuid_phone_log_service_new();
	phoneuid_calendar_service_new();
}


int
main(int argc, char **argv)
{
	g_log_set_fatal_mask(NULL, G_LOG_LEVEL_ERROR);
	g_log_set_default_handler(_log_handler, NULL);
	_load_config();
	g_type_init();

	if (!g_thread_supported())
		g_thread_init(NULL);
	dbus_g_thread_init();

	phoneui_load("phoneuid");
	phoneui_init(argc, argv, NULL);
	phoneuid_dbus_setup();
	phoneui_loop();

	phoneui_deinit();
	g_debug("exited from phoneui_loop!");

	return 0;
}


