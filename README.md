# transfur

A cross platform file transfer software

Our goal is to be able to transfer information from any system to any other system with ease, regardless of system, speed, and age.  

## Quick Project Update

The project is now in an MVP state. TUI/basic GUI still work-in-progress.

## User Interfaces
We currently have a cross-platform GUI tested and working on Linux (WSL2 and Native) powered by Nuklear.

A TUI & GUI that only need the bare minimum features to run.

## Information Transfer

Current modes of information transfer:
- LAN transfer: Supported by Windows and Linux
- Serial transfer: Supported by Windows and Linux

Future planned modes of information transfer:
- Bluetooth transfer
- P2P transfer: meaning if the devices are close enough, one can establish itself a local network allowing other peers to connect to it without Routers or the Internet

Tested and working:
- LAN has been tested with loopback and with local devices on Windows and Linux (WSL2 Ubuntu)
- File has been tested on Windows and Linux (WSL2 Ubuntu)
- Serial has been tested on Linux (WSL2 Ubuntu)

Future tests:
- 1980's laptop running a loopback serial test
- ~~LAN information transfer between 2 local devices~~

## Credits
kaboom (the cool one): implemented all the interfaces and tests and integrated interfaces with gui
jeremiah (inois is ill): ~~didn't do anything 🤣🤣🤣🤣🤣~~ ~~apparently he worked on the non-functional TUI and the first revision of the file interface and is working on GUI~~ did all the tui and gui stuff ✅  
simon (simon): made the ~~entire~~ GUI and placeholder files for us to work on and did some things

## How to Run

Grab a release or build it yourself:

```
make
```

Then run it:

```
./build/Linux/transfur # (or run whatever binary is for your OS)
```

## Usage

For a basic file transfer over LAN, open Transfur on your receiving system, and config it so:  

- Receiving Interface: Click on the Receiving Interface button and select `Receive over LAN`
- Receiving Interface Input: Enter `9000` (your port from where the file would be sent through, you can change this as you wish)

- Sending Interface: Click on the Sending Interface button and select `Send to File`, You can also chain multiple systems with `Send over LAN` or `Send over Serial` as per your use case
- Sending Interface Input: Enter your file path to where you wish to save your file

Then, click Initalize Connection. After that, open Transfur on your sending computer and configure it so:

- Receiving Interface: Click on the Receiving Interface button and select `Read from File`.
- Receiving Interface Input: Enter the file path for the file you wish to Transfur (haha get it)

- Sending Interface: Click on the Sending Interface button and select `Send over LAN`
- Sending Interface Input: Enter the receiving system's IP address and the port selected on the receiving system like this `<ip>:<port>`

Make sure to start the receiving side of the transfur first!

Here's what goes as input for each interface:
- Read/Send from File: File path for your operation
- Read/Send over LAN: `<port>` if this is running via loopback, `<ip>:<port>` if it is over a network
- Read/Send over Serial: `<serial_path>[:<baud>]` if you do not specify baud, it will default to a value of 38400
