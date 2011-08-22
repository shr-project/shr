#!/bin/sh
case "$1" in
    start)
       # Hide cursor
       echo -n -e "\033[?25l" > /dev/tty0

       if [ -f /usr/share/shr-splash/theme/boot.fb ]; then
         (cat /usr/share/shr-splash/theme/boot.fb > /dev/fb0 2> /dev/null) &
       else
         ([ -f /usr/share/shr-splash/theme/boot.fbz ] && bzcat /usr/share/shr-splash/theme/boot.fbz > /dev/fb0 2> /dev/null) &
       fi
    ;;
    stop)
       if [ -f /usr/share/shr-splash/theme/shutdown.fb ]; then
         (cat /usr/share/shr-splash/theme/shutdown.fb > /dev/fb0 2> /dev/null) &
       else
         ([ -f /usr/share/shr-splash/theme/shutdown.fbz ] && bzcat /usr/share/shr-splash/theme/shutdown.fbz > /dev/fb0 2> /dev/null) &
       fi
    ;;
esac

exit 0
