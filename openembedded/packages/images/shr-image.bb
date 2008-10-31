#------------------------------------------------------
# SHR Image Recipe
#------------------------------------------------------

IMAGE_LINGUAS = ""

# getting the base system up
BASE_INSTALL = "\
  ${MACHINE_TASK_PROVIDER} \
  netbase \
  sysfsutils \
  module-init-tools-depmod \
  rsync \
  screen \
  fbset \
  fbset-modes \
  fso-gpsd \
  gsm0710muxd \
  exquisite \
  exquisite-themes \
  exquisite-theme-illume \
"
# Some machines don't set a *runtime* provider for X, so default to Xfbdev here
# virtual/xserver won't work, since the kdrive recipes will build multiple xserver packages
XSERVER ?= "xserver-kdrive-fbdev"

# getting an X window system
X_INSTALL = "\
  glibc-utils \
  glibc-charmap-utf-8 \
  glibc-binary-localedata-en-us \
  glibc-binary-localedata-de-de \
  glibc-binary-localedata-fr-fr \
  glibc-binary-localedata-pt-br \
  glibc-binary-localedata-ca-es \
  glibc-binary-localedata-zh-cn \
  glibc-binary-localedata-zh-tw \
  glibc-binary-localedata-bg-bg \
  glibc-binary-localedata-cs-cz \
  glibc-binary-localedata-da-dk \
  glibc-binary-localedata-nl-nl \
  glibc-binary-localedata-fi-fi \
  glibc-binary-localedata-hu-hu \
  glibc-binary-localedata-it-it \
  glibc-binary-localedata-ja-jp \
  glibc-binary-localedata-ko-kr \
  glibc-binary-localedata-nb-no \
  glibc-binary-localedata-pl-pl \
  glibc-binary-localedata-ru-ru \
  glibc-binary-localedata-sk-sk \
  glibc-binary-localedata-sl-si \
  glibc-binary-localedata-es-ar \
  glibc-binary-localedata-sv-se \
  e-wm \
  e-wm-config-illume \
  e-wm-config-standard \
  e-wm-config-netbook \
  e-wm-config-minimalist \
  e-wm-config-scaleable \
  ${XSERVER} \
  xserver-kdrive-splash-illume \
  xserver-kdrive-common \
  xserver-nodm-init \
  xauth \
  xhost \
  xset \
  xrandr \
  fontconfig-utils \
  ttf-dejavu-common \
  ttf-dejavu-sans \
  ttf-dejavu-sans-mono \
  ttf-arphic-uming \
"

X_INSTALL_append_om-gta02 = "\
  ttf-arphic-uming \
"

# useful command line tools
TOOLS_INSTALL = "\
#  bash \
  dosfstools \
  htop \
  iptables \
  lsof \
  mickeydbus \
  mickeyterm \
  mtd-utils \
  nano \
  powertop \
  s3c24xx-gpio \
  sysstat \
  tcpdump \
"

# audio
AUDIO_INSTALL = "\
  alsa-oss \
  alsa-state \
  alsa-utils-aplay \
  alsa-utils-amixer \
  gst-meta-audio \
#  gst-plugin-mad \
  gst-plugin-modplug \
  gst-plugin-sid \
  fso-sounds \
"

GTK_INSTALL = "\
  openmoko-calculator2 \
  openmoko-terminal2 \
  gpe-scap \
  tangogps \
"

GAMES_INSTALL = "\
  numptyphysics \
"

# FIXME these should rather be part of alsa-state,
# once Om stabilizes them...
AUDIO_INSTALL_append_om-gta01 = "\
  openmoko-alsa-scenarios \
"
AUDIO_INSTALL_append_om-gta02 = "\
  openmoko-alsa-scenarios \
"

# python
PYTHON_INSTALL = "\
  task-python-efl \
  python-codecs \
  python-gst \
"

# SHR
SHR_INSTALL = "\
  frameworkd \
  libframeworkd-glib \
  openmoko-dialer3 \
  openmoko-messages3 \
  openmoko-contacts3 \
  ophonekitd \ 
  libframeworkd-phonegui-efl \
"
# zhone should be added, but a custom package without the autostart script
# removed libframeworkd-phonegui-gtk from SHR_INSTALL

IMAGE_INSTALL = "\
  ${BASE_INSTALL} \
  ${X_INSTALL} \
  ${GTK_INSTALL} \
  ${GAMES_INSTALL} \
  ${AUDIO_INSTALL} \
  ${TOOLS_INSTALL} \
  ${PYTHON_INSTALL} \
  ${SHR_INSTALL} \
"

inherit image

# perform some convenience tweaks to the rootfs
fso_rootfs_postprocess() {
    curdir=$PWD
    cd ${IMAGE_ROOTFS}
    # date/time
    date "+%m%d%H%M%Y" >./etc/timestamp
    # alias foo
    echo "alias pico=nano" >>./etc/profile
    echo "alias fso='cd /local/pkg/fso'" >>./etc/profile
    echo "alias ipkg='opkg'" >>./etc/profile
    # nfs
    mkdir -p ./local/pkg
    echo >>./etc/fstab
    echo "# NFS Host" >>./etc/fstab
    echo "192.168.0.200:/local/pkg /local/pkg nfs noauto,nolock,soft,rsize=32768,wsize=32768 0 0" >>./etc/fstab
    # fix .desktop files for illume
    desktop=`find ./usr/share/applications -name "*.desktop"`
    for file in $desktop; do
        echo "Categories=Office;" >>$file
    done
    # minimal gtk theme foo
    mkdir -p ./etc/gtk-2.0/
    echo 'gtk-font-name = "Sans 5"' >> ./etc/gtk-2.0/gtkrc
    # fix strange iconv/gconf bug
    ln -s libc.so.6 ./lib/libc.so
    cd $curdir
}

# perform some SHR convenience tweaks to the rootfs
shr_rootfs_postprocess() {
    dirs=`find ${FILESDIR} -type d -printf "%P\n" | grep -v "^.$" | grep -v ".svn"`
    for dir in $dirs; do
        mkdir -p ${IMAGE_ROOTFS}/$dir
    done
    files=`find ${FILESDIR} -type f -printf "%P\n" | grep -v ".svn"`
    for file in $files; do
        cp -f ${FILESDIR}/$file ${IMAGE_ROOTFS}/$file
    done

    curdir=$PWD
    cd ${IMAGE_ROOTFS}


    # setup opkg config
    if [ -a ./etc/opkg/all-feed.conf ]; then rm ./etc/opkg/all-feed.conf; fi
    if [ -a ./etc/opkg/armv4t-feed.conf ]; then rm ./etc/opkg/armv4t-feed.conf; fi
    if [ -a ./etc/opkg/fic-gta02-feed.conf ]; then rm ./etc/opkg/fic-gta02-feed.conf; fi
    if [ -a ./etc/opkg/neo1973-feed.conf ]; then rm ./etc/opkg/neo1973-feed.conf; fi
    if [ -a ./etc/opkg/om-gta01-feed.conf ]; then rm ./etc/opkg/om-gta01-feed.conf; fi
    if [ -a ./etc/opkg/om-gta02-feed.conf ]; then rm ./etc/opkg/om-gta02-feed.conf; fi

    # set openmoko theme
    mkdir -p ./etc/gtk-2.0/
    echo 'gtk-theme-name = "openmoko-standard-2"' >> ./etc/gtk-2.0/gtkrc
    echo 'gtk-icon-theme-name = "openmoko-standard"' >> ./etc/gtk-2.0/gtkrc

    cd $curdir
}

ROOTFS_POSTPROCESS_COMMAND += "fso_rootfs_postprocess; shr_rootfs_postprocess"
