#ifndef _PHONEUID_DBUS_COMMON_H
#define _PHONEUID_DBUS_COMMON_H

#define PHONEUID_SERVICE                     "org.shr.phoneuid"

#define PHONEUID_CALL_MANAGEMENT_INTERFACE   PHONEUID_SERVICE ".CallManagement"
#define PHONEUID_DIALER_INTERFACE            PHONEUID_SERVICE ".Dialer"
#define PHONEUID_DIALOGS_INTERFACE           PHONEUID_SERVICE ".Dialogs"
#define PHONEUID_CONTACTS_INTERFACE          PHONEUID_SERVICE ".Contacts"
#define PHONEUID_MESSAGES_INTERFACE          PHONEUID_SERVICE ".Messages"
#define PHONEUID_PHONE_LOG_INTERFACE         PHONEUID_SERVICE ".PhoneLog"

#define PHONEUID_CALL_MANAGEMENT_PATH        "/org/shr/phoneuid/CallManagement"
#define PHONEUID_DIALER_PATH                 "/org/shr/phoneuid/Dialer"
#define PHONEUID_DIALOGS_PATH                "/org/shr/phoneuid/Dialogs"
#define PHONEUID_CONTACTS_PATH               "/org/shr/phoneuid/Contacts"
#define PHONEUID_MESSAGES_PATH               "/org/shr/phoneuid/Messages"
#define PHONEUID_PHONE_LOG_PATH              "/org/shr/phoneuid/PhoneLog"

#endif
