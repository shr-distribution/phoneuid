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

#ifndef _PHONEUID_CONTACTS_H
#define _PHONEUID_CONTACTS_H

#define PHONEUID_TYPE_CONTACTS_SERVICE            (phoneuid_contacts_service_get_type ())
#define PHONEUID_CONTACTS_SERVICE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), PHONEUID_TYPE_CONTACTS_SERVICE, PhoneuidContactsService))
#define PHONEUID_CONTACTS_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PHONEUID_TYPE_CONTACTS_SERVICE, PhoneuidContactsServiceClass))
#define PHONEUID_IS_CONTACTS_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), PHONEUID_TYPE_CONTACTS_SERVICE))
#define PHONEUID_IS_CONTACTS_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PHONEUID_TYPE_CONTACTS_SERVICE))
#define PHONEUID_CONTACTS_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PHONEUID_TYPE_CONTACTS_SERVICE, PhoneuidContactsServiceClass))


typedef struct _PhoneuidContactsService PhoneuidContactsService;
typedef struct _PhoneuidContactsServiceClass PhoneuidContactsServiceClass;

GType phoneuid_contacts_service_get_type(void);

struct _PhoneuidContactsService {
	GObject parent;
};

struct _PhoneuidContactsServiceClass {
	GObjectClass parent;
	DBusGConnection *connection;
};

gboolean
phoneuid_contacts_service_display_list(PhoneuidContactsService *object,
		GHashTable *filter, DBusGMethodInvocation *context);
gboolean
phoneuid_contacts_service_display_contact(PhoneuidContactsService *object,
		const char *contact_path, DBusGMethodInvocation *context);
gboolean
phoneuid_contacts_service_create_contact(PhoneuidContactsService *object,
		GHashTable *values, DBusGMethodInvocation *context);
gboolean
phoneuid_contacts_service_edit_contact(PhoneuidContactsService *object,
		const char *contact_path, DBusGMethodInvocation *context);



PhoneuidContactsService *phoneuid_contacts_service_new(void);



#endif
