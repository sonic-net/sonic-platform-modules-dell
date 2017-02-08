#!/bin/bash

insmod ./dell_mailbox.ko
insmod ./dell_z9100_cpld.ko

#platform init script for Dell S6100

#call iom_power.sh here from obsolute path on target
#/usr/bin/iom_power.sh
#load kernel modules 
#modprobe i2c-mux-pca954x.ko
#modprobe dell_s6100_iom_cpld.ko 

# Attach Ox70 IOM mux's
echo pca9547 0x70 > /sys/bus/i2c/devices/i2c-2/new_device
sleep 2
# Attach 0x71 for iom  cpld's
echo pca9548 0x71 > /sys/bus/i2c/devices/i2c-5/new_device
sleep 2


#Attach cpld devices to drivers for each iom
for ((i=15;i<=18;i++));
do
echo  dell_z9100_iom_cpld 0x3e > /sys/bus/i2c/devices/i2c-$i/new_device 
sleep 2
done

for ((i=7;i<=10;i++));
do
# 0x71 mux on the IOM 1
echo "Attaching PCA9548 for IOM-$i"
echo pca9548 0x71 > /sys/bus/i2c/devices/i2c-$i/new_device
sleep 2
done

# Attach the SFP modules on PCA9548_2
echo sff8436 0x50 > /sys/bus/i2c/devices/i2c-12
echo sff8436 0x50 > /sys/bus/i2c/devices/i2c-13

# Attach 32 instances of EEPROM driver QSFP ports on IO module 1
#eeprom can dump data using below command
for ((i=19;i<=50;i++));
do
echo sff8436 0x50 > /sys/bus/i2c/devices/i2c-$i/new_device 
sleep 2
done

