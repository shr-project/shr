#!/bin/sh
case "$1" in
    start)
       if [ -f /usr/share/shr-splash/theme/boot.fb ]; then
         (cat /usr/share/shr-splash/theme/boot.fb > /dev/fb0) &
       else
         ([ -f /usr/share/shr-splash/theme/boot.fbz ] && bzcat /usr/share/shr-splash/theme/boot.fbz > /dev/fb0) &
       fi
    ;;
    stop)
       if [ -f /usr/share/shr-splash/theme/shutdown.fb ]; then
         (cat /usr/share/shr-splash/theme/shutdown.fb > /dev/fb0) &
       else
         ([ -f /usr/share/shr-splash/theme/shutdown.fbz ] && bzcat /usr/share/shr-splash/theme/shutdown.fbz > /dev/fb0) &
       fi
    ;;
esac

exit 0
