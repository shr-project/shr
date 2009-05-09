#!/bin/sh
case "$1" in
    start)
       [ -f /usr/share/shr-splash/theme/boot.fb ] && cat /usr/share/shr-splash/theme/boot.fb > /dev/fb0
    ;;
    stop)
       [ -f /usr/share/shr-splash/theme/shutdown.fb ] && cat /usr/share/shr-splash/theme/shutdown.fb > /dev/fb0
    ;;
esac

exit 0
