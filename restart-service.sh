#!/bin/bash

# Restart studer232-to-mqtt service
sudo systemctl restart studer232-to-mqtt

# Show output of studer232-to-mqtt service
sudo journalctl -u studer232-to-mqtt -f