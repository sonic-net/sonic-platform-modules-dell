#!/bin/bash
PLATFORM=dell_broadcom
IMAGE=image-HEAD

#remove older image
rm -f target/${PLATFORM}_8GB_dd.img.gz

echo -e "\n# Creating ${PLATFORM}_8GB_dd.img..."
fallocate -l 8G ${PLATFORM}_8GB_dd.img
mkfs.ext4 ${PLATFORM}_8GB_dd.img

echo -e "\n# Mounting ${PLATFORM}_8GB_dd.img on ${PLATFORM}_mount..."
mkdir ${PLATFORM}_mount
mount -t auto -o loop ${PLATFORM}_8GB_dd.img ${PLATFORM}_mount

echo -e "\n# Extracting tarballs into ${PLATFORM}_mount..."
mkdir -p ${PLATFORM}_mount/${IMAGE}/docker
unzip -o fs.zip -x dockerfs.tar.gz -d ${PLATFORM}_mount/${IMAGE}
unzip -op fs.zip dockerfs.tar.gz | tar xz --numeric-owner -f - -C ${PLATFORM}_mount/${IMAGE}/docker

echo -e "\n# Creating 4GB logger..."
mkdir -p ${PLATFORM}_mount/disk-img
dd if=/dev/zero of=${PLATFORM}_mount/disk-img/var-log.ext4 count=8388608
mkfs.ext4 -q ${PLATFORM}_mount/disk-img/var-log.ext4 -F

echo -e "\n# Unmounting and removing ${PLATFORM}_mount..."
umount ${PLATFORM}_mount
rmdir ${PLATFORM}_mount

echo -e "\n# Creating ${PLATFORM}_8GB_dd.img.gz..."
gzip ${PLATFORM}_8GB_dd.img
mv ${PLATFORM}_8GB_dd.img.gz target
echo -e "\n# Done. DD image is under target/${PLATFORM}_8GB_dd.img.gz\n"

