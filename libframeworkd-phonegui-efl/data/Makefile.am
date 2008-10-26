MAINTAINERCLEANFILES = Makefile.in

filesdir = $(datadir)/libframeworkd-phonegui-efl/
files_DATA = sim-auth.edj dialer.edj messages.edj contacts.edj incoming-message.edj dial-pad.edj call.edj

EXTRA_DIST = sim-auth.edc dialer.edc messages.edc contacts.edc incoming-message.edc dial-pad.edc call.edc

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

dial-pad.edj:
	@edje_cc@ dial-pad.edc

call.edj:
	@edje_cc@ call.edc


