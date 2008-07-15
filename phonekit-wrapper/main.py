import dbus, dbus.glib, dbus.service
import gobject
from network import Network
from sms import SMS
from dialer import Dialer

bus = dbus.SystemBus()
name = dbus.service.BusName('org.openmoko.PhoneKit', bus=bus)
objDialer = Dialer(bus, '/org/openmoko/PhoneKit/Dialer')
objNetwork = Network(bus, '/org/openmoko/PhoneKit/Network')
objSMS = SMS(bus, '/org/openmoko/PhoneKit/SMS')
loop = gobject.MainLoop()
print 'Listening'
loop.run()
