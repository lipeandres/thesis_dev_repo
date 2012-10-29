#!/bin/bash
cp ../fs_fixes/profile ./output/target/etc/profile
cp ../fs_fixes/fstab ./output/target/etc/fstab
cp ../fs_fixes/usbfix ./output/target/usr/bin/usbfix
ln -s ./output/target/mnt ./output/root/sdcard
