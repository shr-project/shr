import sys
import os

os.system("edje_cc -id ./data/image ./data/themes/exposure.edc")

from ez_setup import use_setuptools
use_setuptools('0.6c3')

from setuptools import setup, find_packages, Extension
from distutils.sysconfig import get_python_inc
from glob import glob
import commands


dist = setup( name='exposure',
    version='0.2',
    author='Marek',
    author_email='marek@openmoko.com',
    description='Users can configure the system behaviour',
    url='http://projects.openmoko.org/projects/exposure/',
    download_url='http://projects.openmoko.org/scm/?group_id=135',
    license='GNU GPL',
    package_dir={'':'src'},
    packages=['expos'],
    scripts=['src/exposure.py'],
    install_requires=['python-etk >= 0.1.1'],
    setup_requires=['python-evas>=0.2.1'],
    data_files=[('applications', ['src/exposure.desktop']),
	('exposure', ['data/themes/exposure.edj']),
	('images', ['data/image/exposure.png']),
	('/etc/X11/Xsession.d/', ['src/80app-launcher-bootup']),
        ]
)

installCmd = dist.get_command_obj(command="install_data")
installdir = installCmd.install_dir
installroot = installCmd.root

if not installroot:
    installroot = ""

if installdir:
    installdir = os.path.join(os.path.sep,
        installdir.replace(installroot, ""))

