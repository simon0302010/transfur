# transfur
A cross platform file transfer software

Our goal is to be able to transfer information from any system to any other system with ease, regardless of system, speed, and age.  

## User Interfaces
We currently have a cross-platform GUI tested and working on Linux (WSL2 and Native) powered by Nuklear.
~~A TUI is currently work-in-progress by the lazy american~~ Apparently we've moved on to GUI and TUI is just sitting half baked.

Note that the GUI is not yet linked to the information transfer interfaces and is currently seperate.

## Information Transfer

Current modes of information transfer:
- LAN transfer: Supported by Windows and Linux
- Serial transfer: Supported by Windows and Linux

Future planned modes of information transfer:
- Bluetooth transfer
- P2P transfer: meaning if the devices are close enough, one can establish itself a local network allowing other peers to connect to it without Routers or the Internet

Tested and working:
- LAN has been tested and is working on Windows and Linux (WSL2)
- Serial has been tested and is working on Linux (WSL2)

Future tests:
- 1980's laptop running a loopback serial test
- LAN information transfer between 2 local devices 

## Credits
kaboom (the cool one): implemented all the interfaces and tests  
jeremiah (inois is ill): ~~didn't do anything 🤣🤣🤣🤣🤣~~ apparently he worked on the non-functional TUI and the first revision of the file interface and is working on GUI  
simon (simon): made the entire GUI and placeholder files for us to work on, also guided us :3