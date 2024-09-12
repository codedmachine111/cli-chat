# CLI Chat

CLI Chat is a multithreaded chat server that allows multiple clients to connect simultaneously and exchange messages in real-time.
<p align="center">

  <img src="https://github.com/user-attachments/assets/7dec9a3f-6a74-49be-badd-079968168bd9" alt="cargo-banner" width="700">

</p>

## Features

- A group chat simulation of clients connected on the same network.
- Supports multiple clients.
- Private messaging

## Available commands

1. Whisper - Private message other connected clients.
   - Example usage: ```/whisper 29 Hello James``` 29 is the uid of the user you want to message privately.


## Installation steps

1. - Fork the [repo](https://github.com/codedmachine111/cli-chat)
   - Clone the repo to your local machine `git clone https://github.com/codedmachine111/cli-chat.git`
   - Change current directory `cd cli-chat`
     
2. Compile the source code:
```bash
make all
```

4. How to run
- Run the server code on a machine.
```bash
./server
```

- Run the client code on any other machine or the same machine (different terminal tab/window).
```bash
./client
```

> ### **Note**
> If you are running the server and client of different machines, modify the `SERVER_IP_ADDRESS` macro in both client and server files to the public IP-address of the machine running the server code. (ifconfig it)

## Contribution

Contributions are welcome! If you have any suggestions, improvements, or bug fixes, please submit a pull request or open an issue on the GitHub repository.
