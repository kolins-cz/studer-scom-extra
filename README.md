# Studer232-to-MQTT

This program connects to a Studer XCom-232i device, retrieves data, and publishes them to an MQTT broker.

### Usage
- Look at `main.h` set up your MQTT server and feel free to add or remove parameters you would like to retrieve.
- Compile with `make` and you can optionally install a systemd service with `./install-service.sh`
- `homeassistant.yaml` and `lovelace.yaml` are provided to simplify adding this to Home Assistant.

### Contributing

If you would like to contribute to the code, feel free to submit pull requests to improve it.

### Reference

This program is based on the [k3a/studer](https://github.com/k3a/studer) library.