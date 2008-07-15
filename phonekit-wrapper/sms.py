import dbus, dbus.glib, dbus.service
import gobject

class SMS (dbus.service.Object):

	@dbus.service.signal(dbus_interface='org.openmoko.PhoneKit.Network', signature='i')
	def StatusChanged (self, i):
		print 'Dummy StatusChanged %i' % i
	
	@dbus.service.signal(dbus_interface='org.openmoko.PhoneKit.Network', signature='b')
	def PhoneMemoryState (self, b):
		print 'Dummy PhoneMemoryState %b' % b
	
	@dbus.service.signal(dbus_interface='org.openmoko.PhoneKit.Network', signature='b')
	def SimMemoryState (self,s):
		print 'Dummy SimMemoryState %b' % b

	@dbus.service.method(dbus_interface='org.openmoko.PhoneKit.Network', in_signature='', out_signature='bb')
	def GetMemoryStatus (self):
		print'Dummy GetMemoryStatus'
		return (false, false)

	@dbus.service.method(dbus_interface='org.openmoko.PhoneKit.Network', in_signature='ssb', out_signature='s')
	def Send (self, number, message, report):
		print 'Dummy Send %s %s %b' % (number, message, report)
		return 'Dummy Uid'
