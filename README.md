# secure_network_application

This project is a lightweight, custom client-server networking prototype written in C. It was designed to demonstrate secure data transmission, custom network protocol design, and data integrity validation over UDP, simulating the transmission of live media frames between two systems.

## 🚀 Key Features

*   **Custom Packet Protocol:** Uses a tightly packed C-struct (`#pragma pack`) to organize data (headers, auth tokens, payload) with zero serialization overhead.
*   **Data Integrity (CRC32):** Implements a standard IEEE 802.3 CRC32 algorithm to catch network noise and corrupted packets.
*   **Encrypted Payloads:** Secures the "media" data in transit using a symmetric XOR stream cipher.
*   **Authentication:** Requires an authorization token to prevent unverified clients from sending data to the server.
*   **Two-Way Handshake:** The client sends an encrypted payload, and upon successful decryption and validation, the server responds with an ACK packet.

## 📁 Project Structure

*   `data_packet.h`: The shared library containing the custom packet struct, the CRC32 algorithm, and the encryption logic.
*   `udp_recv.c`: The Server program. It listens for incoming UDP packets, verifies the CRC, checks authentication, decrypts the payload, and sends an ACK back.
*   `udp_send.c`: The Client program. It packages the user's message into the custom struct, encrypts it, calculates the checksum, and fires it over the network.
*   `Makefile`: The build script to easily compile the project.

## 🛠️ Prerequisites

To build and run this project, you only need a standard C compiler (`gcc`) and `make` installed on a Linux/Unix environment (or WSL on Windows). No heavy external libraries (like OpenSSL or Protobuf) are required!

## ⚙️ How to Build

Open your terminal in the project directory and run:

```bash
make

./udp_recv 8080

./udp_send 127.0.0.1 8080 "Hello from the Client!"
