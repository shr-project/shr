# A shell check command that I find convenient:
# ls -1 *.png | xargs --replace=pngimg sh -c '(grep -qw "pngimg" Makefile.am  || echo "Missing pngimg")'

#dist_pkgdata_DATA = sim-auth.edj \
#                    incoming-call.edj \
#                    outgoing-call.edj \
#                    dialer.edj \
#                    messages.edj \
#                    contacts.edj \
#                    incoming-message.edj

install:
	install -m 755 -d $(DESTDIR)/etc/ophonekitd/
	install -m 644 gui.conf $(DESTDIR)/etc/ophonekitd/gui.conf

# Manual install instead of dist_pkgdata_DATA, because install: overwrites dist_pkgdata_DATA!
	install -m 755 -d $(DESTDIR)/usr/share/libframeworkd-phonegui-efl/ 
	install -m 644 sim-auth.edj $(DESTDIR)/usr/share/libframeworkd-phonegui-efl/ 
	install -m 644 incoming-call.edj $(DESTDIR)/usr/share/libframeworkd-phonegui-efl/ 
	install -m 644 outgoing-call.edj $(DESTDIR)/usr/share/libframeworkd-phonegui-efl/ 
	install -m 644 dialer.edj $(DESTDIR)/usr/share/libframeworkd-phonegui-efl/ 
	install -m 644 messages.edj $(DESTDIR)/usr/share/libframeworkd-phonegui-efl/ 
	install -m 644 contacts.edj $(DESTDIR)/usr/share/libframeworkd-phonegui-efl/ 
	install -m 644 incoming-message.edj $(DESTDIR)/usr/share/libframeworkd-phonegui-efl/ 
