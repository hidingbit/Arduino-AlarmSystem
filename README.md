# Alarm system project
A project made for the IoT Security and Data Security courses.

# 1. Mosquitto installation
```sh
sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa
sudo apt update
sudo apt install mosquitto
```
After the installation, the Mosquitto service should start automatically.

## Enable TLS

Generate a Certificate Authority (CA) certificate and key.

### Certificate Authority
```sh
openssl genrsa -out ca.key 2048
openssl req -x509 -new -nodes -key ca.key -sha256 -days 3650 -out ca.crt
```
### Server

Generate a server key.

**Note:** the `openssl.cnf` file is provided within this repository's root directory.
```sh
openssl genrsa -out server.key 2048
```
Generate a certificate signing request to send to the CA.
```sh
openssl req -new -key server.key -config openssl.cnf -out server.csr
```

Obtain a signed certificate from the CA using its key:
```sh
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 3650 -sha256 -extensions v3_req -extfile openssl.cnf
```

### Client

Generate a client key.
```sh
openssl genrsa -out client.key 2048
```
Generate a certificate signing request to send to the CA.
```sh
openssl req -new -key client.key -out client.csr
```
Obtain a signed certificate from the CA using its key:

```sh
openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 3650
```

### Configure Mosquitto to enable TLS
Create a configuration file:

```sh
sudo nano /etc/mosquitto/conf.d/custom.conf
```

And paste in the following lines:

```sh
allow_anonymous true
listener 8883
cafile /etc/mosquitto/ca_certificates/ca.crt
certfile /etc/mosquitto/certs/server.crt
keyfile /etc/mosquitto/certs/server.key
require_certificate true
```

Save and close the file.

Now move the previously created files (certificates and keys) to the following directories:

```sh
# Move the CA certificate inside the 'ca_certificates' directory
sudo mv ca.crt /etc/mosquitto/ca_certificates/ca.crt
```

```sh
# Move the server certificate inside the 'certs' directory
sudo mv server.crt /etc/mosquitto/certs/server.crt
```

```sh
# Move the server key inside the 'certs' directory
sudo mv server.key /etc/mosquitto/certs/server.key
```

### Change file permissions

In order for Mosquitto to be able to access to these files, we need to change some permissions:

```sh
sudo chown root: /etc/mosquitto/ca_certificates/ca.crt
sudo chmod 644 /etc/mosquitto/ca_certificates/ca.crt
```

```sh
sudo chown mosquitto: /etc/mosquitto/certs/server.crt
sudo chmod 600 /etc/mosquitto/certs/server.crt
```
```sh
sudo chown mosquitto: /etc/mosquitto/certs/server.key
sudo chmod 600 /etc/mosquitto/certs/server.key
```

### Restart Mosquitto
```sh
sudo systemctl restart mosquitto
```

# 2. Setup Arduino UNO and ESP32 boards using Arduino IDE

## Installing needed libraries

Two libraries needs to be installed (both can be searched withing Arduino IDE using the Library Manager):

1. [MQTT](https://github.com/256dpi/arduino-mqtt) by Joel Gaehwiler
2. [ArduinoJson](https://arduinojson.org/) by Benoit Blanchon

## Upload sketches

Upload the `arduino.ino` sketch to the Arduino UNO board and the `esp32_tls.ino` sketch to the ESP32 board.

Fill in the information written in the file `arduino_secrets.h` provided within the `esp32_tls` directory.

# 3. Hardhat

After cloning this repository, move inside the `hardhat` directory and install all dependencies.

```sh
cd hardhat
npm install
```

### Start the Hardhat Network

```sh
npx hardhat node
```

### Deploy the contract

```sh
npx hardhat run --network localhost scripts/deploy.js
```

## Run the script

```sh
npx hardhat run --network localhost scripts/index.js
```
