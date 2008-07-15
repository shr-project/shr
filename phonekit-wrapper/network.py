import dbus, dbus.glib, dbus.service
import gobject

class Network (dbus.service.Object):
	
	fsoNetworkBus = None

	def __init__(self, bus, object_path):
		dbus.service.Object.__init__(self, bus, object_path)
		fsoCallBus = dbus.Interface(bus.get_object('org.freesmartphone.ogsmd', '/org/freesmartphone/GSM/Device'),
				'org.freesmartphone.GSM.Call')
		fsoCallBus.connect_to_signal('CallStatus',receiveSignal)
		@dbus.service.signal(dbus_interface='org.openmoko.PhoneKit.Network', signature='i')

	def StatusChanged (self, i):
		print 'Dummy StatusChanged %i' % i
	
	@dbus.service.signal(dbus_interface='org.openmoko.PhoneKit.Network', signature='s')
	def ProviderChanged(self,s):
		print 'Dummy ProviderChanged %s' % s

	@dbus.service.signal(dbus_interface='org.openmoko.PhoneKit.Network', signature='s')
	def SubscriberNumberChanged (self, s):
		 print 'Dummy SubscriberNumberChanged %s' % s

	@dbus.service.method(dbus_interface='org.openmoko.PhoneKit.Network', in_signature='', out_signature='s')
	def GetHomeCountryCode (self):
		print 'Dummy GetHomeCountryCode'
		return 'Dummy GetHomeCountryCode'

	@dbus.service.method(dbus_interface='org.openmoko.PhoneKit.Network', in_signature='', out_signature='s')
	def GetCountryCode (self):
		print 'Dummy GetCountryCode'
		return 'Dummy GetCountryCode'

	@dbus.service.method(dbus_interface='org.openmoko.PhoneKit.Network', in_signature='', out_signature='s')
	def GetSubscriberNumber (self):
		print 'Dummy GetSubscriberNumber'
		return 'Dummy GetSubscriberNumber'

	@dbus.service.method(dbus_interface='org.openmoko.PhoneKit.Network', in_signature='', out_signature='s')
	def GetProviderName (self):
		print 'Dummy GetProviderName'
		return 'Dummy GetProviderName'
