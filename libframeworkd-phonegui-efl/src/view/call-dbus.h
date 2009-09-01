#ifndef _CALL_DBUS_H
#define _CALL_DBUS_H

#include "call-common.h"
#include <frameworkd-glib/odeviced/audio.h> 
#include <dbus/dbus-glib.h>
#include <glib.h>

#define ODEVICED_BUS "org.freesmartphone.odeviced"
#define DEVICE_AUDIO_PATH "/org/freesmartphone/Device/Audio" 
#define DEVICE_AUDIO_IFACE "org.freesmartphone.Device.Audio" 

#define DEVICE_VIBRATOR_PATH "/org/freesmartphone/Device/LED/neo1973_vibrator"
#define DEVICE_VIBRATOR_IFACE "org.freesmartphone.Device.LED" 

static CallSoundMode sound_mode = CALL_SOUND_MODE_ACTIVE;
static  char * ringtone_name = "/usr/share/sounds/ringtone_ringnroll.wav";
static  int sound_lenght = 5;
static  int on_duration = 200;
static  int off_duration = 800;

DBusGConnection* init_g_connection();
int init_proxy(DBusGConnection* bus, 
               DBusGProxy** proxy ,
               char*  bus_name, 
               char* object_path, 
               char* iface);

#endif
