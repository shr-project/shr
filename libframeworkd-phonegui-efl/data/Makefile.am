MAINTAINERCLEANFILES = Makefile.in

filesdir = $(datadir)/libframeworkd-phonegui-efl/
files_DATA = sim-auth.edj dialer.edj messages.edj contacts.edj incoming-message.edj keypad.edj call.edj dialog.edj ussd.edj

EXTRA_DIST = sim-auth.edc dialer.edc messages.edc contacts.edc incoming-message.edc keypad.edc call.edc dialog.edc ussd.edc

clean-local:
	rm -f *.edj

sim-auth.edj:
	@edje_cc@ sim-auth.edc

dialer.edj:
	@edje_cc@ dialer.edc

messages.edj:
	@edje_cc@ messages.edc

contacts.edj:
	@edje_cc@ contacts.edc

incoming-message.edj:
	@edje_cc@ incoming-message.edc

keypad.edj:
	@edje_cc@ keypad.edc

call.edj:
	@edje_cc@ call.edc

dialog.edj:
	@edje_cc@ dialog.edc

ussd.edj:
	@edje_cc@ ussd.edc

