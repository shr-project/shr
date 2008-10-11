MAINTAINERCLEANFILES = Makefile.in

filesdir = $(datadir)/libframeworkd-phonegui-efl/
files_DATA = sim-auth.edj incoming-call.edj outgoing-call.edj dialer.edj messages.edj contacts.edj incoming-message.edj

EXTRA_DIST = sim-auth.edc incoming-call.edc outgoing-call.edc dialer.edc messages.edc contacts.edc incoming-message.edc

clean-local:
	rm -f *.edj

sim-auth.edj:
	@edje_cc@ sim-auth.edc

incoming-call.edj:
	@edje_cc@ incoming-call.edc

outgoing-call.edj:
	@edje_cc@ outgoing-call.edc

dialer.edj:
	@edje_cc@ dialer.edc

messages.edj:
	@edje_cc@ messages.edc

contacts.edj:
	@edje_cc@ contacts.edc

incoming-message.edj:
	@edje_cc@ incoming-message.edc

