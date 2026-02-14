\# Qt IPC Server-Client



Qt project showing local socket communication between two programs.



\## What does it do?



\- Server listens for connections

\- Client connects and sends a message

\- Server reverses the message and sends it back

\- Client displays the result



\## Requirements



\- Qt 6

\- C++17 compiler



\## Build and run



1\. Build Server project in Qt Creator

2\. Build Client project in Qt Creator

3\. Run Server first

4\. Run Client and type a message



\## Example

```

Client input: test message

Server response: egassem tset

```



\## Files



\- `Server/` - server application

\- `Client/` - client application



\## How the protocol works



Messages have a 4-byte header with the length, followed by the actual text.

```

\[4 bytes: length]\[message data]

```



This prevents problems when data arrives in chunks.



\## Notes



Made this to learn Qt's `QLocalServer` and `QLocalSocket` classes. Works on Windows, Linux, and macOS.



The server can handle multiple clients connecting at the same time.

