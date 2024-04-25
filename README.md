# Studer232-to-MQTT

This program connects to a Studer XCom-232i device, retrieves data, and publishes them to an MQTT broker.

### Usage
- Look at main.h and feel free to add or remove parameters you would like to retrieve.
- Compile with `make` and you can optinally install a systemd service with  `./install-service.sh`
- `homeassistant.yaml` and `lovelace.yaml` provided to simplify addind this to Homeassistant 

### Contributing

If you would like to contribute to the code, feel free to submit pull requests to improve it.

### Reference

This program is based on the [k3a/studer](https://github.com/k3a/studer) library.