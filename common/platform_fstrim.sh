#!/bin/bash

# Enable/Disable fstrim service for the platform

if [[ "$1" == "init" ]]; then
    # Enable fstrim after 3 minutes of uptime
    sleep 180
    systemctl start fstrim.timer
    systemctl start fstrim.service
elif [[ "$1" == "deinit" ]]; then
    # Disable fstrim
    systemctl stop fstrim.service
    systemctl stop fstrim.timer
else
    echo "platform_fstrim : Invalid option : $1!"
    exit 1
fi

exit 0
