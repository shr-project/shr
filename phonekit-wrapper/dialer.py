import dbus, dbus.glib, dbus.service
import gobject

class Dialer (dbus.service.Object):

	_voiceCall = "voice"
	
	fsoCallBus = None

	idCall = -1

	outgoing = False
	
	def __init__(self, bus, object_path):
		dbus.service.Object.__init__(self, bus, object_path)
		fsoCallBus = dbus.Interface(bus.get_object('org.freesmartphone.ogsmd', '/org/freesmartphone/GSM/Device'), 'org.freesmartphone.GSM.Call')
		fsoCallBus.connect_to_signal('CallStatus',receiveSignal)

	def receiveSignal(self, id, status, properties):
		if(idCall == id):
			if(status == 'incoming'):
				self.IncomingCall(None)
			elif(status == 'outgoing'):
				self.OutgoingCall(None)
			elif(status == 'active'):
				outgoing = False
				self.Talking()
			elif(status == 'held'):
				print 'held state'
			elif(status == 'released'):
				if(outgoing):
					self.Rejected()
				else:
					self.HungUp()
			self.StatusChanged(-1)
		
	@dbus.service.signal(dbus_interface='org.openmoko.PhoneKit.Dialer', signature='')
	def Talking (self):
		print 'Dummy Talking'

	@dbus.service.signal(dbus_interface='org.openmoko.PhoneKit.Dialer', signature='')
	def HungUp (self):
		print 'Dummy HungUp'
	
	@dbus.service.signal(dbus_interface='org.openmoko.PhoneKit.Dialer', signature='i')
	def StatusChanged (self, i):
		print 'Dummy StatusChanged %i' % i
	
	@dbus.service.signal(dbus_interface='org.openmoko.PhoneKit.Dialer', signature='')
	def Rejected (self):
		print 'Dummy Rejected'

	@dbus.service.signal(dbus_interface='org.openmoko.PhoneKit.Dialer', signature='s')
	def OutgoingCall (self, s):
		 print 'Dummy Outgoing %s' % s

	@dbus.service.signal(dbus_interface='org.openmoko.PhoneKit.Dialer', signature='s')
	def IncomingCall (self, s):
		print 'Dummy Incoming %s' % s

	@dbus.service.method(dbus_interface='org.openmoko.PhoneKit.Dialer', in_signature='s', out_signature='')
	def HangUp (self, s):
		print 'Dummy HangUp %s' % s
		if(idCall != -1):
			fsoCallBus.Release('Release',idCall)

	@dbus.service.method(dbus_interface='org.openmoko.PhoneKit.Dialer', in_signature='s', out_signature='')
	def Dial (self, number): 
		print 'Dummy Dial %s' % number
		idCall = fsoCallBus.Dial(number, _voiceCall)
		outgoing = True

