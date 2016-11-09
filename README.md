=======================================================
To make project
=======================================================

install some libraries (root privilege is needed):
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-ttf-dev
sudo apt-get install libsdl2-gfx-dev
sudo apt-get install libxml2-dev
sudo apt-get install libftdi1-dev

and just run make:
make


=======================================================
To add access to device from user level
=======================================================

create file (root privilege is needed): 
/etc/udev/rules.d/99-vector-dscope.rules

add line ti it: 
SUBSYSTEM=="usb",ENV{DEVTYPE}=="usb_device",ATTRS{idVendor}=="eda3",ATTRS{idProduct}=="2179",MODE:="0666"

=======================================================
And then just run application
=======================================================

./host_app
